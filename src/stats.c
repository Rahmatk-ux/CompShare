#include "stats.h"

void startTimer(clock_t* start) {
    *start = clock();
}

double endTimer(clock_t start) {
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

CompressionStats calculateStats(long originalSize, long compressedSize, double timeTaken) {
    CompressionStats stats;
    stats.originalSize = originalSize;
    stats.compressedSize = compressedSize;
    stats.compressionRatio = (originalSize > 0) ? (double)compressedSize / originalSize : 0.0;
    stats.bitsSaved = (originalSize - compressedSize) * 8;
    stats.timeTaken = timeTaken;
    return stats;
}

void printStats(const char* filename, CompressionStats stats) {
    printf("====================================\n");
    printf("File: %s\n", filename);
    printf("Original size   : %ld bytes\n", stats.originalSize);
    printf("Compressed size : %ld bytes\n", stats.compressedSize);
    printf("Compression ratio: %.2f%%\n", stats.compressionRatio * 100);
    printf("Bits saved      : %ld bits\n", stats.bitsSaved);
    printf("Time taken      : %.2f seconds\n", stats.timeTaken);
    printf("====================================\n");
}
