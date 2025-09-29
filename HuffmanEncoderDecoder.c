#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INTERNAL_NODE_CODE '\0'
#define EXTENDED_ASCII_SET_SIZE 256
#define HUFFMAN_LEFT_TRAVERSAL_CODE '0'
#define HUFFMAN_RIGHT_TRAVERSAL_CODE '1'
#define NOW_ON_LEAF_NODE 0
#define NOW_ON_INTERNAL_NODE 1

typedef struct HuffmanNode{
    char *code;
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
void insertSymbolCodesHelper(char *symbolCodes[], HuffmanNode *root, int depth);
int insertSymbolCodes(char *symbolCodes[], HuffmanNode *root, int depth);

int linkedListLength = 0;
int bitCount = 0;
unsigned char buffer = 0;

int main() {
    int symbolFrequencies[EXTENDED_ASCII_SET_SIZE] = {0};
    symbolFrequencies[97] = 1;
    symbolFrequencies[98] = 2;
    symbolFrequencies[99] = 3;
    symbolFrequencies[100] = 4;
    symbolFrequencies[101] = 5;
    symbolFrequencies[102] = 6;
    symbolFrequencies[103] = 7;
    symbolFrequencies[104] = 8;
    symbolFrequencies[105] = 9;
    symbolFrequencies[106] = 10;
    symbolFrequencies[107] = 11;
    symbolFrequencies[108] = 12;
    symbolFrequencies[109] = 13;
    symbolFrequencies[110] = 14;
    symbolFrequencies[111] = 15;
    symbolFrequencies[112] = 16;
    symbolFrequencies[113] = 17;
    symbolFrequencies[114] = 18;
    symbolFrequencies[115] = 19;
    HuffmanNode *dummy = createLinkedList(symbolFrequencies);
    //printHuffmanNodeLinkedList(dummy);
    HuffmanNode *root = createHuffmanTree(dummy);
    printTree(root, 0);
    int i;
    char *symbolCodes[EXTENDED_ASCII_SET_SIZE] = {NULL};
    insertSymbolCodesHelper(symbolCodes, root, 0);
    for (i = 0; i < EXTENDED_ASCII_SET_SIZE; i++){
        if (symbolCodes[i]){
            printf("%c: %s\n", i, symbolCodes[i]);
        }
    }
}

void insertSymbolCodesHelper(char *symbolCodes[], HuffmanNode *root, int depth)
{
    root->code = malloc(1);   // allocate space for empty string
    root->code[0] = '\0';
    insertSymbolCodes(symbolCodes, root, depth);
}

int insertSymbolCodes(char *symbolCodes[], HuffmanNode *root, int depth)
{
    int result;
    if (!root){
        return NOW_ON_LEAF_NODE;
    }
    if (root->left){
        root->left->code = malloc(depth+2);
        strcpy(root->left->code, root->code);
        root->left->code[depth] = HUFFMAN_LEFT_TRAVERSAL_CODE;
        root->left->code[depth+1] = '\0';
    }
    if (root->right){
        root->right->code = malloc(depth+2);
        strcpy(root->right->code, root->code);
        root->right->code[depth] = HUFFMAN_RIGHT_TRAVERSAL_CODE;
        root->right->code[depth+1] = '\0';
    }
    result = insertSymbolCodes(symbolCodes, root->left, depth+1);
    if (result == NOW_ON_LEAF_NODE){
        symbolCodes[root->symbol] = malloc(depth+1);
        strcpy(symbolCodes[root->symbol], root->code);
        return NOW_ON_INTERNAL_NODE;
    }
    result = insertSymbolCodes(symbolCodes, root->right, depth+1);
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