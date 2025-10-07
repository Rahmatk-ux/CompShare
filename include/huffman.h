#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_TREE_HT 256


typedef struct MinHeapNode {
    unsigned char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;


typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    MinHeapNode **array;
} MinHeap;


MinHeapNode* newNode(unsigned char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* node);
MinHeapNode* extractMin(MinHeap* minHeap);
void buildMinHeap(MinHeap* minHeap);
MinHeapNode* buildHuffmanTree(int freq[]);


void buildCodes(MinHeapNode* root, char* code, int top, char codes[256][MAX_TREE_HT]);

void writeBit(FILE* out, int bit, unsigned char *buffer, int *bitCount);
void flushBits(FILE* out, unsigned char *buffer, int *bitCount);

#endif
