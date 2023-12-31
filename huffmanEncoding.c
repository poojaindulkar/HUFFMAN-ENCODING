#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct Tuple
{
    char c;
    int freq;
} Tuple;

typedef struct TreeNode
{
    Tuple data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct PriorityQueue
{
    TreeNode heap[128]; // Min heap
    int size;
} PriorityQueue;

void enqueue(PriorityQueue *p, Tuple e);
void enqueueTreeNode(PriorityQueue *p, TreeNode e);
TreeNode dequeue(PriorityQueue *p);
bool isEmpty(PriorityQueue *p);
TreeNode *encodeText(PriorityQueue *pq);
void inOrder(TreeNode *root);
void encodeTree(TreeNode *root, char codes[128], int codeptr);
int length(char *c);
void writeBinaryStringToFile(const char *binaryString, FILE *Compressed_file);
void decode_original_str(TreeNode *root, char *encoded_string, char *decoded_string, size_t i);

char final_encoded_map[128][128];


int main()
{
    int freq[128];
    char ip_string[100000] = "";
    for (size_t i = 0; i < 128; i++)
    {
        freq[i] = 0;
    }

    FILE *file = fopen("infile.txt", "r");
    if (file == NULL)
    {
        printf("Unable to open the file.\n");
        return 1;
    }

    char line[128];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        strcat(ip_string, line);
    }
    fclose(file);


    for (size_t i = 0; i < strlen(ip_string); i++)
    {
        if (ip_string[i] != '\0')
            freq[(int)ip_string[i]]++;
    }

    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    for (size_t i = 0; i < 128; i++)
    {
        Tuple t;
        if (freq[i] != 0)
        {
            t.c = (char)(i);
            t.freq = freq[i];
            enqueue(pq, t);
        }
    }

    printf("Queue over\n");

    printf("EncodeText \n");
    TreeNode *root = encodeText(pq);
    printf("tree made successfully");

    char codes[128];
    encodeTree(root, codes, 0);
    printf("\n\nCodes are - ");
    printf("\nChar\t\t code\t\t\tsize");
    int resultStrsize = 0;
    int originalStrsize = 0;
    for (size_t i = 0; i < 128; i++)
    {
        if (freq[i] != 0)
        {
            char *code = final_encoded_map[i];
            int l = length(code);
            resultStrsize += l * freq[i];
            originalStrsize += 8 * freq[i];
            printf("\n% -10c\t % -12s\t % 8d\t", (char)i, code, l);
        }
       
    }

    double percentage = (double)(originalStrsize - resultStrsize) / (double)originalStrsize * 100;
    printf("\n\n\nOriginal size = %d\nCompressed Size = %d\nPercentage = %f", originalStrsize, resultStrsize, percentage);

    char encoded_string[strlen(ip_string) * 64];
    memset(encoded_string, '\0', sizeof(encoded_string));
    
    for (size_t i = 0; i < strlen(ip_string); i++)
    {
        strcat(encoded_string, final_encoded_map[(int)ip_string[i]]);
    }

    FILE *Compressed_file = fopen("Compress.txt", "wb");
    if (file == NULL)
    {
        printf("Unable to create the Compress file.\n");
        return 1;
    }
    fwrite(encoded_string, sizeof(char), strlen(encoded_string), Compressed_file);
    fclose(Compressed_file);

    printf("\n\nOriginal :\n%s", ip_string);

    
   // printf("\n\nOriginal :\n%s", ip_string);
    //printf("\nEncoder encoding the original data...");
   // printf("\nEncoded Data - %s\n", encoded_string);

    printf("\n\n\nDecoder decoding the original string...");
    char decoded_string[1000000];
    decode_original_str(root, encoded_string, decoded_string, 0);

    FILE *outfile_Decompress = fopen("Decompresed.txt", "wb");
    if (outfile_Decompress == NULL)
    {
            printf("Unable to create the Compress file.\n");
            return 1;
    }

    fwrite(decoded_string, sizeof(char), strlen(decoded_string), outfile_Decompress);
    fclose(outfile_Decompress);
    
    return 0;
}

int length(char *c)
{
    int l = 0;
    while (c[l])
    {
        l++;
    }
    return l;
}
void writeBinaryStringToFile(const char *binaryString, FILE *Compressed_file)
{
    //unsigned int slen = strlen(binaryString);
    
    unsigned int slen = strlen(binaryString);
    unsigned int len = slen / 8 + !!(slen % 8);
    unsigned char *str = malloc(len);

    unsigned int i;
    for (i = 0; i < slen; i++)
    {
        if (!(i % 8))
            str[i / 8] = 0;
        str[i / 8] = (str[i / 8] << 1) | (binaryString[i] == '1' ? 1 : 0);
    }
    if (slen % 8)
        str[len - 1] <<= (8 - slen % 8);

    fwrite(str, sizeof(char), len, Compressed_file);
    fclose(Compressed_file);
}


