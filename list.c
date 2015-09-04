#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	int item;
} node_t;

typedef struct list {
	node_t *head;
	node_t *tail;
} list_t;

int insert_front(list_t **list, int item)
{
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL)
		return -1;

	node_t *tmp;

	new->item = item;
	if ((*list)->head == NULL) {
		printf("First item ever\n");
		(*list)->head = new;
		(*list)->head->prev = NULL;
		(*list)->head->next = NULL;
		(*list)->tail = (*list)->head;
	} else {
		tmp = new;
		tmp->prev = NULL;
		tmp->next = (*list)->head;
		(*list)->head->prev = tmp;
		(*list)->head = tmp;
	} 
	
	return 0;
}

int insert_end(list_t **list, int item)
{
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL)
		return -1;
	node_t *tmp;
	
	new->item = item;
	if ((*list)->head == NULL) {
		(*list)->head = new;
		(*list)->head->prev = NULL;
		(*list)->head->next = NULL;
		(*list)->tail = (*list)->head;
	} else {
		for ((*list)->tail = (*list)->head; (*list)->tail != NULL;) {
			tmp = (*list)->tail;
			(*list)->tail = (*list)->tail->next;
		}
		(*list)->tail = new; 
		tmp->next = (*list)->tail; 
		(*list)->tail->prev = tmp;
		(*list)->tail->next = NULL;
	}

	return 0;
}

int main(void)
{
	list_t *list;
	node_t *current;
	list->head = NULL;
	list->tail = NULL;

	insert_end(&list, 1);
	insert_end(&list, 2);
	insert_end(&list, 3);
	insert_end(&list, 4);
	
	for (current = list->tail; current != NULL; current = current->prev)
		printf("%d\n", current->item);
	
	
	
}

			
		


	
		
		
	

