#include <stdio.h>

#include "backend.h"

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

Backend logger_backend() {
	Backend b = {
		.put = logger_put,
		.get = logger_get,
		.del = logger_del,
		.clear = logger_clear,
	};
	return b;
}