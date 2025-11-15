#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_NODES 511
#define MAX_CODE_LENGTH 256

typedef struct HuffmanNode {
    unsigned char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct PriorityQueue {
    int size;
    int capacity;
    HuffmanNode **nodes;
} PriorityQueue;

typedef struct HuffmanCode {
    unsigned char code[MAX_CODE_LENGTH];
    int length;
} HuffmanCode;

// Function prototypes
HuffmanNode* createNode(unsigned char data, unsigned freq);
PriorityQueue* createQueue(int capacity);
void swapNodes(HuffmanNode** a, HuffmanNode** b);
void minHeapify(PriorityQueue* queue, int idx);
HuffmanNode* extractMin(PriorityQueue* queue);
void insertQueue(PriorityQueue* queue, HuffmanNode* node);
void buildHuffmanTree(PriorityQueue* queue);
void generateCodes(HuffmanNode* root, HuffmanCode codes[], unsigned char code[], int top);
void compressFile(const char* inputFile, const char* outputFile);
void decompressFile(const char* inputFile, const char* outputFile);
void freeHuffmanTree(HuffmanNode* root);

#endif
