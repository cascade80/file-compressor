#include "huffman.h"

HuffmanNode* createNode(unsigned char data, unsigned freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

PriorityQueue* createQueue(int capacity) {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    queue->size = 0;
    queue->capacity = capacity;
    queue->nodes = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    return queue;
}

void swapNodes(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(PriorityQueue* queue, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < queue->size && queue->nodes[left]->freq < queue->nodes[smallest]->freq)
        smallest = left;

    if (right < queue->size && queue->nodes[right]->freq < queue->nodes[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&queue->nodes[smallest], &queue->nodes[idx]);
        minHeapify(queue, smallest);
    }
}

HuffmanNode* extractMin(PriorityQueue* queue) {
    if (queue->size == 0)
        return NULL;

    HuffmanNode* minNode = queue->nodes[0];
    queue->nodes[0] = queue->nodes[queue->size - 1];
    queue->size--;
    minHeapify(queue, 0);
    return minNode;
}

void insertQueue(PriorityQueue* queue, HuffmanNode* node) {
    if (queue->size == queue->capacity)
        return;

    int i = queue->size;
    queue->size++;
    queue->nodes[i] = node;

    while (i != 0 && queue->nodes[(i - 1) / 2]->freq > queue->nodes[i]->freq) {
        swapNodes(&queue->nodes[i], &queue->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void buildHuffmanTree(PriorityQueue* queue) {
    while (queue->size > 1) {
        HuffmanNode* left = extractMin(queue);
        HuffmanNode* right = extractMin(queue);

        HuffmanNode* parent = createNode(0, left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        insertQueue(queue, parent);
    }
}

void generateCodes(HuffmanNode* root, HuffmanCode codes[], unsigned char code[], int top) {
    if (root->left) {
        code[top] = 0;
        generateCodes(root->left, codes, code, top + 1);
    }

    if (root->right) {
        code[top] = 1;
        generateCodes(root->right, codes, code, top + 1);
    }

    if (!root->left && !root->right) {
        codes[root->data].length = top;
        memcpy(codes[root->data].code, code, top);
    }
}

void compressFile(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "rb");
    FILE* output = fopen(outputFile, "wb");

    if (!input || !output) {
        printf("Error opening files!\n");
        return;
    }

    // Count frequency of each byte
    unsigned freq[256] = {0};
    int ch;
    while ((ch = fgetc(input)) != EOF) {
        freq[ch]++;
    }
    rewind(input);

    // Create priority queue and insert nodes
    PriorityQueue* queue = createQueue(256);
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            insertQueue(queue, createNode(i, freq[i]));
        }
    }

    // Build Huffman tree
    buildHuffmanTree(queue);
    HuffmanNode* root = extractMin(queue);

    // Generate Huffman codes
    HuffmanCode codes[256] = {0};
    unsigned char code[MAX_CODE_LENGTH];
    generateCodes(root, codes, code, 0);

    // Write header (frequency table)
    fwrite(freq, sizeof(unsigned), 256, output);

    // Compress data
    unsigned char buffer = 0;
    int bitCount = 0;

    while ((ch = fgetc(input)) != EOF) {
        HuffmanCode hcode = codes[ch];
        for (int i = 0; i < hcode.length; i++) {
            buffer = (buffer << 1) | hcode.code[i];
            bitCount++;

            if (bitCount == 8) {
                fputc(buffer, output);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    // Write remaining bits
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fputc(buffer, output);
        fputc(bitCount, output); // Store number of valid bits in last byte
    } else {
        fputc(8, output); // All bits were valid
    }

    // Cleanup
    freeHuffmanTree(root);
    free(queue->nodes);
    free(queue);
    fclose(input);
    fclose(output);

    printf("File compressed successfully!\n");
}

void decompressFile(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "rb");
    FILE* output = fopen(outputFile, "wb");

    if (!input || !output) {
        printf("Error opening files!\n");
        return;
    }

    // Read frequency table
    unsigned freq[256];
    fread(freq, sizeof(unsigned), 256, input);

    // Rebuild Huffman tree
    PriorityQueue* queue = createQueue(256);
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            insertQueue(queue, createNode(i, freq[i]));
        }
    }

    buildHuffmanTree(queue);
    HuffmanNode* root = extractMin(queue);

    // Read last byte info
    fseek(input, -1, SEEK_END);
    int lastByteBits = fgetc(input);
    fseek(input, 256 * sizeof(unsigned), SEEK_SET);

    // Decompress data
    HuffmanNode* current = root;
    unsigned char byte;
    long fileSize;
    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    fseek(input, 256 * sizeof(unsigned), SEEK_SET);

    long dataEnd = fileSize - 2; // Exclude last byte and bit count

    while (ftell(input) < dataEnd || (ftell(input) == dataEnd && lastByteBits > 0)) {
        byte = fgetc(input);
        int bitsToProcess = (ftell(input) == dataEnd + 1) ? lastByteBits : 8;

        for (int i = 7; i >= 8 - bitsToProcess; i--) {
            int bit = (byte >> i) & 1;

            if (bit == 0)
                current = current->left;
            else
                current = current->right;

            if (!current->left && !current->right) {
                fputc(current->data, output);
                current = root;
            }
        }
    }

    // Cleanup
    freeHuffmanTree(root);
    free(queue->nodes);
    free(queue);
    fclose(input);
    fclose(output);

    printf("File decompressed successfully!\n");
}

void freeHuffmanTree(HuffmanNode* root) {
    if (root == NULL) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}
