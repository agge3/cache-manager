#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv) {
	uint64_t size = 8ull << 30;
	char *p = malloc(size);
	for (uint64_t i = 0; i < size; ++i) {
		p[i] = 1;
	}
	while (1);
	return 0;
}
