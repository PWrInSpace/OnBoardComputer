#ifndef QUEUE_ROCKET_H
#define QUEUE_ROCKET_H

#ifdef _cplusplus
extern "C"{
#endif

typedef struct Queue{
  char *data;
  struct Queue *next_elem;
}t_queue;

void init(t_queue *elem);

void push(t_queue **elem, char *data);

char *pop(t_queue **queue);

void print(t_queue * elem);

#ifdef _cplusplus
}
#endif

#endif