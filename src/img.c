#include "huffman.h"
#include "img.h"
#include <string.h>

#define BMP_HEADER_SIZE 54

void compressImage(const char* inputFile, const char* outputFile) {
    FILE* in = fopen(inputFile, "rb");
    if (!in) { printf("Cannot open BMP file\n"); return; }

    unsigned char header[BMP_HEADER_SIZE];
    fread(header, 1, BMP_HEADER_SIZE, in); 

    int freq[256] = {0};
    int c;
    while ((c = fgetc(in)) != EOF) freq[c]++;
    rewind(in);
    fseek(in, BMP_HEADER_SIZE, SEEK_SET);  
 
    MinHeapNode* root = buildHuffmanTree(freq);

   
    char codes[256][MAX_TREE_HT] = {{0}};
    char code[MAX_TREE_HT];
    buildCodes(root, code, 0, codes);

    FILE* out = fopen(outputFile, "wb");
    if (!out) { printf("Cannot open output file\n"); return; }

   
    fwrite(header, 1, BMP_HEADER_SIZE, out);

    
    fwrite(freq, sizeof(int), 256, out);

   
    unsigned char buffer = 0;
    int bitCount = 0;
    while ((c = fgetc(in)) != EOF) {
        for (int i = 0; codes[c][i] != '\0'; i++) {
            writeBit(out, codes[c][i] == '1', &buffer, &bitCount);
        }
    }
    flushBits(out, &buffer, &bitCount);

    fclose(in);
    fclose(out);
    printf("Image compressed: %s -> %s\n", inputFile, outputFile);
}

void decompressImage(const char* inputFile, const char* outputFile) {
    FILE* in = fopen(inputFile, "rb");
    if (!in) { printf("Cannot open compressed image\n"); return; }

    unsigned char header[BMP_HEADER_SIZE];
    fread(header, 1, BMP_HEADER_SIZE, in);  

    int freq[256];
    fread(freq, sizeof(int), 256, in);

    MinHeapNode* root = buildHuffmanTree(freq);
    MinHeapNode* current = root;

    FILE* out = fopen(outputFile, "wb");
    if (!out) { printf("Cannot open output BMP\n"); return; }

 
    fwrite(header, 1, BMP_HEADER_SIZE, out);

   
    int c;
    while ((c = fgetc(in)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            int bit = (c >> i) & 1;
            current = (bit == 0) ? current->left : current->right;
            if (!current->left && !current->right) {
                fputc(current->data, out);
                current = root;
            }
        }
    }

    fclose(in);
    fclose(out);
    printf("Image decompressed: %s -> %s\n", inputFile, outputFile);
}
