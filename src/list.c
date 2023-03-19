#include "include/list.h"

list_node *at_index_from(list_node *origin, int index) {
	if (!origin)
		return NULL;


	list_node *node = origin;

	// If index is negative, traverse backwards.
	if (index < 0) {
		for (int i = 0; i > index; --i)
			if (node->prev)
				node = node->prev;
			else
				return NULL; // Error if index is out of range.

		return node;
	}

	for (int i = 0; i < index; ++i)
		if (node->next)
			node = node->next;
		else
			return NULL; // Error if index is out of range.

	return node;
}

list_node *insert_after(list_node *node, const char *to_insert) {
	if (!node)
		return NULL;

	// Create new node.
	list_node *left = node;
	list_node *right = left->next;
	list_node *insert = malloc(sizeof(list_node));
	if (!insert)
		return NULL;
	insert->item = to_insert;

	// Link them.
	left->next = insert;
	insert->prev = left;
	insert->next = right;
	if (right)
		right->prev = insert;

	return insert;
}

list_node *insert_before(list_node *node, const char *to_insert) {
	if (!node)
		return NULL;

	// Create new node.
	list_node *right = node;
	list_node *left = right->prev;
	list_node *insert = malloc(sizeof(list_node));
	if (!insert)
		return NULL;
	insert->item = to_insert;

	// Link them.
	right->prev = insert;
	insert->next = right;
	insert->prev = left;
	if (left)
		left->next = insert;

	return insert;
}

list_node *seek_beginning(list_node *node) {
	if (!node)
		return NULL;

	// Already at the beginning.
	if (node->prev == NULL)
		return node;

	list_node *begin = node;

	// Traverse the list, stopping at the beginning.
	while (begin->prev != NULL)
		begin = begin->prev;

	return begin;
}

list_node *seek_end(list_node *node) {
	if (!node)
		return NULL;

	// Already at the end.
	if (node->next == NULL)
		return node;

	list_node *end = node;

	// Traverse the list, stopping at the end.
	while (end->next != NULL)
		end = end->next;

	return end;
}

int count_nodes(list_node *node) {
	if (!node)
		return 0;

	// Start from the beginning.
	list_node *beginning = seek_beginning(node);

	int count = 0;
	do {
		++count;
	} while ((beginning = beginning->next) != NULL);

	return count;
}

int get_index_from_beginning(list_node *node) {
	if (!node)
		return 0;

	list_node *beginning = seek_beginning(node);

	int count = 0;
	do {
		++count;
		if (beginning == node)
			break;
	} while ((beginning = beginning->next) != NULL);

	return count;
}

void remove_node(list_node *node) {
	if (!node)
		return;

	list_node *left = node->prev;
	list_node *right = node->next;

	if (left)
		left->next = right;
	if (right)
		right->prev = left;
}

void remove_node_free(list_node *node, bool also_free_item) {
	if (!node)
		return;

	remove_node(node);

	if (also_free_item)
		free(node->item);

	free(node);
}
