#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex_current_readers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_current_writers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t z = PTHREAD_MUTEX_INITIALIZER;  // only one waiting reader

sem_t wsem;        // exclusive access to the ressource (init at 1)
sem_t rsem;        // to block readers (init at 1)
volatile size_t current_readers = 0;
volatile size_t current_writers = 0;



void* reader(void* arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&z);
        // only one reader waiting on rsem
        sem_wait(&rsem);
        {
            pthread_mutex_lock(&mutex_current_readers);
            current_readers++;
            if (current_readers == 1) {
                sem_wait(&wsem);
            }
            pthread_mutex_unlock(&mutex_current_readers);
        }
        sem_post(&rsem);  // free next reader/writer
        pthread_mutex_unlock(&z);

        // read ressource...
        for (int i=0; i<10000; i++);

        pthread_mutex_lock(&mutex_current_readers);
        current_readers--;
        if (current_readers == 0) {
            sem_post(&wsem);  // last reader
        }
        pthread_mutex_unlock(&mutex_current_readers);
        break;
    }
    return (NULL);
}

void* writer(void* arg){
    (void)arg;
    while (1) {
        pthread_mutex_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers++;
        if (current_writers == 1) {
            sem_wait(&rsem);
        }
        pthread_mutex_unlock(&mutex_current_writers);
        sem_wait(&wsem);
        // critical section, only one writer at a time
        // write ressource...
        for (int i=0; i<10000; i++);

        sem_post(&wsem);
        pthread_mutex_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers--;
        if (current_writers == 0) {
            sem_post(&rsem);  // last writer
        }
        pthread_mutex_unlock(&mutex_current_writers);
        break;
    }
    return (NULL);
}

int main(int argc, char *argv[])
{
    // test du nbre d'arg
    if (argc != 3) {
        printf("Utilisation : %s <nb_lecteurs> <nb_ecrivains>\n", argv[0]);
        return 1;
    }
    int nbr_r = atoi(argv[1]);
    int nbr_w = atoi(argv[2]);

    // Allocation dynamique des tableaux de threads en fonction des arguments
    pthread_t *readers = malloc(nbr_r * sizeof(pthread_t));
    pthread_t *writers = malloc(nbr_w * sizeof(pthread_t));

    
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);

    for (int i = 0; i < nbr_r; i++)
        pthread_create(&readers[i], NULL, reader, NULL);
    for (int i = 0; i < nbr_w; i++)
        pthread_create(&writers[i], NULL, writer, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(writers[i], NULL);

    // Destruction propre des sémaphores
    sem_destroy(&rsem);
    sem_destroy(&wsem);

    // Libération de la mémoire allouée dynamiquement
    free(readers);
    free(writers);

    return 0;
}