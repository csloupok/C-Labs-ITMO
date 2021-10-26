//
// Created by Eldar Kasymov on 21.10.2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Реализуем функцию выбора максимального элемента
#define max(a, b) (((a) > (b)) ? (a) : (b))

// Будем работать в системе по основанию миллиард, т.е. каждый элемент массива содержит сразу 9 цифр.
const int base = 1000000000;

typedef struct {
    size_t size;
    int32_t *data;
} uint1024_t;


uint1024_t from_uint(unsigned int x) {
    int big_int_size;
    if (x >= base) big_int_size = 2;
    else big_int_size = 1;
    uint1024_t big_int;
    big_int.data = malloc(big_int_size * sizeof(int32_t));
    big_int.size = big_int_size;
    big_int.data[0] = x % base;
    if (big_int_size > 1) big_int.data[1] = x / base;
    return big_int;
}

void printf_value(uint1024_t x) {
    // Выводим самый последний элемент вектора (или 0, если вектор пустой).
    // Затем выводим все оставшиеся элементы вектора, дополняя их нулями до 9 символов.
    int arr_sz;
    if (arr_sz != 0)
        printf("%d", x.data[arr_sz - 1]);
    else
        printf("%d", 0);
    for (int i = arr_sz - 2; i >= 0; --i)
        printf("%09d", x.data[i]);
}

void scanf_value(uint1024_t *x) {
    // Считываем строку и записываем ее в массив чаров
    char str[1024];
    scanf("%s", str);
    int number_len = strlen(str);
    int inp_size;
    // Считаем количество ячеек в массиве необходииых для занесения числа
    if (number_len % 9 == 0)
        inp_size = number_len / 9;
    else
        inp_size = number_len / 9 + 1;
    uint1024_t inp;
    inp.size = inp_size;
    inp.data = malloc(inp_size * sizeof(int32_t));
    for (int i = number_len, j = 0; i > 0; i -= 9, j++) {
        str[i] = 0;
        if (i >= 9)
            inp.data[j] = str + i - 9;
        else
            inp.data[j] = str;
    }
    *x = inp;
}

int main() {
    return 0;
}