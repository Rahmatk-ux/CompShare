#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include <time.h>

typedef struct {
    long originalSize;     
    long compressedSize;    
    double compressionRatio;
    long bitsSaved;
    double timeTaken;     
} CompressionStats;

// Function prototypes
void startTimer(clock_t* start);
double endTimer(clock_t start);
CompressionStats calculateStats(long originalSize, long compressedSize, double timeTaken);
void printStats(const char* filename, CompressionStats stats);

#endif