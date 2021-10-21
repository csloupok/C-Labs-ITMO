#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[2], "r");
    if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--bytes") == 0)) {
        int bytes_count = 0;
        while (fgetc(fp) != EOF) bytes_count++;
        printf("%d\n", bytes_count);
    } else if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--lines") == 0)) {
        int character = 0, lines_count = 1;
        while (character != EOF) {
            character = fgetc(fp);
            if (character == '\n') ++lines_count;
        }
        printf("%d\n", lines_count);
    } else if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--words") == 0)) {
        int words_count = 0, character = 0, word = 0;
        while (character != EOF) {
            character = fgetc(fp);
            if (character == ' ' || character == '\n' || character == '\t' || character == EOF) {
                if (word == 1) {
                    word = 0;
                    ++words_count;
                }
            } else word = 1;
        }
        printf("%d\n", words_count);
    } else if (strcmp(argv[1], "-n") == 0) {
        int lines_count = 1, words_count = 0, character = 0, word = 0;
        while (character != EOF) {
            character = fgetc(fp);
            if (character == ' ' || character == '\n' || character == '\t' || character == EOF) {
                if (character == '\n') ++lines_count;
                if (word == 1) {
                    word = 0;
                    ++words_count;
                }
            } else word = 1;
        }
        printf("%d\n", (words_count / lines_count));
        return 0;
    }
    return 0;
}
