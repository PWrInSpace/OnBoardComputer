#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue{
  char *data;
  struct Queue *next_elem;
}t_queue;

void init(t_queue *elem);

void add(t_queue **elem, char *data);

char *pop(t_queue **queue);

void print(t_queue * elem);

#endif