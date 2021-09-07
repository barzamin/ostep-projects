#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"

void exec_op_str(Backend backend, char* opstr) {
	char* buf = strdup(opstr);
	char* tokend = buf;

	char* cmd = strsep(&tokend, ",");
	if (cmd == NULL || strlen(cmd) == 0 || strlen(cmd) > 1) {
		printf("bad command\n");
		free(buf); return;
	}

	switch (cmd[0]) {
		case 'p': {
			printf("put\n");
			char* key = strsep(&tokend, ",");
			if (key == NULL) {
				printf("bad command\n");
				break;
			}

			char* val = strsep(&tokend, ",");
			if (val == NULL) {
				printf("bad command\n");
				break;
			}

			backend.put(backend.ctx, atoi(key), val);

			break;
		}
		
		case 'g': {
			char* key = strsep(&tokend, ",");
			if (key == NULL) {
				printf("bad command\n");
				break;
			}

			char* val = backend.get(backend.ctx, atoi(key));
			if (val == NULL) {
				printf("%d not found\n", atoi(key));
				break;
			}
			printf("%d,%s\n", atoi(key), val);
			
			break;
		}
		
		case 'd': {
			char* key = strsep(&tokend, ",");
			if (key == NULL) {
				printf("bad command\n");
				break;
			}

			backend.del(backend.ctx, atoi(key));
			
			break;
		}
		
		case 'c': {
			backend.clear(backend.ctx);
			
			break;
		}
		
		case 'a': {
			printf("ALL unimplemented\n");
			exit(-1);
			break;
		}
		
		default:
			printf("bad command\n");
			break;
	}
	free(buf);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		return 0; // do nothing if no args
	}

	Backend backend = logger_backend();

	for (int i = 1; i < argc; i++) {
		exec_op_str(backend, argv[i]);
	}
}