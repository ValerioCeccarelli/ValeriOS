#include "list.h"

void list_init(list_t *list)
{
    list->head = 0;
    list->tail = 0;
}

void list_enqueue(list_t *list, list_node_t *data)
{
    if (list->head == 0)
    {
        list->head = data;
        list->tail = data;
        data->next = 0;
    }
    else
    {
        list->tail->next = data;
        list->tail = data;
        data->next = 0;
    }
}

list_node_t *list_dequeue(list_t *list)
{
    list_node_t *data = list->head;
    if (data != 0)
    {
        list->head = data->next;
        if (list->head == 0)
        {
            list->tail = 0;
        }
    }
    return data;
}

void list_remove(list_t *list, list_node_t *node)
{
    if (node == list->head)
    {
        list->head = node->next;
    }
    if (node == list->tail)
    {
        list->tail = node->prev;
    }
    if (node->prev != 0)
    {
        node->prev->next = node->next;
    }
    if (node->next != 0)
    {
        node->next->prev = node->prev;
    }
}

list_node_t *list_find(list_t *list, int (*predicate)(const void *))
{
    list_node_t *node = list->head;
    while (node != 0)
    {
        if (predicate(node->data))
        {
            return node;
        }
        node = node->next;
    }
    return 0;
}

void list_foreach(list_t *list, void (*func)(list_node_t *))
{
    list_node_t *node = list->head;
    while (node != 0)
    {
        list_node_t *next = node->next;
        func(node);
        node = next;
    }
}

int list_is_empty(list_t *list)
{
    return list->head == 0;
}