void enqueue(PriorityQueue *p, Tuple e)
{
    // Tuple* heap = p->heap;
    p->heap[p->size].data = e;
    p->heap[p->size].left = p->heap[p->size].right=NULL;
    p->size += 1;
    int child = p->size - 1;
    int parent = 0;
    while (child != 0)
    {
        parent = (child - 1) / 2;
        if (p->heap[parent].data.freq >= p->heap[child].data.freq)
        {
            TreeNode temp = p->heap[parent];
            p->heap[parent] = p->heap[child];
            p->heap[child] = temp;
            child = parent;
        }
        else
        {
            break;
        }
    }
}


void enqueueTreeNode(PriorityQueue *p, TreeNode e)
{
    // Tuple* heap = p->heap;
    p->heap[p->size] = e;
    p->size += 1;
    int child = p->size - 1;
    int parent = 0;
    while (child != 0)
    {
        parent = (child - 1) / 2;
        if (p->heap[parent].data.freq >= p->heap[child].data.freq)
        {
            TreeNode temp = p->heap[parent];
            p->heap[parent] = p->heap[child];
            p->heap[child] = temp;
            child = parent;
        }
        else
        {
            break;
        }
    }
}

TreeNode dequeue(PriorityQueue *p)
{

    TreeNode maxItem = p->heap[0];
    p->heap[0] = p->heap[p->size - 1];
    p->heap[p->size - 1].data.c = '\0';
    p->heap[p->size - 1].data.freq = 0;
    p->size -= 1;

    int parent = 0;

    while (parent >= 0)
    {
        int swapped = -1;
        int leftChild = 2 * parent + 1;
        if (leftChild < p->size)
        {
            if (p->heap[parent].data.freq > p->heap[leftChild].data.freq)
            {
                TreeNode temp = p->heap[parent];
                p->heap[parent] = p->heap[leftChild];
                p->heap[leftChild] = temp;
                swapped = leftChild;
            }
        }
        int rightChild = 2 * parent + 2;
        if (rightChild < p->size)
        {
            if (p->heap[parent].data.freq > p->heap[rightChild].data.freq)
            {
                TreeNode temp = p->heap[parent];
                p->heap[parent] = p->heap[rightChild];
                p->heap[rightChild] = temp;
                swapped = rightChild;
            }
        }
        if (swapped == -1)
        {
            break;
        }
        parent = swapped;
    }

    return maxItem;
}

bool isEmpty(PriorityQueue *p)
{
    return p->size == 0;
}

TreeNode *encodeText(PriorityQueue *pq)
{
    TreeNode *root = NULL;
    while (!isEmpty(pq))
    {
        TreeNode min = dequeue(pq);
        if (isEmpty(pq))
            return root;

        TreeNode min2 = dequeue(pq);
        int tempSum = min.data.freq + min2.data.freq;

        TreeNode *tempNode = (TreeNode *)malloc(sizeof(TreeNode));
        Tuple tempData;
        tempData.c = '\0';
        tempData.freq = tempSum;
        tempNode->data = tempData;

        TreeNode *rightTempNode = (TreeNode *)malloc(sizeof(TreeNode));
        rightTempNode->data = min2.data;
        rightTempNode->left = min2.left;
        rightTempNode->right = min2.right;

        TreeNode *leftTempNode = (TreeNode *)malloc(sizeof(TreeNode));
        leftTempNode->data = min.data;
        leftTempNode->left = min.left;
        leftTempNode->right = min.right;

        tempNode->right = rightTempNode;
        tempNode->left = leftTempNode;
        TreeNode temp;
        temp.data = tempNode->data;
        temp.left = tempNode->left;
        temp.right = tempNode->right;

        root = tempNode;
        enqueueTreeNode(pq, temp);
    }

    return root;
}

void encodeTree(TreeNode *root, char codes[128], int codeptr)
{
    if (!root)
        return;
    if (!root->left && !root->right)
    {
        int i = 0, j = 0;
        // final_encoded_map[(int)root->data.c] = codes;
        while (i < codeptr)
        {
            final_encoded_map[(int)root->data.c][j++] = codes[i++];
        }

        // printf("\n%c - %s", root->data.c, codes);
        return;
    }
    if (root->left)
    {
        codes[codeptr] = '0';
        encodeTree(root->left, codes, codeptr + 1);
    }
    if (root->right)
    {
        codes[codeptr] = '1';
        encodeTree(root->right, codes, codeptr + 1);
    }
}

void inOrder(TreeNode *root)
{
    if (!root)
        return;
    inOrder(root->left);
    printf("\n[%c, %d]", root->data.c, root->data.freq);
    inOrder(root->right);
}

void decode_original_str(TreeNode *root, char *encoded_string, char *decoded_string, size_t i)
{
    TreeNode *currNode = root;
    int j = 0;
    while (i < strlen(encoded_string))
    {
        if (encoded_string[i] == '0')
        {
            currNode = currNode->left;
        }
        else
        {
            currNode = currNode->right;
        }
        if (!currNode->left && !currNode->right)
        {
            decoded_string[j++] = currNode->data.c;
            currNode = root;
        }
        i++;

    }
}
