#pragma once

typedef struct Backend {
	void* ctx;
	int   (*put)(void* ctx, int key, char* val);
	char* (*get)(void* ctx, int key);
	int   (*del)(void* ctx, int key);
	int   (*clear)(void* ctx);
	// int (*all)(void* ctx);
} Backend;

Backend logger_backend();