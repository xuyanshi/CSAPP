#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t a, b;

void *t1(void *arg) {
  while (1) {
    sem_wait(&a);
    printf("b");
    fflush(stdout);
    sem_post(&b);
  }
}

void *t2(void *arg) {
  while (1) {
    sem_wait(&b);
    printf("ee");
    fflush(stdout);
    sem_post(&a);
  }
}

int main() {
  pthread_t tid1, tid2;
  sem_init(&a, 0, 1);
  sem_init(&b, 0, 0);
  pthread_create(&tid1, NULL, t1, NULL);
  pthread_create(&tid2, NULL, t2, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  sem_destroy(&a);
  sem_destroy(&b);
  return 0;
}