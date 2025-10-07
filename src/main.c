#include "txt.h"
#include "img.h"
#include "stats.h"
#include <stdio.h>
#include <sys/stat.h>  
#include <time.h>

long getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return 0;
}

int main() {
    int choice;
    printf("Choose:\n");
    printf("1. Compress Text\n2. Decompress Text\n");
    printf("3. Compress Image\n4. Decompress Image\n");
    scanf("%d", &choice);

    clock_t start;
    double timeTaken;
    long originalSize, compressedSize;
    CompressionStats stats;

    switch (choice) {
        case 1:
            startTimer(&start);
            compressText("data/sample.txt", "build/sample_text.huff");
            timeTaken = endTimer(start);

            originalSize = getFileSize("data/sample.txt");
            compressedSize = getFileSize("build/sample_text.huff");
            stats = calculateStats(originalSize, compressedSize, timeTaken);
            printStats("sample.txt", stats);
            break;

        case 2:
            decompressText("build/sample_text.huff", "build/output.txt");
            break;

        case 3:
            startTimer(&start);
            compressImage("data/sample.bmp", "build/sample_img.huff");
            timeTaken = endTimer(start);

            originalSize = getFileSize("data/sample.bmp");
            compressedSize = getFileSize("build/sample_img.huff");
            stats = calculateStats(originalSize, compressedSize, timeTaken);
            printStats("sample.bmp", stats);
            break;

        case 4:
            decompressImage("build/sample_img.huff", "build/output.bmp");
            break;

        default:
            printf("Invalid choice\n");
    }

    return 0;
}
