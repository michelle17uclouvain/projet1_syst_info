//inspiration du code fourni dans les slides du cours (Leçon 7) 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 8 // places dans le buffer
#define TOTAL_ITEMS 131072 // nombre total d'éléments

// Ressources partagées
static int buffer[N]; // buffer partagé 
static int in_index = 0; // prochaine case où insérer
static int out_index = 0; // prochaine case où retirer

static pthread_mutex_t mutex;
static sem_t empty; // places libres
static sem_t full; // places occupées

typedef struct {
    int id; // identifiant du producteur
    int count; // nombre d'éléments à produire / consommer
} thread_arg_t;

// Gestion d'erreur
static void die(int err, const char *msg) {
    fprintf(stderr, "Erreur %d dans %s\n", err, msg);
    exit(EXIT_FAILURE);
}

// Simulation traitement CPU 
static void process(void) {
    for (int i = 0; i < 10000; i++);
}

// Inserer un élément dans le buffer
static void insert_item(int item) {
    buffer[in_index] = item;
    in_index = (in_index + 1) % N;
}

// Retirer un élément du buffer
static int remove_item(void) {
    int item = buffer[out_index];
    out_index = (out_index + 1) % N;
    return item;
}

// Producteur
static void *producer(void *param) {
    thread_arg_t *arg = (thread_arg_t *) param;

    for (int i = 0; i < arg->count; i++) {
        process(); // production simulée
        sem_wait(&empty);  // attente d'une place libre
        pthread_mutex_lock(&mutex);
        // section critique
        insert_item(arg->id);
        pthread_mutex_unlock(&mutex);
        sem_post(&full); // une place remplie en plus
    }
    return NULL;
}

// Consommateur
static void *consumer(void *param) {
    thread_arg_t *arg = (thread_arg_t *) param;

    for (int i = 0; i < arg->count; i++) {
        sem_wait(&full);  // attente d'une place remplie
        pthread_mutex_lock(&mutex);
        // section critique
        int item = remove_item();
        (void) item; // la valeur n'est pas utilisée
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // une place libre en plus
        process(); // consommation simulée
    }
    return NULL;
}

// Répartir TOTAL_ITEMS entre n threads 
static void distribute(thread_arg_t *args, int n) {
    int base = TOTAL_ITEMS / n;
    int rest = TOTAL_ITEMS % n;
    for (int i = 0; i < n; i++) {
        args[i].id = i + 1;
        args[i].count = base + (i < rest ? 1 : 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <nb_producteurs> <nb_consommateurs>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int nprod = atoi(argv[1]);
    int ncons = atoi(argv[2]);
    if (nprod <= 0 || ncons <= 0) {
        fprintf(stderr, "Les nombres de threads doivent être strictement positifs\n");
        return EXIT_FAILURE;
    }

    pthread_t prod[nprod], cons[ncons];
    thread_arg_t pargs[nprod], cargs[ncons];
    distribute(pargs, nprod);
    distribute(cargs, ncons);

    int err;
    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0) die(err, "pthread_mutex_init");
    err = sem_init(&empty, 0, N); // buffer vide : N places libres
    if (err != 0) die(err, "sem_init(empty)");
    err = sem_init(&full, 0, 0); // aucune place remplie
    if (err != 0) die(err, "sem_init(full)");

    for (int i = 0; i < nprod; i++) {
        err = pthread_create(&prod[i], NULL, producer, &pargs[i]);
        if (err != 0) die(err, "pthread_create(producer)");
    }
    for (int i = 0; i < ncons; i++) {
        err = pthread_create(&cons[i], NULL, consumer, &cargs[i]);
        if (err != 0) die(err, "pthread_create(consumer)");
    }

    for (int i = 0; i < nprod; i++) {
        err = pthread_join(prod[i], NULL);
        if (err != 0) die(err, "pthread_join(producer)");
    }
    for (int i = 0; i < ncons; i++) {
        err = pthread_join(cons[i], NULL);
        if (err != 0) die(err, "pthread_join(consumer)");
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}