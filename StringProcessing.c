#define  _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <ctype.h>

// Maximal word length
#define  MAX_WORD_LEN   31

// Number of different letters in a word
// Case-insensitive
unsigned int lettersCount(const char * w)
{
    unsigned int l[26] = {0};
    unsigned int count = 0;
    for(int i = 0; w[i]; ++i)
        l[w[i]-'a'] = 1;
    for(int i = 0; i < 26; ++i)
        count += l[i];
    return count;
}

// Type for word-count pair
typedef struct Word_
{
    char word[MAX_WORD_LEN+1];
    unsigned int  count;
} Word;

// Node of binary search tree
typedef struct Node_
{
    Word w;
    struct Node_ *l, *r; // Pointer to left/right children
} Node;

// Malloc and init Node
Node * makeNode(const char * w)
{
    Node * n = malloc(sizeof(Node));
    if (n == NULL)
    {
        fprintf(stderr,"No memory\n");
        exit(1);
    }
    n->w.count = 1;
    strcpy(n->w.word,w);
    n->l = n->r = NULL;
    return n;
}

// Binary search tree
typedef struct BST_
{
    Node* root;         // Root of the tree
    unsigned int maxl;  // Number of different letters
    unsigned int adds;  // Total adds counter
} BST;

// Recursive memory free
void freeNode(Node * n)
{
    if (n == NULL) return;
    if (n->l) freeNode(n->l);
    if (n->r) freeNode(n->r);
    free(n);
}

// Functions for BST
// Init BST
void initBST(BST*tree)
{
    tree->maxl = 0;
    tree->adds = 0;
    tree->root = NULL;
}

// Free BST nodes
void delBST(BST*tree)
{
    freeNode(tree->root);
}

// Add Word 'w' to tree 'a'
void addWord(BST*a, const char * w)
{
    // Update total words counter
    a->adds++;
    // How many different letters?
    unsigned int difLets = lettersCount(w);

    // Compare a new number with maximum one.
    if (difLets < a->maxl) return;  // Not enough different letters
    if (difLets > a->maxl)          // New record
    {
        delBST(a);                  // Free and remove old tree
        a->root = makeNode(w);      // Add a new node as a root
        a->maxl = difLets;          // Adjust data
        return;
    }
    // Need to insert word into non-empty tree
    // Search and add w into BST
    // Start from root
    for(Node * s = a->root;;)
    {
        // Compare with a current node
        int cmp = strcmp(w,s->w.word);
        if (cmp < 0) // Look on the left subtree
        {
            if (s->l) { s = s->l; continue; }
            // Left subtree is empty; add our word, exit
            s->l = makeNode(w);
            break;
        }
        else if (cmp > 0) // Look on the right subtree
        {
            if (s->r) { s = s->r; continue; }
            // Right subtree is empty; add our word, exit
            s->r = makeNode(w);
            break;
        }
        else
        {   // Already in tree; adjust counter
            s->w.count++; break;
        }
    }
}

// Recursive output of Node, using counter
void printWord(Node* n, unsigned int * cnt)
{
    if (n == NULL) return;
    // First, output left subtree
    if (n->l) printWord(n->l,cnt);

    // Output current node, adjust counter
    printf("> %d word(s)  %s\n",n->w.count,n->w.word);
    (*cnt)++;

    // Last, output left subtree
    if (n->r) printWord(n->r,cnt);
}


// Output the whole tree and statistics
void printBST(BST*a)
{
    unsigned int cnt = 0;
    printWord(a->root,&cnt);

    printf("Max different letters count: %u\n",a->maxl);
    printf("Total words in file: %d\n", a->adds);
    printf("Total different words with max different letters count: %u\n", cnt);

}

// Read a word from the file
int readNextWord(FILE *f, char w[MAX_WORD_LEN+1])
{                                 // In order not to care about the terminating null character
    memset(w,0,MAX_WORD_LEN+1);   //
    int r = fgetc(f);             // Current symbol
    if (r == EOF) return 0;       // End of file - bye
    if (!isalpha(r))              // Not a-zA-Z
    {
        for(;;)                   // Reading until a-zA-Z or EOF
        {
            r = fgetc(f);
            if (r == EOF) return 0;
            if (isalpha(r)) break;
        }
    }
    // a-zA-Z found, ungetc for reading
    ungetc(r,f);
    // Read a word with no more than MAX_WORD_LEN symbols
    // Extra characters are transferred to a new word
    for(unsigned int i = 0; i < MAX_WORD_LEN; ++i)
    {
        if (isalpha(r = fgetc(f)))
            w[i] = (char)tolower(r);  // Case-insensitive, so convert to lowercase
        else break;
    }
    return 1;
}

int main(int argc, char * argv[])
{
    char fileName[_MAX_PATH+1]; // Filename
    // Get filename
    if (argc > 1)  // From command-line, if any
        strncpy(fileName,argv[1],_MAX_PATH);
    else           // Else from user input
    {
        printf("Enter the file name: ");
        fgets(fileName,_MAX_PATH+1,stdin);
        if (fileName[strlen(fileName)-1] == '\n')
            fileName[strlen(fileName)-1] = '\0';
    }

    // Open the file
    FILE * f = fopen(fileName,"rb");
    if (f == NULL)
    {
        fprintf(stderr,"Can't open the file `%s`\n",fileName);
        return 1;
    }

    // Make BST
    BST a;
    initBST(&a);
    char w[MAX_WORD_LEN+1];

    // Read the whole file
    while(readNextWord(f,w)) addWord(&a,w);

    // Output the result
    printBST(&a);

    //Free memory
    delBST(&a);
}

