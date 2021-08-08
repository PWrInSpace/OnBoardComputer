#include <queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init(t_queue *elem){
  elem = NULL;
}

void push(t_queue **queue, char *data){
  t_queue *temp, *new_element; 
  new_element = (t_queue*)malloc(sizeof(t_queue) + sizeof(data));
  new_element->data = malloc(sizeof(data));
  strcpy(new_element->data, data);
  
  new_element->next_elem = NULL;

  if(*queue == NULL){
    *queue = new_element;
    return;
  }
  
  temp = *queue;
  while (temp->next_elem != NULL)
  {
    temp = temp->next_elem;
  }
  
  temp->next_elem = new_element;
}

char *pop(t_queue **queue){
  t_queue *next;
  char *value = malloc(sizeof((*queue)->data));

  if(*queue == NULL){
    printf("Brak elementów na stosie!");
  }
  next = (*queue)->next_elem;
  strcpy(value, (*queue)->data);
  
  free((*queue)->data);
  free(*queue);
  *queue = next;

  return value;
}


// DEUBUG DO CZYSTEGO C - do wywalenia
void print(t_queue * queue){
  t_queue *current = queue;
  
  if(current == NULL){
    printf("Pusto!");
    return;
  }

  while(current != NULL){
    printf("%s\n", current->data);
    current = current->next_elem;
  }
}
