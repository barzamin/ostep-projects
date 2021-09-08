// doubly-linked list
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct list_node {
	struct list_node* prev;
	struct list_node* next;
	void*     data;
} list_node;

typedef void node_data_dtor(void* data);

typedef struct list {
	list_node* head;
	list_node* tail;
	size_t size;

	// dtor for node data; if not NULL, this list is owning and will
	// destroy its nodes' data when list_empty/list_destroy() are called.
	node_data_dtor* dtor;
} list;

void list_init(list* self);
list* list_new();
list_node* list_node_new(void* data);
void list_node_destroy(list_node*, node_data_dtor*);
void list_set_dtor(list* self, node_data_dtor* dtor);
void list_empty(list*);
void list_destroy(list*);

list_node* list_rpush(list* self, list_node* node);
list_node* list_lpush(list* self, list_node* node);
list_node* list_rpop(list* self);
list_node* list_lpop(list* self);

list_node* list_index(list* self, ssize_t index);

void list_remove(list* self, list_node* node);

typedef bool list_val_eq(const void* a, const void* b);
list_node* list_find(list* self, list_val_eq* compar, const void* val);

typedef void node_printer(const list_node* node);
void list_debug(list* self, node_printer* printer);

typedef enum list_iter_dir {
	ITER_FWD,
	ITER_REV,
} list_iter_dir;
typedef struct list_iter {
	list_node* next;
	list_iter_dir dir;
} list_iter;
list_iter list_iter_new(list* list, list_iter_dir dir);
list_node* list_iter_yield(list_iter* self);