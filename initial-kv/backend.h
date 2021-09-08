#pragma once

typedef struct backend {
	void* ctx;
	int   (*put)(void* ctx, int key, char* val);
	char* (*get)(void* ctx, int key);
	int   (*del)(void* ctx, int key);
	int   (*clear)(void* ctx);
	int   (*all)(void* ctx);

	int   (*quit)(void* ctx);
} backend_t;

backend_t init_logger_backend();
backend_t init_list_backend();