//
// Created by Eldar Kasymov on 21.10.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Реализуем функцию выбора максимального элемента.
#define max(a, b) (((a) > (b)) ? (a) : (b))

// Один из вариантов хранения длинных чисел — массив целых чисел int.
// Каждый элемент — это одна цифра числа в b-ичной системе счисления.
// Для повышения эффективности каждый элемент массива будет содержать несколько цифр.
// Работаем в системе счисления по основанию миллиард, тогда каждый элемент содержит 9 цифр.
const int base = 1000 * 1000 * 1000;

typedef struct {
    size_t size;
    int32_t *data;
} uint1024_t;

// Цифры будут храниться в массиве в следующем порядке: сначала идут наименее значимые цифры.
// Кроме того после выполнения любой из операций лидирующие нули отсутствуют.
uint1024_t from_uint(unsigned int x) {
    int size;
    if (x >= base) size = 2;
    else size = 1;
    uint1024_t big_int;
    big_int.data = calloc(size, sizeof(int32_t));
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
    inp.data = calloc(size, sizeof(int32_t));
    for (int i = number_len, j = 0; i > 0; i -= 9, j++) {
        // Для корректной работы atoi строка должна
        // быть нуль-терминированной, то есть оканчиваться символом «\0».
        str[i] = "\0";
        if (i >= 9)
            inp.data[j] = atoi(str + i - 9);
        else
            inp.data[j] = atoi(str);
    }
    *x = inp;
}

// Выводим самый последний элемент вектора (или 0, если вектор пустой).
// Затем выводим все оставшиеся элементы вектора с конца, дополняя их нулями до 9 символов.
void printf_value(uint1024_t x) {
    int size = x.size;
    if (size != 0)
        printf("%d", x.data[size - 1]);
    else
        printf("%d", 0);
    for (int i = size - 2; i >= 0; --i)
        printf("%09d", x.data[i]);
}

// Сложение. Прибавляет к числу x число y и сохраняет результат в sum.
// Проход от первого разряда до разряда равному длине суммы (максимального из двух чисел).
// Если сумма будет больше, чем основание системы счисления,то произойдет перенос в старший разряд.
uint1024_t add_op(uint1024_t x, uint1024_t y) {
    uint1024_t sum;
    // carry - перенос в следующий разряд.
    int carry = 0;
    int size = max(x.size, y.size);
    sum.data = calloc(size, sizeof(int32_t));
    sum.size = size;
    for (int i = 0; i < size || carry; i++) {
        if (i == size) {
            sum.data = realloc(sum.data, (size + 1) * sizeof(int32_t));
            sum.data[size] = 0;
            sum.size++;
        }
        sum.data[i] = x.data[i] + y.data[i] + carry;
        if (sum.data[i] >= base) {
            carry = 1;
            sum.data[i] -= base;
        } else carry = 0;
    }
    return sum;
}

// Вычитание. Отнимает от числа a число b(a⩾b) и сохраняет результат в diff.
// Проход от первого разряда до разряда равному длине разности (максимального из двух чисел).
// Если разница в текущем разряде меньше нуля, то занимаем из старшего разряда.
uint1024_t subtr_op(uint1024_t x, uint1024_t y) {
    uint1024_t diff;
    int carry = 0;
    int size = max(x.size, y.size);
    diff.data = calloc(size, sizeof(int32_t));
    diff.size = size;
    for (int i = 0; i < size || carry; i++) {
        if (i == size)
            break;
        diff.data[i] = x.data[i] - y.data[i] - carry;
        if (diff.data[i] < 0) {
            carry = 1;
            diff.data[i] += base;
        } else carry = 0;
    }
    // Удаляем лидирующие нули.
    while (diff.data[diff.size - 1] == 0 && diff.size > 1)
        diff.size--;
    diff.data = realloc(diff.data, diff.size * sizeof(int32_t));
    return diff;
}

// Умножение. Работает, как умножение столбиком.
// Для каждой i-ой цифры второго множителя:
// Находим произведение первого множителя на i-ую цифру.
// Сдвигаем результат на слеующий разряд.
uint1024_t mult_op(uint1024_t x, uint1024_t y) {
    uint1024_t comp;
    int size = x.size + y.size;
    comp.data = calloc(size, sizeof(int32_t));
    comp.size = size;
    // Цикл по количеству цифр в первом числе.
    for (int i = 0; i < x.size; i++)
        // Цикл по количеству цифр во втором числе.
        for (int j = 0, carry = 0; j < y.size || carry; j++) {
            long long t = comp.data[i + j] + x.data[i] * 1ll * y.data[j] + carry;
            // Значение цифры в рязряде i + j.
            comp.data[i + j] = (int) (t % base);
            // Перенос в следующий разряд.
            carry = (int) (t / base);
        }
    while (comp.data[size - 1] == 0 && size > 1)
        size--;
    comp.data = realloc(comp.data, size * sizeof(int32_t));
    comp.size = size;
    return comp;
}

int main() {
    uint1024_t num1, num2;
    char input[320];
    // Инициализация массива нулями.
    memset(input, 0, 320);
    printf("[Введите первое число]:\n");
    scanf_value(&num1);
    printf("[Введите второе число]:\n");
    scanf_value(&num2);
    fseek(stdin, 0, SEEK_SET);
    while (1) {
        printf("[Выберите операцию: + - *]\n-----------ИЛИ-----------\n[Введите stop для завершения]\n");
        scanf("%c", input);
        if (!strcmp(input, "+")) num1 = add_op(num1, num2);
        else if (!strcmp(input, "-")) num1 = subtr_op(num1, num2);
        else if (!strcmp(input, "*")) num1 = mult_op(num1, num2);
        else break;
        printf_value(num1);
        fseek(stdin, 0, SEEK_SET);
        printf("\nВведите новое число\n");
        scanf_value(&num2);
        fseek(stdin, 0, SEEK_SET);
    }
    return 0;
}