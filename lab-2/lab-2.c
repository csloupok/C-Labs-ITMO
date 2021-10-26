#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const int base = 1000000000;

typedef struct {
    size_t size;
    int32_t *data;
} uint1024_t;

void printf_value(uint1024_t x) {
    if (x.size != 0)
        printf("%d", x.data[x.size - 1]);
    else
        printf("%d", 0);
    for (int i = x.size - 2; i >= 0; --i)
        printf("%09d", x.data[i]);
}

int main() {
    return 0;
}