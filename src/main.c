#include "txt.h"
#include "img.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

long getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage:\n");
        printf("  %s compress_txt input.txt output.huff\n", argv[0]);
        printf("  %s decompress_txt input.huff output.txt\n", argv[0]);
        printf("  %s compress_img input.bmp output.huff\n", argv[0]);
        printf("  %s decompress_img input.huff output.bmp\n", argv[0]);
        return 1;
    }

    char* operation = argv[1];
    char* inputFile = argv[2];
    char* outputFile = argv[3];

    clock_t start;
    double timeTaken;
    long originalSize, compressedSize;
    CompressionStats stats;

    if (strcmp(operation, "compress_txt") == 0) {
        startTimer(&start);
        compressText(inputFile, outputFile);
        timeTaken = endTimer(start);

        originalSize = getFileSize(inputFile);
        compressedSize = getFileSize(outputFile);
        stats = calculateStats(originalSize, compressedSize, timeTaken);
        printStats(inputFile, stats);
    }
    else if (strcmp(operation, "decompress_txt") == 0) {
        decompressText(inputFile, outputFile);
    }
    else if (strcmp(operation, "compress_img") == 0) {
        startTimer(&start);
        compressImage(inputFile, outputFile);
        timeTaken = endTimer(start);

        originalSize = getFileSize(inputFile);
        compressedSize = getFileSize(outputFile);
        stats = calculateStats(originalSize, compressedSize, timeTaken);
        printStats(inputFile, stats);
    }
    else if (strcmp(operation, "decompress_img") == 0) {
        decompressImage(inputFile, outputFile);
    }
    else {
        printf("Invalid operation: %s\n", operation);
        return 1;
    }

    return 0;
}
