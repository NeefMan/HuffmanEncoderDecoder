#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INTERNAL_NODE_CODE '\0'
#define EXTENDED_ASCII_SET_SIZE 256
#define HUFFMAN_LEFT_TRAVERSAL_CODE 0
#define HUFFMAN_RIGHT_TRAVERSAL_CODE 1
#define NOW_ON_LEAF_NODE 0
#define NOW_ON_INTERNAL_NODE 1

typedef struct SymbolCodeNode{
    unsigned int code;
    int bitCount;
}SymbolCodeNode;
typedef struct HuffmanNode{
    SymbolCodeNode *symbolCodeNode;
    unsigned char symbol;
    int frequency;
    struct HuffmanNode *left, *right;
    struct HuffmanNode *next;
}HuffmanNode;

void insertHuffmanNode(HuffmanNode *newNode, HuffmanNode *dummy);
void printHuffmanNodeLinkedList(HuffmanNode *dummy);
void printTree(HuffmanNode *root, int depth);
HuffmanNode *createLinkedList(int symbolFrequencies[]);
HuffmanNode *createHuffmanNode(unsigned char symbol, int frequency);
HuffmanNode *createHuffmanTree(HuffmanNode *dummy);
HuffmanNode *popHuffmanNode(HuffmanNode *dummy);
void writeBit(FILE *file, int bit);
void flushBuffer(FILE *file);
int insertSymbolCodes(SymbolCodeNode *symbolCodes[], HuffmanNode *root);
void printSymbolCodes(SymbolCodeNode *symbolCodes[]);
void printSymbolFrequencies(int symbolFrequencies[]);
void ensureProperUsage(int argc, char *argv[]);
void encodeFile(char *readFileName, HuffmanNode *root);
void readFrequencies(int symbolFrequencies[], char *fileName);
void decodeFile(FILE *readFile, HuffmanNode *root);
FILE *safefopen(char *fileName, char *method);
void printBinary(unsigned int n, int length);

int linkedListLength = 0;
int bitCount = 0;
unsigned char buffer = 0;
int symbolFrequencies[EXTENDED_ASCII_SET_SIZE] = {0};
SymbolCodeNode *symbolCodes[EXTENDED_ASCII_SET_SIZE] = {NULL};
int totalBitCount = 0;

