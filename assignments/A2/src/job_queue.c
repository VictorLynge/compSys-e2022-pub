#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "job_queue.h"

int job_queue_init(struct job_queue *job_queue, int capacity) {
  if (capacity <= 0)
      return 1;

  job_queue->capacity = capacity;
  job_queue->n = 0;
  job_queue->head = NULL;
  job_queue->destroyed = 0;

  pthread_mutex_init(&job_queue->job_mutex, NULL);
  pthread_cond_init(&job_queue->full_cond, NULL);
  pthread_cond_init(&job_queue->empty_cond, NULL);

  return 0;
}


int job_queue_destroy(struct job_queue *job_queue) {
  assert(pthread_mutex_lock(&job_queue->job_mutex) == 0);
  while (job_queue->n > 0){
    pthread_cond_wait(&job_queue->full_cond, &job_queue->job_mutex);
  }
  
  job_queue->destroyed = 1;

  pthread_cond_broadcast(&job_queue->empty_cond);
  assert(pthread_mutex_unlock(&job_queue->job_mutex) == 0);
  return 0;
}


int job_queue_push(struct job_queue *job_queue, void *data) {
  assert(pthread_mutex_lock(&job_queue->job_mutex) == 0);
  assert(job_queue != NULL);
  if (job_queue->destroyed == 1){
      return -1;
  }
  struct job* j = malloc(sizeof(struct job));
  
  
  while (job_queue->capacity == job_queue->n){
    pthread_cond_wait(&job_queue->full_cond, &job_queue->job_mutex);
  }
  
  j->assignment = data;
  j->next = job_queue->head;
  job_queue->head = j;
  job_queue->n += 1;

  pthread_cond_broadcast(&job_queue->empty_cond);
  assert(pthread_mutex_unlock(&job_queue->job_mutex) == 0);

  return 0;
}

int job_queue_pop(struct job_queue *job_queue, void **data) {
  assert(job_queue != NULL);
  assert(pthread_mutex_lock(&job_queue->job_mutex) == 0);
  
  while (job_queue->n == 0 && job_queue->destroyed != 1){
    pthread_cond_wait(&job_queue->empty_cond, &job_queue->job_mutex);
  }

  if (job_queue->destroyed == 1){
    assert(pthread_mutex_unlock(&job_queue->job_mutex) == 0);
    return -1;
  }
  
  /*
  printf("Popping from queue:");
  struct job* curr_ptr = job_queue->head;
  do {
    printf("%p\n", (void*)curr_ptr);
    curr_ptr = curr_ptr->next;
    } while (curr_ptr->next != NULL);
  printf("Done poppin\n");*/
  

  *data = job_queue->head->assignment;
  
  struct job* temp = job_queue->head->next;
  free(job_queue->head);
  job_queue->head = (struct job*)temp;
  job_queue->n -= 1;

  pthread_cond_broadcast(&job_queue->full_cond);
  assert(pthread_mutex_unlock(&job_queue->job_mutex) == 0);

  return 0;
}
