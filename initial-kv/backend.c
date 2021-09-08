#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "dll.h"

int logger_put(void* ctx, int key, char* val) {
	printf("l: put(%d, %s)\n", key, val);
	return 0;
}

char* logger_get(void* ctx, int key) {
	printf("l: get(%d)\n", key);
	return NULL;
}

int logger_del(void* ctx, int key) {
	printf("l: del(%d)\n", key);
	return 0;
}

int logger_clear(void* ctx) {
	printf("l: clear\n");
	return 0;
}

backend_t init_logger_backend() {
	backend_t b = {
		.put = logger_put,
		.get = logger_get,
		.del = logger_del,
		.clear = logger_clear,
	};
	return b;
}

typedef struct list_backend {
	list kvs;
} list_backend_t;

typedef struct kv_pair {
	int key;
	char* val;
} kv_pair_t;

void kv_pair_dtor(void* data) {
	kv_pair_t* pair = data;
	if (pair) {
		free(pair->val);
		free(pair);
	}
}

bool kv_pair_eq(const void* a_, const void* b_) {
	const kv_pair_t* a = a_;
	const kv_pair_t* b = b_;

	return a->key == b->key;
}

void pair_node_printer(const list_node* node) {
	const kv_pair_t* pair = node->data;
	if (pair) {
		printf("%d: %s\n", pair->key, pair->val);
	}
}

int bckend_list_put(void* ctx_, int key, char* val) {
	list_backend_t* ctx = ctx_;
	kv_pair_t* pair = malloc(sizeof(kv_pair_t));
	if (!pair) {
		perror("bckend_list_put: malloc()");
		exit(1);
	}
	pair->key = key;
	pair->val = strdup(val);

	// find any existing node with this key and remove
	list_node* node = list_find(&ctx->kvs, kv_pair_eq, pair);
	if (node) {
		list_remove(&ctx->kvs, node);
	}

	// insert new node
	list_rpush(&ctx->kvs, list_node_new(pair));

	return 0;
}

char* bckend_list_get(void* ctx_, int key) {
	list_backend_t* ctx = ctx_;

	kv_pair_t query_pair = {.key = key, .val = NULL};

	list_node* node = list_find(&ctx->kvs, kv_pair_eq, &query_pair);
	if (node) {
		return ((kv_pair_t*)node->data)->val;
	}

	return NULL;
}

int bckend_list_del(void* ctx_, int key) {
	list_backend_t* ctx = ctx_;

	kv_pair_t query_pair = {.key = key, .val = NULL};

	// find node with this key and remove
	list_node* node = list_find(&ctx->kvs, kv_pair_eq, &query_pair);
	if (node) {
		list_remove(&ctx->kvs, node);
		return 0;
	}

	return -1;
}

int bckend_list_clear(void* ctx_) {
	list_backend_t* ctx = ctx_;

	list_empty(&ctx->kvs);

	return 0;
}

int bckend_list_all(void* ctx_) {
	list_backend_t* ctx = ctx_;
	
	list_iter iter = list_iter_new(&ctx->kvs, ITER_FWD);
	list_node* node;
	while ((node = list_iter_yield(&iter))) {
		kv_pair_t* pair = node->data;
		printf("%d,%s\n", pair->key, pair->val);
	}


	return 0;
}

int bckend_list_quit(void* ctx_) {
	list_backend_t* ctx = ctx_;

	FILE* f_snapshot = fopen("kv.snapshot", "w");
	if (!f_snapshot) {
		perror("bckend_list_quit: fopen()");
		exit(1);
	}

	list_iter iter = list_iter_new(&ctx->kvs, ITER_FWD);
	list_node* node;
	while ((node = list_iter_yield(&iter))) {
		kv_pair_t* pair = node->data;
		fprintf(f_snapshot, "%d,%s\n", pair->key, pair->val);
	}

	fclose(f_snapshot);

	list_empty(&ctx->kvs);
	free(ctx);

	return 0;
}

void snapshot_hydrate(backend_t backend) {
	FILE* f_snapshot = fopen("kv.snapshot", "r");
	if (!f_snapshot) {
		// if there's no file, that's fine; we just don't have a snapshot to restore from.
		return;
	}

	char* line = NULL;
	size_t len = 0;
	ssize_t n_read;
	while ((n_read = getline(&line, &len, f_snapshot)) != -1) {
		char* tok_end = line;
		char* key = strsep(&tok_end, ",");
		char* val = tok_end;
		if (val[strlen(val) - 1] == '\n') val[strlen(val) - 1] = 0;

		if (val != NULL) {
			backend.put(backend.ctx, atoi(key), val);
		}
	}
	free(line);

	fclose(f_snapshot);
}

backend_t init_list_backend() {
	list_backend_t* backend_ctx = malloc(sizeof(list_backend_t));
	if (!backend_ctx) {
		perror("init_list_backend: malloc()");
		exit(1);
	}

	list_init(&backend_ctx->kvs);
	backend_ctx->kvs.dtor = kv_pair_dtor;

	backend_t b = {
		.ctx = backend_ctx,
		.put = bckend_list_put,
		.get = bckend_list_get,
		.del = bckend_list_del,
		.clear = bckend_list_clear,
		.all = bckend_list_all,

		.quit = bckend_list_quit,
	};

	// hydrate from snapshot
	snapshot_hydrate(b);

	return b;
}