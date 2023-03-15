#ifndef GLASSES2_LIST_H_
#define GLASSES2_LIST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct list_node list_node;

struct list_node {
	char      *item;
	list_node *next; // NULL marks this as the end;
	list_node *prev; // NULL marks this as the beginning.
};

#define PRINT_LIST_FROM(list)       \
do {                                \
list_node *current_enum = (list);   \
for (;;) {                          \
printf("%s\n", current_enum->item); \
current_enum = current_enum->next;  \
if (!current_enum) break;           \
}                                   \
} while (0);

#define PRINT_LIST_ALL(list) PRINT_LIST_FROM(seek_beginning((list)))

list_node *at_index_from(list_node *origin, int index);
int        get_index_from_beginning(list_node *node);

// Assumes to_insert has sufficient scope; essentially must be malloced.
list_node *insert_after(list_node *node, const char *to_insert);
list_node *insert_before(list_node *node, const char *to_insert);

list_node *seek_beginning(list_node *node);
list_node *seek_end(list_node *node);

int count_nodes(list_node *node);

void remove_node(list_node *node);
void remove_node_free(list_node *node, bool also_free_item);

#endif // GLASSES2_LIST_H_
