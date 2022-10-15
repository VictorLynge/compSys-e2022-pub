#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <pthread.h>

struct job {
  struct job* next;
  void* assignment;
};

struct job_queue {
  int n;
  int capacity;
  struct job* head;
  int destroyed;

  pthread_mutex_t job_mutex;
  pthread_cond_t full_cond;
  pthread_cond_t empty_cond;
};

// Initialise a job queue with the given capacity.  The queue starts out
// empty.  Returns non-zero on error.
int job_queue_init(struct job_queue *job_queue, int capacity);

// Destroy the job queue.  Blocks until the queue is empty before it
// is destroyed.
int job_queue_destroy(struct job_queue *job_queue);

// Push an element onto the end of the job queue.  Blocks if the
// job_queue is full (its size is equal to its capacity).  Returns
// non-zero on error.  It is an error to push a job onto a queue that
// has been destroyed.
int job_queue_push(struct job_queue *job_queue, void *data);

// Pop an element from the front of the job queue.  Blocks if the
// job_queue contains zero elements.  Returns non-zero on error.  If
// job_queue_destroy() has been called (possibly after the call to
// job_queue_pop() blocked), this function will return -1.
int job_queue_pop(struct job_queue *job_queue, void **data);

#endif
