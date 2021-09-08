#include "dll.h"
#include <stdlib.h>
#include <stdio.h>

void list_init(list* self) {
	self->head = NULL;
	self->tail = NULL;
	self->size = 0;

	self->dtor = NULL;
}

// allocates and initializes a new list
list* list_new() {
	list* l = malloc(sizeof(list));
	if (!l) {
		perror("list_new: malloc()");
		exit(1);
	}

	list_init(l);

	return l;
}

// empties a list, calling each node's dtor in order
void list_empty(list* self) {
	list_node* node = self->head;
	while (node != NULL) {
		list_node* next = node->next;
		if (self->dtor) {
			self->dtor(node->data);
		}
		free(node);
		node = next;
	}
}

// empties and then frees a list
void list_destroy(list* self) {
	list_empty(self);
	free(self);
}

// allocates and initializes a new node with the given data
list_node* list_node_new(void* data) {
	list_node* self = malloc(sizeof(list_node));
	if (!self) {
		perror("list_node_new: malloc()");
		exit(1);
	}

	self->prev = NULL;
	self->next = NULL;
	self->data = data;

	return self;
}

// calls the dtor of a list node and then frees it
void list_node_destroy(list_node* self, node_data_dtor* dtor) {
	if (dtor)
		dtor(self->data);
	free(self);
}

// sets the item dtor for a list
void list_set_dtor(list* self, node_data_dtor* dtor) {
	self->dtor = dtor;
}

// pushes a new tail/right item
list_node* list_rpush(list* self, list_node* n) {
	if (!n) return NULL;

	if (self->size == 0) {  // first insert
		self->head = n;
		self->tail = n;
		n->prev = NULL;
		n->next = NULL;
	} else {
		n->prev = self->tail;
		n->next = NULL;
		self->tail->next = n;
		self->tail = n;
	}

	self->size++;

	return n;
}

// pushes a new head/left item
list_node* list_lpush(list* self, list_node* n) {
	if (!n) return NULL;

	if (self->size == 0) {  // first insert
		self->head = n;
		self->tail = n;
		n->prev = NULL;
		n->next = NULL;
	} else {
		n->next = self->head;
		n->prev = NULL;
		self->head->prev = n;
		self->head = n;
	}

	self->size++;

	return n;
}

// pops the tail/right item
list_node* list_rpop(list* self) {
	if (self->size == 0) return NULL;

	list_node* n = self->tail;
	if (self->size > 1) {
		self->tail = n->prev;
		self->tail->next = NULL;
	}

	// unlink
	n->next = NULL;
	n->prev = NULL;

	self->size--;

	return n;
}

// pops the head/left item
list_node* list_lpop(list* self) {
	if (self->size == 0) return NULL;

	list_node* n = self->head;
	if (self->size > 1) {
		self->head = n->next;
		self->head->prev = NULL;
	}

	// unlink
	n->next = NULL;
	n->prev = NULL;

	self->size--;

	return n;
}

// find a value in the list by using the provided comparison function
list_node* list_find(list* self, list_val_eq* compar, const void* val) {
	list_node* n = self->head;
	while (n != NULL) {
		if (compar(n->data, val)) {
			return n;
		}

		n = n->next;
	}

	return NULL;
}

// index into a list; negative indices index from the end (-1 => last element)
// todo: abstract iterator
list_node* list_index(list* self, ssize_t index) {
	list_iter_dir dir = ITER_FWD;
	if (index < 0) {
		dir = ITER_REV;
		index = -index - 1;
	}

	if (index > self->size) return NULL;

	list_node* node = (dir == ITER_FWD) ? self->head : self->tail;
	for (int i = 0; i < index; i++) {
		if (dir == ITER_FWD)
			node = node->next;
		else
			node = node->prev;
	}

	return node;
}

list_iter list_iter_new(list* list, list_iter_dir dir) {
	list_iter it;
	
	switch (dir) {
		case ITER_FWD: it.next = list->head;
		case ITER_REV: it.next = list->tail;
	}
	it.dir = dir;
	
	return it;
}

list_node* list_iter_yield(list_iter* iter) {
	list_node* curr = iter->next;
	if (curr) {
		switch (iter->dir) {
			case ITER_FWD: iter->next = curr->next;
			case ITER_REV: iter->next = curr->prev;
		}
	}

	return curr;
}

// remove a given node from a list
void list_remove(list* self, list_node* node) {
	if (node->prev)
		node->prev->next = node->next;
	else
		self->head = node->next;

	if (node->next)
		node->next->prev = node->prev;
	else
		self->tail = node->prev;

	if (self->dtor)
		self->dtor(node->data);
	free(node);

	self->size--;
}

// print debug info for a list, using a provided function to display nodes
void list_debug(list* self, node_printer* printer) {
	printf("[list of %ld nodes], head = %p, tail = %p\n",
		self->size, (void*)self->head, (void*)self->tail);
	size_t i = 0;
	list_node* node = self->head;
	while (node != NULL) {
		printf("%ld: ", i);
		printer(node);
		node = node->next;
		i++;
	}
}
