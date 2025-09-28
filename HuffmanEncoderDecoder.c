#include <stdio.h>
#include <stdlib.h>

#define INTERNAL_NODE_CODE '\0'
#define EXTENDED_ASCII_SET_SIZE 256
#define HUFFMAN_LEFT_TRAVERSAL_CODE 0
#define HUFFMAN_RIGHT_TRAVERSAL_CODE 1

typedef struct HuffmanNode{
    unsigned char symbol;
    int frequency;
    struct HuffmanNode *left, *right;
    struct HuffmanNode *next;
}HuffmanNode;

void insertHuffmanNode(HuffmanNode *newNode, HuffmanNode *dummy);
void printHuffmanNodeLinkedList(HuffmanNode *dummy);
void printTree(HuffmanNode *root, int depth);
HuffmanNode *createLinkedList(int symbolFrequencies[]);
HuffmanNode *createHuffmanNode(char symbol, int frequency);
HuffmanNode *createHuffmanTree(HuffmanNode *dummy);
HuffmanNode *popHuffmanNode(HuffmanNode *dummy);
void writeBit(FILE *file, int bit, unsigned char *buffer, int *bitCount);
void flushBuffer(FILE *file, unsigned char *buffer, int *bitCount);
void insertSymbolCodes(HuffmanNode *root, int depth);

int linkedListLength = 0;
int bitCount = 0;
unsigned char buffer = 0;

int main() {
    int symbolFrequencies[EXTENDED_ASCII_SET_SIZE] = {0};
    symbolFrequencies[99] = 1;
    symbolFrequencies[98] = 3;
    symbolFrequencies[101] = 4;
    symbolFrequencies[97] = 5;
    symbolFrequencies[102] = 6;
    HuffmanNode *dummy = createLinkedList(symbolFrequencies);
    //printHuffmanNodeLinkedList(dummy);
    HuffmanNode *root = createHuffmanTree(dummy);
    printTree(root, 0);
    FILE *file = fopen("out.txt", "wb");
    char bits[] = {"10110010011011011110001101011001000111001010101011"};
    int nbits = 50;
    for (int i = 0; i < nbits; i++){
        writeBit(file, bits[i] - '0', &buffer, &bitCount);
    }
    flushBuffer(file, &buffer, &bitCount);
    fclose(file);
    char *symbolCodes[EXTENDED_ASCII_SET_SIZE] = {NULL};
    //insertSymbolCodes(root, 0);
}

void insertSymbolCodes(HuffmanNode *root, int depth)
{
    if (!root){
        return;
    }
    insertSymbolCodes(root->left, depth+1);
    
}

void writeBit(FILE *file, int bit, unsigned char *buffer, int *bitCount)
{
    *buffer = (*buffer << 1) | (bit & 1);
    (*bitCount)++;
    if (*bitCount == 8){
        fputc(*buffer, file);
        *bitCount = 0;
        *buffer = 0;
    }
}

void flushBuffer(FILE *file, unsigned char *buffer, int *bitCount)
{
    if (*bitCount > 0){
        *buffer <<= 8 - *bitCount;
        fputc(*buffer, file);
        *bitCount = 0;
        *buffer = 0;
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

HuffmanNode *createHuffmanNode(char symbol, int frequency)
{
    HuffmanNode *huffmanNode = malloc(sizeof(HuffmanNode));
    if (!huffmanNode) {
        fprintf(stderr, "Memory allocation failed for HuffmanNode\n");
        exit(1);
    }
    huffmanNode->symbol = symbol;
    huffmanNode->frequency = frequency;
    huffmanNode->left = NULL;
    huffmanNode->right = NULL;
    huffmanNode->next = NULL;
    
    return huffmanNode;
}