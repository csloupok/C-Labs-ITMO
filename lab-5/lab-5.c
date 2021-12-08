#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Initial states of cells
#define LIVE 1 // Filled with black pixel
#define DEAD 0 // Filled with white pixel

typedef struct BMP_Header {
    char info1[18];
    char width[4];
    char height[4];
    char info2[28];
} Header;

typedef struct BMP_Dimensions {
    int height, width;
} Bitmap;

// BMP ColorTable stores 4 bytes for colors (RED, GREEN, BLUE, unused)
typedef struct BMP_Colors {
    int R, G, B;
} Color;

// Big-Endian converting function
int BytesToInteger(unsigned char *bytes) {
    return (int) ((bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0]);
}

int ColorToState(Color pixel) {
    if (pixel.B == 0 && pixel.R == 0 && pixel.G == 0) // Black pixel (RGB 0 0 0)
        return LIVE;
    return DEAD; // Otherwise, white pixel
}

int GetNeighborsNumber(int x, int y, int **field, int w, int h) {
    int neighborsCount = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y)
                continue;

            // Checking all the neighbors cells (cell is either 0 or 1)
            neighborsCount += field[(j + h) % h][(i + w) % w];
        }
    }
    return neighborsCount;
}

void LifeCycle(int **currentField, Bitmap bitmap) {
    // Creating the next generation currentField
    int **nextField = (int **) malloc(sizeof(int *) * bitmap.height);
    for (int i = 0; i < bitmap.height; i++)
        nextField[i] = (int *) malloc(bitmap.width * sizeof(int));

    // Just following the Game Of Life rules
    int neighborsCount;
    for (int y = 0; y < bitmap.height; y++)
        for (int x = 0; x < bitmap.width; x++) {
            neighborsCount = GetNeighborsNumber(x, y, currentField, bitmap.width, bitmap.height);
            if (currentField[y][x] == LIVE) {
                if (neighborsCount < 2 || neighborsCount > 3)
                    nextField[y][x] = DEAD;
                else
                    nextField[y][x] = LIVE;
            } else {
                if (neighborsCount == 3)
                    nextField[y][x] = LIVE;
                else
                    nextField[y][x] = DEAD;
            }
        }
    // Updating current field
    for (int y = 0; y < bitmap.height; y++)
        for (int x = 0; x < bitmap.width; x++)
            currentField[y][x] = nextField[y][x];

    // Clearing memory for the next generation field
    for (int i = 0; i < bitmap.height; i++)
        free(nextField[i]);
    free(nextField);
}

int **ReadField(FILE *fin, Bitmap bitmap) {
    // Reading pixels and their color
    Color **pixels = (Color **) malloc(bitmap.height * sizeof(Color *));
    for (int i = 0; i < bitmap.height; i++)
        pixels[i] = (Color *) malloc(bitmap.width * sizeof(Color));
    for (int i = bitmap.height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap.width; j++) {
            pixels[i][j].B = getc(fin);
            pixels[i][j].G = getc(fin);
            pixels[i][j].R = getc(fin);
        }

    // Creating field
    int **field = (int **) malloc(bitmap.height * sizeof(int *));
    for (int i = 0; i < bitmap.height; i++) {
        field[i] = (int *) malloc(bitmap.width * sizeof(int));
    }

    // Filling field with cells, according to their state (color)
    for (int i = 0; i < bitmap.height; i++)
        for (int j = 0; j < bitmap.width; j++) {
            field[i][j] = ColorToState(pixels[i][j]);
        }

    // Freeing memory
    for (int i = 0; i < bitmap.height; i++)
        free(pixels[i]);
    free(pixels);
    fclose(fin);
    return field;
}

int main(int argc, char *argv[]) {
    // Opening a file
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL) {
        printf("Неправильный ввод\n");
        return 1;
    }
    if (strcmp(argv[1], "--input") || strcmp(argv[3], "--output")) {
        printf("Неправильный ввод\n");
        return 1;
    }
    FILE *input = fopen(argv[2], "rb");
    if (input == NULL) {
        printf("Невозможно октрыть файл");
        return 1;
    }

    // Reading max iterations and dump frequency
    int maxIter;
    if (argv[5] != NULL && !(strcmp(argv[5], "--max_iter")))
        maxIter = atoi(argv[6]);
    else
        maxIter = 10;

    int dumpFreq;
    if (argv[7] != NULL && !(strcmp(argv[7], "--dump_freq")))
        dumpFreq = atoi(argv[8]);
    else
        dumpFreq = 1;

    // Reading BMP and building field
    Header header;
    fread(&header, 1, 54, input);
    Bitmap bitmap;
    bitmap.height = BytesToInteger((unsigned char *) header.height);
    bitmap.width = BytesToInteger((unsigned char *) header.width);
    int **currentField = ReadField(input, bitmap);

    //Main program cycle with file output and game algorithm
    for (int i = 1; i <= maxIter; i++) {
        // Creating directory and file naming for output
        char filePath[256];
        char fileName[32];
        sprintf(fileName, "%d", i);
        strcpy(filePath, argv[4]);
        strcat(filePath, "/");
        strcat(filePath, fileName);
        strcat(filePath, ".bmp");
        FILE *output = fopen(filePath, "wb");
        if (output == NULL) {
            printf("Невозможно создать файл\n");
            return 1;
        }

        // Algorithm of game
        LifeCycle(currentField, bitmap);

        // Checking frequency
        if (i % dumpFreq != 0)
            continue;

        // Writing field to file
        fwrite(&header, 1, 54, output);
        int n = 0;
        unsigned char *newPixels = (unsigned char *) malloc(bitmap.width * bitmap.height * 3);
        for (int m = bitmap.height - 1; m >= 0; m--)
            for (int j = 0; j < bitmap.width; j++)
                for (int k = 0; k < 3; k++) {
                    if (currentField[m][j] == DEAD)
                        newPixels[n] = 255;
                    else
                        newPixels[n] = 0;
                    n++;
                }
        fwrite(newPixels, 1, bitmap.width * bitmap.height * 3, output);
        free(newPixels);
        fclose(output);
    }
    // Freeing memory
    for (int i = 0; i < bitmap.height; i++)
        free(currentField[i]);
    free(currentField);
    return 0;
}