#include <stdio.h>
#include <stdlib.h>

#define EXTENDED_ASCII_SET_SIZE 256
#define PRINTABLE_ASCII_LOWER 32
#define PRINTABLE_ASCII_UPPER 126

typedef struct HuffmanNode{
    char character;
    int frequency;
};

typedef struct ListNode{
    struct HuffmanNode *node;
    struct ListNode *next;
};

void collectCharacterFrequencies(int characterFrequencies[], char *fileName);
void printCharacterFrequencies(int characterFrequencies[]);
int isProperProgramUsage(int argc, char *argv[]);
struct ListNode *createLinkedList(int characterFrequencies[]);

int main(int argc, char *argv[])
{
    if (!isProperProgramUsage(argc, argv)){
        return 1;
    }

    int characterFrequencies[EXTENDED_ASCII_SET_SIZE] = {0};
    collectCharacterFrequencies(characterFrequencies, argv[1]);
    
    struct ListNode *head = createLinkedList(characterFrequencies);

    printCharacterFrequencies(characterFrequencies);

    return 0;
}

struct ListNode *createLinkedList(int characterFrequencies[])
{
    struct ListNode *head = malloc(sizeof(struct ListNode));
    struct ListNode *node = head;
    int i = 0;
    while (i < EXTENDED_ASCII_SET_SIZE){
        if (characterFrequencies[i] > 0){
            node->node->character = i;
            node->node->frequency = characterFrequencies[i];
            while (head->node->frequency)
        }
    }
}

int isProperProgramUsage(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
}

void printCharacterFrequencies(int characterFrequencies[])
{
    printf("Character frequencies:\n");
    for (int i = 0; i < EXTENDED_ASCII_SET_SIZE; i++){
        if (characterFrequencies[i] > 0){
            if (i >= PRINTABLE_ASCII_LOWER && i <= PRINTABLE_ASCII_UPPER){
                printf("'%c': %d\n", i, characterFrequencies[i]);
            }else{
                printf("0x%02X: %d\n", i, characterFrequencies[i]); // Non-Printable ascii characters
            }
        }
    }
}

void collectCharacterFrequencies(int characterFrequencies[], char *fileName)
{
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL){
        perror("Error opening file");
        return 1;
    }

    int currentCharacter;

    while ((currentCharacter = fgetc(filePointer)) != EOF){
        characterFrequencies[(unsigned char)currentCharacter]++;
    }

    fclose(filePointer);
}