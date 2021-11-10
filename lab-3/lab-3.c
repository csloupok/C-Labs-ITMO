#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Пример строки лога:
//199.72.81.55 - - [01/Jul/1995:00:00:01 -0400] "GET /history/apollo/ HTTP/1.0" 200 6245

const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

long long dateConvert(char *str) {
    struct tm time = {
            .tm_mday = atoi(str),
            .tm_year = atoi(str + 7) - 1900,
            .tm_hour = atoi(str + 12),
            .tm_min = atoi(str + 15),
            .tm_sec = atoi(str + 18),
            .tm_isdst = -1
    };
    str[6] = '\0';
    int n = 0;
    while (strcmp(str + 3, months[n]))
        n++;
    time.tm_mon = n;
    return mktime(&time);
}

int main() {
    long long window;
    window = 10;
    FILE *fin = fopen("/Users/eldarkasymov/CLionProjects/Progs/lab-3/access_log_Jul95", "r");
    if (!fin) {
        printf("не читается файл\n");
        return 1;
    }
    long long res_allocated = 50;
    long long time_allocated = 1000;
    //Массив на res_allocated строк, если больше - делаем realloc
    char **res = (char **) malloc(res_allocated * sizeof(char *));
    //Массив для хранения времени запроса в секундах для подсчета максимального количества запросов в промежутке
    long long *time = malloc(time_allocated * sizeof(long long));
    //Максимальная длина HTTP запроса = 2048 + \n = 2049
    char line[2049];
    long long error_count = 0;
    long long line_count = 0;
    while (fgets(line, sizeof(line), fin) != NULL) {
        //Выделяем только часть строки с датой и временем
        char str[21];
        int i = 0, j = strlen(line) - 1;
        while (line[i++] != '[');
        memcpy(str, line + i, 20);
        str[20] = '\0';
        if (line_count == time_allocated) {
            time_allocated *= 2;
            time = realloc(time, time_allocated * sizeof(long long));
        }
        time[line_count++] = dateConvert(str);

        //Ищем нужную ошибку 5xx
        while (line[j--] != ' ');
        if (line[j - 2] == '5') {
            if (error_count == res_allocated) {
                res_allocated *= 2;
                res = realloc(res, res_allocated * sizeof(char *));
            }
            //Ищем запрос
            while (line[i++] != '"');
            while (line[j--] != '"');
            line[j + 1] = '\0';
            res[error_count] = malloc(strlen(line + i) + 1);
            strcpy(res[error_count++], line + i);
        }
    }
    long long left_border = 0, current_window = 0;
    long long time_stamp1 = time[0], time_stamp2 = time[0];
    long long request_count = 1, request_max = 0;
    long long line_start = 0, line_end = 0;
    for (long long i = 1; i < line_count; i++)
    {
        current_window += time[i] - time[i - 1];
        request_count++; 
        while (current_window > window && left_border + 1 < line_count) {
            current_window -= time[left_border + 1] - time[left_border];
            left_border++;
            request_count--;
        }
        if (request_count > request_max) {
            time_stamp1 = time[left_border];
            time_stamp2 = time[i];
            request_max = request_count;
            line_start = left_border;
            line_end = i;
        }
    }
    fclose(fin);
    free(time);
    printf("\nВременное окно в [%lld] секунд:\n[%lld до %lld]\nмаксимальное число запросов в этом окне: [%lld]\nв строках: [%lld - %lld]\n",
           window, time_stamp1, time_stamp2, request_max, line_start + 1, line_end + 1);
    printf("\nКоличество ошибок: [%lld]\n\n", error_count);

    if (error_count) {
        for (long long i = 0; i < error_count; i++) {
            printf("%s\n", res[i]);
        }
    }
    return 0;
}