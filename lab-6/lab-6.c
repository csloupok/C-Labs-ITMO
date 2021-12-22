#include <stdio.h>
#include <string.h>

// Format:
// |File size|
// |File name|
// |Contents of file|

// Side Functions:
// Little-Endian converting
int BytesToInteger(unsigned char *bytes) {
    return (int) ((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);
}

// shit go back
void IntegerToBytes(unsigned int size, unsigned char *bytes) {
    bytes[0] = (size >> 24) & 0xFF;
    bytes[1] = (size >> 16) & 0xFF;
    bytes[2] = (size >> 8) & 0xFF;
    bytes[3] = size & 0xFF;
}

//Main Functions:
int Create(FILE *archive, char *file) {
    FILE *input = fopen(file, "rb");
    if (input == NULL) {
        printf("Такой файл не существует!\n");
        return 1;
    }
    unsigned char info[4]; // 4 байта под инфу
    fseek(input, 0, SEEK_END); // в конец файла
    unsigned int fileSize = ftell(input); // читаем размерчик
    unsigned int fileNameSize = strlen(file); // читаем размер строки
    IntegerToBytes(fileSize, info); // перевод
    fwrite(info, sizeof(char), 4, archive); // пишем в архив инфу
    IntegerToBytes(fileNameSize, info);
    fwrite(info, sizeof(char), 4, archive);
    fwrite(file, sizeof(char), fileNameSize, archive);
    fseek(input, 0, SEEK_SET); // в начало и пишем уже контент
    for (int i = 0; i < fileSize; i++) {
        char temp = fgetc(input);
        fputc(temp, archive);
    }
    fclose(input);
    return 0;
}

int Extract(char *archive) {
    FILE *input = fopen(archive, "rb");
    if (input == NULL) {
        printf("Такой файл не существует!\n");
        return 1;
    }
    fseek(input, 0, SEEK_END);
    unsigned int archiveSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    while (ftell(input) < archiveSize) { //ну тут все изи тоже самое что и в криейт
        unsigned char info[4];
        fread(info, sizeof(char), 4, input);
        unsigned int fileSize = BytesToInteger(info);
        fread(info, sizeof(char), 4, input);
        unsigned int fileNameSize = BytesToInteger(info);
        char fileName[fileNameSize + 1]; // и место под нулик
        for (int i = 0; i < fileNameSize; i++) {
            char character = fgetc(input);
            fileName[i] = character;
        }
        fileName[fileNameSize] = '\0'; // терминатор нафиг
        FILE *output = fopen(fileName, "wb");
        for (int i = 0; i < fileSize; i++) {
            char temp = fgetc(input);
            fputc(temp, output);
        }
        fclose(output);
    }
    fclose(input);
    return 0;
}

int List(char *archive) {
    FILE *input = fopen(archive, "rb");
    if (input == NULL) {
        printf("Такой файл не существует!\n");
        return 1;
    }
    fseek(input, 0, SEEK_END);
    unsigned int archiveSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    if (archiveSize == 0)
        printf("Архив пустой!\n");
    while (ftell(input) < archiveSize) { //ну тут все изи тоже самое что и в криейт и в экстракт
        unsigned char info[4];
        fread(info, sizeof(char), 4, input); // размер файлика чтоб скипать конент
        unsigned int fileSize = BytesToInteger(info);
        fread(info, sizeof(char), 4, input); // названиееее
        unsigned int fileNameSize = BytesToInteger(info); // перевод обратно а то не поймет
        for (int i = 0; i < fileNameSize; i++) {
            char character = fgetc(input);
            printf("%c", character);
        }
        printf("\n");
        fseek(input, fileSize, SEEK_CUR);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char *archive;
    if (strcmp(argv[1], "--file") == 0)
        archive = argv[2];
    else {
        printf("Неправильный ввод!\n");
        return 1;
    }
    if (strcmp(argv[3], "--create") == 0) {
        FILE *output = fopen(archive, "ab");
        if (output == NULL) {
            printf("Такой файл не существует!\n");
            return 1;
        }
        for (int i = 4; i < argc; i++) {
            char *filesToArchive = argv[i];
            Create(output, filesToArchive);
        }
        fclose(output);
    } else if (strcmp(argv[3], "--extract") == 0) {
        Extract(archive);
    } else if (strcmp(argv[3], "--list") == 0) {
        List(archive);
    }
    return 0;
}