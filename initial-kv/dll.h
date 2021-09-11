// doubly-linked list
// erin marshall <limarshall@wisc.edu>
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

//! Node in a doubly-linked list. Stores a `void*` pointer to node data.
typedef struct list_node {
	struct list_node* prev;
	struct list_node* next;
	void*     data;
} list_node;

//! Every \ref list may have an associated node destructor function pointer,
//! which is called immediately before freeing nodes, whenever that may happen.
//! This is the type of that function pointer.
typedef void node_data_dtor(void* data);

//! Represents a doubly-linked list.
//!
//! Additionally, optionally encapsulates destruction behavior (\ref #dtor).
//! If \ref #dtor is non-`NULL`, it will be executed on a node's data
//! immediately before freeing the node, anywhere that may happen;
//! this replicates the semantics of an owning collection when it exists,
//! and a borrowing collection when it does not.
typedef struct list {
	list_node* head;
	list_node* tail;
	size_t size;

	// dtor for node data; if not NULL, this list is owning and will
	// destroy its nodes' data when list_empty/list_destroy() are called.
	node_data_dtor* dtor;
} list;

//! Initializes a \ref list which has already been allocated.
void list_init(list* self);
//! Allocates, initializes, and returns a new \ref list.
list* list_new();
//! Allocates, initializes, and returns a new \ref list_node, with the given `data`.
list_node* list_node_new(void* data);
//! Sets a \ref list's node dtor.
void list_set_dtor(list* self, node_data_dtor* dtor);
//! Empties the given \ref list, unlinking, destroying, and freeing all nodes.
void list_empty(list*);
//! Empties (\ref list_empty) the given \ref list, and then frees it.
void list_destroy(list*);

//! Pushes a \ref list_node to the rhs of a given \ref list.
list_node* list_rpush(list* self, list_node* node);
//! Pushes a \ref list_node to the lhs of a given \ref list.
list_node* list_lpush(list* self, list_node* node);
//! Pops a \ref list_node from the rhs of a given \ref list.
list_node* list_rpop(list* self);
//! Pops a \ref list_node from the lhs of a given \ref list.
list_node* list_lpop(list* self);

//! Indexes a \ref list Python-style, returning a node.
//! Negative indices count from the tail of the list, -1 corresponding to the last element.
list_node* list_index(list* self, ssize_t index);

//! Removes a given \ref list_node from the \ref list.
void list_remove(list* self, list_node* node);

//! Type of comparison functions used to find a list element based on its data.
typedef bool list_val_eq(const void* a, const void* b);
//! Find a list element given a data `val` to search for and a `compar`ison function.
list_node* list_find(list* self, list_val_eq* compar, const void* val);

//! Type of functions which print the contents of a single \ref list_node, `\n`-suffixed.
typedef void node_printer(const list_node* node);
//! Print all elements of a list, using the given `printer` to display nodes.
void list_debug(list* self, node_printer* printer);

//! Direction in which a list iterator will proceed.
typedef enum list_iter_dir {
	ITER_FWD,
	ITER_REV,
} list_iter_dir;
//! Iterator over a doubly-linked list.
typedef struct list_iter {
	list_node* next;
	list_iter_dir dir;
} list_iter;
//! "Constructs" a new \ref list_iter by-value, given a list and a direction to iterate it in.
list_iter list_iter_new(list* list, list_iter_dir dir);
//! Yields an element from a \ref list_iter, and advances it. Yields `NULL` when iterator is exhausted.
list_node* list_iter_yield(list_iter* self);