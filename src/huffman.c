#include "huffman.h"
#include <string.h>

MinHeapNode* newNode(unsigned char data, unsigned freq) {
    MinHeapNode* temp = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < (int)minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < (int)minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* node) {
    int i = minHeap->size;
    minHeap->size++;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; i--) {
        minHeapify(minHeap, i);
    }
}

MinHeapNode* buildHuffmanTree(int freq[]) {
    MinHeap* minHeap = createMinHeap(256);

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            minHeap->array[minHeap->size] = newNode((unsigned char)i, freq[i]);
            minHeap->size++;
        }
    }
    buildMinHeap(minHeap);

    while (minHeap->size > 1) {
        MinHeapNode* left = extractMin(minHeap);
        MinHeapNode* right = extractMin(minHeap);

        MinHeapNode* top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void buildCodes(MinHeapNode* root, char* code, int top, char codes[256][MAX_TREE_HT]) {
    if (root->left) {
        code[top] = '0';
        buildCodes(root->left, code, top + 1, codes);
    }
    if (root->right) {
        code[top] = '1';
        buildCodes(root->right, code, top + 1, codes);
    }
    if (!root->left && !root->right) {
        code[top] = '\0';
        strcpy(codes[root->data], code);
    }
}

void writeBit(FILE* out, int bit, unsigned char *buffer, int *bitCount) {
    if (bit) *buffer |= (1 << (7 - *bitCount));
    (*bitCount)++;
    if (*bitCount == 8) {
        fputc(*buffer, out);
        *buffer = 0;
        *bitCount = 0;
    }
}

void flushBits(FILE* out, unsigned char *buffer, int *bitCount) {
    if (*bitCount > 0) {
        fputc(*buffer, out);
    }
}
