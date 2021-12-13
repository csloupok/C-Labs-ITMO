#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

typedef struct Header {
    unsigned char id[3];
    unsigned char version;
    unsigned char subversion;
    unsigned char flags;
    unsigned char size[4];
} id3v2_header;

typedef struct Frame {
    unsigned char id[4];
    unsigned char size[4];
    unsigned char flags[2];
} id3v2_frame;

void change_frame_size(int new_size, unsigned char *res) {
    for (int i = 3; i > -1; i--) {
        res[i] = new_size % 128;
        new_size /= 128;
    }
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc != 3 && argc != 4) {
        printf("Неправильно введена команда!");
        return 1;
    }

    FILE *fin;
    fin = fopen(argv[1] + 11, "r+b");

    if (fin == NULL) {
        printf("Неправильно выбран файл!");
        return 1;
    }

    int show_flag = 0, get_flag = 0, set_flag = 0;
    char *field_ptr;
    char *value_ptr;
    int value_size = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--show") == 0) {
            show_flag = 1;
            break;
        } else if (strncmp(argv[i], "--get=", 6) == 0) {
            get_flag = 1;
            field_ptr = argv[i] + 6;
            printf("%s\n", field_ptr);
            break;
        } else if (strncmp(argv[i], "--set=", 6) == 0) {
            set_flag = 1;
            field_ptr = argv[i] + 6;
            printf("%s\n", field_ptr);
        } else if (strncmp(argv[i], "--value=", 8) == 0) {
            value_ptr = argv[i] + 8;
            value_size = strlen(value_ptr);
            printf("%s\n", value_ptr);
        }
    }

    char header_id[4];
    int header_size;
    id3v2_header header;
    fread(&header, sizeof(char), sizeof(header), fin);
    //Перевод из байтов в инт
    header_size = header.size[0] * 0x200000 +   //2^21
                  header.size[1] * 0x4000 +     //2^14
                  header.size[2] * 0x80 +       //2^7
                  header.size[3] + 10;
    for (int i = 0; i < 3; i++) {
        header_id[i] = header.id[i];
    }
    header_id[3] = '\0';
    printf("header: %sv2.%x.%x; %d bytes\n", header_id, header.version, header.subversion, header_size);

    int frames_end = 0;
    int byte_sum = 0;
    int last_ptr = 0;
    while (byte_sum < header_size) {
        int frame_size = 0;
        char frame_id[5];
        char frame_encoding;
        id3v2_frame frame;
        fread(&frame, sizeof(char), sizeof(frame), fin);

        for (int i = 0; i < 4; i++) {
            if ((frame.id[i] >= '0' && frame.id[i] <= '9') || (frame.id[i] >= 'A' && frame.id[i] <= 'Z')) {
                frame_id[i] = frame.id[i];
            } else {
                frames_end = 1;
                break;
            }
        }

        if (frames_end) {
            break;
        }

        frame_id[4] = '\0';
        frame_size = frame.size[0] * 0x200000 +   //2^21
                     frame.size[1] * 0x4000 +     //2^14
                     frame.size[2] * 0x80 +       //2^7
                     frame.size[3];

        byte_sum += 10;
        fread(&frame_encoding, sizeof(char), sizeof(frame_encoding), fin);

        if (set_flag && (strcmp(field_ptr, frame_id) == 0)) {
            change_frame_size(value_size + 1, frame.size);
            // сохраняем текущую позицию в файле
            int file_ptr = ftell(fin);
            fseek(fin, frame_size - 1, SEEK_CUR);
            char *buf = calloc(header_size - file_ptr - value_size, sizeof(char));
            fread(buf, sizeof(char), header_size - file_ptr - value_size, fin);

            char x[1];
            x[0] = '\0';
            fseek(fin, file_ptr - 1, SEEK_SET);
            fwrite(x, sizeof(char), 1, fin);

            // переписываем тэг
            fseek(fin, file_ptr, SEEK_SET);
            fwrite(value_ptr, sizeof(char), value_size, fin);

            // пишем буфер
            fwrite(buf, sizeof(char), header_size - file_ptr - value_size, fin);

            // пишем новый размер фрейма
            fseek(fin, file_ptr - 7 , SEEK_SET);
            fwrite(frame.size, sizeof(char), sizeof(frame.size), fin);
            fclose(fin);
            free(buf);
            return 0;
        }
        if (frame_encoding == 0) {
            char temp[frame_size];
            fgets(temp, (int) frame_size, fin);
            if (show_flag) {
                printf("%s %s\n", frame_id, temp);
            } else if (get_flag && (strcmp(field_ptr, frame_id) == 0)) {
                printf("%s %s\n", frame_id, temp);
                return 0;
            }

        } else if (frame_encoding == 1) {
            unsigned char bom[3];
            fread(&bom, sizeof(char), 2, fin);
            bom[2] = '\0';
            if (show_flag) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
                printf("%s ", frame_id);
                wprintf(L"%ls\n", temp);
            } else if (get_flag && (strcmp(field_ptr, frame_id) == 0)) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
                printf("%s ", frame_id);
                wprintf(L"%ls\n", temp);
            } else if (set_flag) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
            }

        } else if (frame_encoding == 2) {
            if (show_flag) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
                printf("%s ", frame_id);
                wprintf(L"%ls\n", temp);
            } else if (get_flag && (strcmp(field_ptr, frame_id) == 0)) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
                printf("%s ", frame_id);
                wprintf(L"%ls\n", temp);
            } else if (set_flag) {
                wchar_t temp[frame_size];
                fgetws(temp, (int) frame_size / 2, fin);
            }

        } else if (frame_encoding == 3) {
            char temp[frame_size];
            fgets(temp, (int) frame_size, fin);
            if (show_flag) {
                printf("%s %s\n", frame_id, temp);
            } else if (get_flag && (strcmp(field_ptr, frame_id) == 0)) {
                printf("%s %s\n", frame_id, temp);
                return 0;
            }
        }
        last_ptr = ftell(fin);
        byte_sum += frame_size;
    }
    fseek(fin, last_ptr, SEEK_SET);
    fclose(fin);
    return 0;
}