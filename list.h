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

/// @brief This function initializes the list.
/// @param list List to initialize.
void list_init(list_t *list);

/// @brief This function adds a node at the end of the list. Complexity O(1)
/// @param list List to add the node to.
/// @param node Node to add.
void list_enqueue(list_t *list, list_node_t *node);

/// @brief This function removes the first node from the list. Complexity O(1)
/// @param list List to remove the node from.
/// @return The removed node.
list_node_t *list_dequeue(list_t *list);

/// @brief This function removes a node from the list. Complexity O(1)
/// @param list List to remove the node from.
void list_remove(list_t *list, list_node_t *node);

/// @brief This function finds a node in the list that satisfies the predicate. Complexity O(n)
/// @param list List where to search.
/// @param predicate Predicate to satisfy.
list_node_t *list_find(list_t *list, int (*predicate)(const void *));

/// @brief This function iterates over the list and calls the function for each node.
/// @param list List to iterate over.
/// @param func Function to call for each node.
void list_foreach(list_t *list, void (*func)(list_node_t *));

/// @brief This function checks if the list is empty.
/// @param list List to check.
/// @return 1 if the list is empty, 0 otherwise.
int list_is_empty(list_t *list);