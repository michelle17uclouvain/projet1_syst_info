#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>

pthread_mutex_t mutex_current_readers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_current_writers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t z = PTHREAD_MUTEX_INITIALIZER;  // only one waiting reader

sem_t wsem;        // exclusive access to the ressource (init at 1)
sem_t rsem;        // to block readers (init at 1)
volatile size_t current_readers = 0;
volatile size_t current_writers = 0;



void* reader(void* arg) {
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
}

void* writer(void* arg) {
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
}

int main(void)
{
    pthread_t readers[3], writers[2];
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);

    for (int i = 0; i < 3; i++)
        pthread_create(&readers[i], NULL, reader, NULL);
    for (int i = 0; i < 2; i++)
        pthread_create(&writers[i], NULL, writer, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(writers[i], NULL);

    // Destruction propre des sémaphores
    sem_destroy(&rsem);
    sem_destroy(&wsem);

    return 0;
}