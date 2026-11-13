#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// Variables globales pour stocker les pointeurs (allocation dynamique)
pthread_t *phil;
pthread_mutex_t *baguette;
int nb_philosophes;


void mange(int id) {
  printf("Philosophe [%d] mange\n",id);
  for(int i=0;i< rand(); i++) {
    // philosophe mange
  }
}

void* philosophe ( void* arg )
{
  int *id=(int *) arg;
  int left = *id;
  int right = (left + 1) % nb_philosophes;
  while(true) {
    // philosophe pense
    if(left<right) {
      pthread_mutex_lock(&baguette[left]);
      pthread_mutex_lock(&baguette[right]);
    }
    else {
      pthread_mutex_lock(&baguette[right]);
      pthread_mutex_lock(&baguette[left]);
    }
    mange(*id);
    pthread_mutex_unlock(&baguette[left]);
    pthread_mutex_unlock(&baguette[right]);
    usleep(500000); // simulation du temps de penser 
  }
  return (NULL);
}

int main(int argc, char *argv[])
{
  // Vérification de l'argument
    if (argc != 2) {
        printf("Utilisation : %s <nombre_de_philosophes>\n", argv[0]);
        return 1;
    }

    nb_philosophes = atoi(argv[1]);
    if (nb_philosophes < 2) {
        printf("Il faut au moins 2 philosophes.\n");
        return 1;
    }

    // Allocation dynamique des tableaux selon le nombre choisi
    phil = malloc(nb_philosophes * sizeof(pthread_t));
    baguette = malloc(nb_philosophes * sizeof(pthread_mutex_t));
    int *ids = malloc(nb_philosophes * sizeof(int));

    for(int i=0; i<nb_philosophes; i++){
        pthread_mutex_init(&baguette[i],NULL);
    }

    for(int i = 0; i < nb_philosophes; i++) {
        ids[i] = i;
        pthread_create(&phil[i], NULL, philosophe, &ids[i]);
    }
    sleep(3);// les threads vient leurs vies

    // Libération propre (même si dans un test simple le système le fait à la fermeture)
    free(phil);
    free(baguette);
    free(ids);
    return 0;
}