// expected argv: [exe-name] [encode/decode] [file-name]
int main(int argc, char *argv[]) {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    ensureProperUsage(argc, argv);
    char *fileName = argv[2];
    char *usage = argv[1];
    FILE *readFile;

    if (strcmp(usage, "encode") == 0){
        readFrequencies(symbolFrequencies, fileName);
    }else{
        readFile = safefopen(fileName, "rb");
        fread(symbolFrequencies, sizeof(int), 256, readFile);
    }

    HuffmanNode *dummy = createLinkedList(symbolFrequencies);
    HuffmanNode *root = createHuffmanTree(dummy);
    insertSymbolCodes(symbolCodes, root);

    if (strcmp(usage, "encode") == 0){
        encodeFile(fileName, root);
    }else{
        decodeFile(readFile, root);
        fclose(readFile);
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s took %f seconds to execute.\n", argv[0], cpu_time_used);
}

FILE *safefopen(char *fileName, char *method)
{
    FILE *file = fopen(fileName, method);
    if (file == NULL){
        fprintf(stderr, "Error opening file [%s]\n", fileName);
        exit(1);
    }
    return file;
}

void decodeFile(FILE *readFile, HuffmanNode *root)
{
    FILE *outputFile = safefopen("decoded_output.txt", "w");
    HuffmanNode *node = root;
    int bitShiftN, bit, bitsRemaining;
    unsigned c;
    bitsRemaining = totalBitCount;

    while ((c = fgetc(readFile)) != EOF){
        if (bitsRemaining < 8){
            c = c >> 8 - bitsRemaining;
            bitShiftN = bitsRemaining-1;
        }else{
            bitShiftN = 7;
        }
        while (bitShiftN >= 0){
            bit = (c >> bitShiftN) & 1;
            if (bit == 0){
                node = node->left;
            }else{
                node = node->right;
            }
            if (!node->left){
                fputc(node->symbol, outputFile);
                node = root;
            }
            bitShiftN--;
            bitsRemaining--;
        }
    }
}

void readFrequencies(int symbolFrequencies[], char *fileName)
{
    FILE *file = safefopen(fileName, "r");
    int c;
    while ((c = fgetc(file)) != EOF){
        symbolFrequencies[c]++;
    }
}

void encodeFile(char *readFileName, HuffmanNode *root)
{
    char writeFileName[] = {"output.txt"};
    FILE *writeFile = safefopen(writeFileName, "wb");
    // Write entire symbolFrequencies array to file (4 bytes for each frequency)
    fwrite(symbolFrequencies, sizeof(int), EXTENDED_ASCII_SET_SIZE, writeFile); 

    FILE *readFile = safefopen(readFileName, "r");
    
    int c, i;
    int symbolCode;
    while ((c = fgetc(readFile)) != EOF){
        symbolCode = symbolCodes[c]->code;
        for (i = symbolCodes[c]->bitCount-1; i >= 0; i--){
            writeBit(writeFile, (symbolCode >> i) & 1);
        }
    }
    flushBuffer(writeFile);

    fclose(readFile);
    fclose(writeFile);
}

void ensureProperUsage(int argc, char *argv[])
{
    if (argc != 3){
        fprintf(stderr, "Proper usage: %s [encode/decode] [file-name]\n", argv[0]);
        exit(1);
    }
    if (strcmp(argv[1], "encode") != 0 && strcmp(argv[1], "decode") != 0){
        fprintf(stderr, "Unknown command: %s\nProper usage: %s [encode/decode] [file-name]\n", argv[1], argv[0]);
        exit(1);
    }
    FILE *file = safefopen(argv[2], "r");
    fclose(file);
}

void printBinary(unsigned int n, int length) {
    for (int i = length-1; i >= 0; i--) {
        putchar((n & (1u << i)) ? '1' : '0');
    }
}

void printSymbolCodes(SymbolCodeNode *symbolCodes[])
{
    printf("total bit count: %d\n", totalBitCount);
    for (int i = 0; i < EXTENDED_ASCII_SET_SIZE; i++){
        if (symbolCodes[i]){
            printf("bitcount: %d\t%c: ", symbolCodes[i]->bitCount, i);
            printBinary(symbolCodes[i]->code, symbolCodes[i]->bitCount);
            printf("\tfreq: %d\n", symbolFrequencies[i]);
        }
    }
}

void printSymbolFrequencies(int symbolFrequencies[]){
    for (int i = 0; i < EXTENDED_ASCII_SET_SIZE; i++){
        if (symbolFrequencies[i] > 0){
            printf("%c: %d\n", i, symbolFrequencies[i]);
        }
    }
}

int insertSymbolCodes(SymbolCodeNode *symbolCodes[], HuffmanNode *root)
{
    int result;
    if (!root){
        return NOW_ON_LEAF_NODE;
    }
    if (root->left){
        root->left->symbolCodeNode->code = root->symbolCodeNode->code << 1;
        root->left->symbolCodeNode->bitCount = root->symbolCodeNode->bitCount+1;
    }
    if (root->right){
        root->right->symbolCodeNode->code = root->symbolCodeNode->code << 1;
        root->right->symbolCodeNode->code |= HUFFMAN_RIGHT_TRAVERSAL_CODE;
        root->right->symbolCodeNode->bitCount = root->symbolCodeNode->bitCount+1;
    }
    result = insertSymbolCodes(symbolCodes, root->left);
    if (result == NOW_ON_LEAF_NODE){
        symbolCodes[root->symbol] = root->symbolCodeNode;
        totalBitCount += symbolFrequencies[root->symbol] * root->symbolCodeNode->bitCount;
        return NOW_ON_INTERNAL_NODE;
    }
    result = insertSymbolCodes(symbolCodes, root->right);
}

void writeBit(FILE *file, int bit)
{   
    buffer = (buffer << 1) | (bit & 1);
    bitCount++;
    if (bitCount == 8){
        fputc(buffer, file);
        bitCount = 0;
        buffer = 0;
    }
}

void flushBuffer(FILE *file)
{
    if (bitCount > 0){
        buffer <<= 8 - bitCount;
        fputc(buffer, file);
        bitCount = 0;
        buffer = 0;
    }
}

#define PRINT_TREE_INDENT_STEP 4

void printTree(HuffmanNode *root, int depth) {
    if (root == NULL) {
        return;
    }
    printTree(root->right, depth + 1);
    for (int i = 0; i < depth * PRINT_TREE_INDENT_STEP; i++) {
        printf(" ");
    }
    if (root->symbol != '\0') {
        printf("%c(%d)\n", root->symbol, root->frequency);
    } else {
        printf("*(%d)\n", root->frequency);
    }
    printTree(root->left, depth + 1);
}


HuffmanNode *popHuffmanNode(HuffmanNode *dummy)
{
    if (linkedListLength < 1){
        fprintf(stderr, "cannot pop, empty linked list\n");
        exit(1);
    }
    HuffmanNode *temp = dummy->next;
    dummy->next = temp->next;
    linkedListLength--;
    return temp;
}

HuffmanNode *createHuffmanTree(HuffmanNode *dummy)
{
    HuffmanNode *newInternalNode, *node1, *node2;
    int sumNodeFrequencies;
    while (linkedListLength > 1){
        node1 = popHuffmanNode(dummy);
        node2 = popHuffmanNode(dummy);
        sumNodeFrequencies = node1->frequency + node2->frequency;
        newInternalNode = createHuffmanNode(INTERNAL_NODE_CODE, sumNodeFrequencies);
        newInternalNode->left = node1;
        newInternalNode->right = node2;
        insertHuffmanNode(newInternalNode, dummy);
    }
    return dummy->next;
}

void printHuffmanNodeLinkedList(HuffmanNode *dummy)
{
    HuffmanNode *node = dummy->next;
    while (node){
        printf("symbol: %d, frequency: %d\n", node->symbol, node->frequency);
        node = node->next;
    }
}

void insertHuffmanNode(HuffmanNode *newNode, HuffmanNode *dummy)
{
    int currentFrequency = newNode->frequency;
    HuffmanNode *prev = dummy;
    HuffmanNode *cur = dummy->next;
    
    while (cur && currentFrequency > cur->frequency){
        prev = cur;
        cur = cur->next;
    }
    
    prev->next = newNode;
    newNode->next = cur;
    linkedListLength++;
}

HuffmanNode *createLinkedList(int symbolFrequencies[])
{
    HuffmanNode *dummy = createHuffmanNode(INTERNAL_NODE_CODE, 0);
    HuffmanNode *newNode;
    int currentFrequency;
    for (int i = 0; i < EXTENDED_ASCII_SET_SIZE; i++){
        currentFrequency = symbolFrequencies[i];
        if (currentFrequency > 0){
            newNode = createHuffmanNode(i, currentFrequency);
            insertHuffmanNode(newNode, dummy);
        }
    }
    return dummy;
}

HuffmanNode *createHuffmanNode(unsigned char symbol, int frequency)
{
    SymbolCodeNode *symbolCodeNode = malloc(sizeof(SymbolCodeNode));
    if (!symbolCodeNode) {
        fprintf(stderr, "Memory allocation failed for SymbolCodeNode\n");
        exit(1);
    }
    symbolCodeNode->bitCount = 0;
    symbolCodeNode->code = 0;

    HuffmanNode *huffmanNode = malloc(sizeof(HuffmanNode));
    if (!huffmanNode) {
        fprintf(stderr, "Memory allocation failed for HuffmanNode\n");
        exit(1);
    }
    huffmanNode->symbolCodeNode = symbolCodeNode;
    huffmanNode->symbol = symbol;
    huffmanNode->frequency = frequency;
    huffmanNode->left = NULL;
    huffmanNode->right = NULL;
    huffmanNode->next = NULL;
    
    return huffmanNode;
}