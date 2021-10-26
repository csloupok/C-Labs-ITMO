//
// Created by Eldar Kasymov on 21.10.2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Реализуем функцию выбора максимального элемента.
#define max(a, b) (((a) > (b)) ? (a) : (b))

// Будем работать в системе по основанию миллиард, т.е. каждый элемент массива содержит сразу 9 цифр.
const int base = 1000000000;

typedef struct {
    size_t size;
    int32_t *data;
} uint1024_t;

uint1024_t from_uint(unsigned int x) {
    int size;
    if (x >= base) size = 2;
    else size = 1;
    uint1024_t big_int;
    big_int.data = malloc(size * sizeof(int32_t));
    big_int.size = size;
    big_int.data[0] = x % base;
    if (size > 1)
        big_int.data[1] = x / base;
    return big_int;
}

void scanf_value(uint1024_t *x) {
    // Считываем строку и записываем ее в массив чаров.
    char str[1024];
    scanf("%s", str);
    int number_len = strlen(str);
    int size;
    // Считаем количество ячеек в массиве необходииых для занесения числа.
    if (number_len % 9 == 0)
        size = number_len / 9;
    else
        size = number_len / 9 + 1;
    uint1024_t inp;
    inp.size = size;
    inp.data = malloc(size * sizeof(int32_t));
    for (int i = number_len, j = 0; i > 0; i -= 9, j++) {
        str[i] = 0;
        if (i >= 9)
            inp.data[j] = atoi(str + i - 9);
        else
            inp.data[j] = atoi(str);
    }
    *x = inp;
}

void printf_value(uint1024_t x) {
    // Выводим самый последний элемент вектора (или 0, если вектор пустой).
    // Затем выводим все оставшиеся элементы вектора, дополняя их нулями до 9 символов.
    int size = x.size;
    if (size != 0)
        printf("%d", x.data[size - 1]);
    else
        printf("%d", 0);
    for (int i = size - 2; i >= 0; --i)
        printf("%09d", x.data[i]);
}

uint1024_t add_op(uint1024_t x, uint1024_t y) {
    uint1024_t sum;
    bool overflow = false;
    int size = max(x.size, y.size);
    sum.data = malloc(size * sizeof(int32_t));
    sum.size = size;
    for (int i = 0; i <= size; i++) {
        sum.data[i] = x.data[i] + y.data[i] + overflow;
        if (sum.data[i] >= base)
            overflow = true;
        if (overflow)
            sum.data[i] -= base;
    }

    if (overflow) {
        sum.data = realloc(sum.data, (size + 1) * sizeof(int32_t));
        sum.data[size] = 0;
        sum.size++;
    }
    return sum;
}



int main() {
    uint1024_t num1, num2;
    printf("Введите первое число\n");
    scanf_value(&num1);
    printf("Введите второе число\n");
    scanf_value(&num2);
    printf_value(num1);
    printf_value(num2);
    return 0;
}