#pragma once

typedef struct list_node
{
    struct list_node *next;
    struct list_node *prev;
    void *data;
} list_node_t;

typedef struct list
{
    list_node_t *head;
    list_node_t *tail;
} list_t;

void list_init(list_t *list);
void list_enqueue(list_t *list, list_node_t *data);
list_node_t *list_dequeue(list_t *list);
void list_remove(list_t *list, list_node_t *node);

list_node_t *list_find(list_t *list, int (*predicate)(const void *));
void list_foreach(list_t *list, void (*func)(list_node_t *));