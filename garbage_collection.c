#include<stdio.h>
#include<stddef.h>

#define MAX_SIZE_OF_MEMORY 15000
#define MAX_DEGREE 3

char main_memory[MAX_SIZE_OF_MEMORY];

typedef struct meta_data
{
    size_t size;
    int data_status;
    struct meta_data *next;
} meta_data;

typedef struct Node
{
    int data;
    int outptrCount;   // gives degree of node
    int node_index;    // determines the index of node
    int marked;        //used in countinng mechanism to determine if node is marked or not
    struct Node *next[MAX_DEGREE];   //each pointer points to other node
} Node;

meta_data *start_ptr = (void*)(main_memory);


void initialize_memory()
{   
    printf("Memory initialized  Successfully  \n");
    start_ptr->size = MAX_SIZE_OF_MEMORY - sizeof(meta_data);
    start_ptr->data_status = 1;
    start_ptr->next = NULL;
}

void print_memory()
{
    meta_data *temp = start_ptr;
    printf("Memory Layout:\n");

    while (temp != NULL)
    {
        printf("Size: %zu, Status: %d\n", temp->size, temp->data_status);
        temp = temp->next;
    }
    printf("\n");
}

void split_block(meta_data *temp, size_t required_size)
{
    meta_data *next_free_block = (void*)((void*)temp + required_size + sizeof(meta_data));
    next_free_block->size = temp->size - required_size - sizeof(meta_data);
    next_free_block->data_status = 1;
    temp->data_status = 0;
    temp->size = required_size;
    next_free_block->next = temp->next;
    temp->next = next_free_block;  
}

void *allocate(size_t required_size)
{
    meta_data *temp;
    if (start_ptr->size == 0)
    {
        printf("Initializing the memory\n");
        initialize_memory();
    }
    temp = (void*)main_memory;
    void *result = NULL;
    while ((temp->next != NULL) && (temp->size < required_size || temp->data_status == 0))
    {
        temp = temp->next;
        printf("One meta_data block checked\n");
    }
    if (temp->size == required_size)
    {
        printf("Perfect meta_data block is found\n");
        temp->data_status = 0;
        result = (void*)temp++;
    }
    else if (temp->size > required_size)
    {
        printf("Splitting the larger meta_data block\n");
        split_block(temp, required_size);
        result = (void*)temp++;
    }
    else
    {
        printf("Unable to allocate the memory\n");
    }
    return result;
}

void merge_block()
{
    meta_data *curr;
    meta_data *prev;
    curr = (void *)start_ptr;
    while (curr != NULL && curr->next != NULL)
    {
        if (curr->data_status == 1 && curr->next->data_status == 1)
        {   
            printf("merging two adjecent memory blocks \n");
            curr->size += (curr->next->size) + sizeof(meta_data);
            curr->next = curr->next->next;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}


void deallocate(void *dptr)
{
    if (((void *)main_memory <= dptr) && (dptr <= (void *)(main_memory + MAX_SIZE_OF_MEMORY)))
    {
        meta_data *curr = dptr;
        curr->data_status = 1;
        printf("Memory block deallocated successfully\n");
        merge_block();
    }
    else
    {
        printf("Please provide a valid pointer allocated by My_Alloc\n");
    }
}


Node *initialize_node(Node *memory_block, int val, int out_nodes, int idx) {
    if (memory_block != NULL) {
        memory_block->data = val;
        memory_block->outptrCount = out_nodes;
        memory_block->node_index = idx;
        memory_block->marked = 0; // marked=0 -> unmarked, marked=1 -> marked
        for (int i = 0; i < MAX_DEGREE; i++) {
            memory_block->next[i] = NULL;
        }
    }
    return memory_block;
}


void initialize_2d_matrix(int arr[8][8])
{
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            arr[i][j]=0;
        }
    }
}

void built_Adjacency_Matrix(Node *root, int matrix[8][8])
{
    for (int i = 0; i < root->outptrCount; i++)
    {   
        if(root->next[i]!=NULL)
        {
            matrix[root->node_index][root->next[i]->node_index] = 1;
            built_Adjacency_Matrix(root->next[i], matrix);

        }
    }
}

void printMatrix(int matrix[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

void addMatrices(int adj_Matrix1[8][8], int adj_Matrix2[8][8], int res_Matrix[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            res_Matrix[i][j] = adj_Matrix1[i][j] + adj_Matrix2[i][j];
    }
}

void refrence_count_mechanism(int res_matrix[8][8],Node *node_list[])
{    
    int no_of_zeros;
     for (int col = 0; col < 8; col++)
    {
        if (node_list[col]->marked == 0)    // unmarked nodes are explored 
        {
            no_of_zeros = 0;
            for (int row = 0; row < 8; row++)
            {
                if (res_matrix[row][col] == 0)
                    no_of_zeros++;
            }

            if (no_of_zeros == 8)
            {
                deallocate(node_list[col]);
                printf("(Node---%d).\n", node_list[col]->data);
            }
        }
    }
}


void markPhase(int res_Matrix[8][8], Node *node_list[8])
{
    int no_of_zeros;
    for (int col = 0; col < 8; col++)
    {
        if (node_list[col]->marked == 0)
        {
            no_of_zeros = 0;
            for (int row = 0; row < 8; row++)
            {
                if (res_Matrix[row][col] == 0)
                    no_of_zeros++;
            }

            if (no_of_zeros != 8)
            {
                node_list[col]->marked = 1;
                printf("Marking Node -%d as Active \n", node_list[col]->data);
            }
        }
        else
        {
            node_list[col]->marked = 1;
            printf("Marking Node -%d as Active \n", node_list[col]->data);
        }
    }
    printf("\n");
}

void sweepPhase(Node *node_list[8])
{
    for (int i = 0; i < 8; i++)
    {
        if (node_list[i]->marked == 0)
        {
            deallocate((void *)node_list[i]);
            printf("(Node-----%d).\n", node_list[i]->data);
        }

        if (i != 0 && i != 3)
            node_list[i]->marked = 0;
    }
}

int main()
{
    initialize_memory();
          printf("%ld\n",sizeof(Node));
    // creating the nodes 
    
    
    Node *n1 = allocate(sizeof(Node));
     print_memory();
    Node *n2 = allocate(sizeof(Node));
     print_memory();
    Node *n3 = allocate(sizeof(Node));
     print_memory();
    Node *n5 = allocate(sizeof(Node));
     print_memory();
    Node *n7 = allocate(sizeof(Node));
     print_memory();
    Node *n8 = allocate(sizeof(Node));
     print_memory();
    Node *n9 = allocate(sizeof(Node));
     print_memory();
    Node *n10 = allocate(sizeof(Node));
     print_memory();
    
    n1 = initialize_node(n1, 1, 3, 0);
    n2 = initialize_node(n2, 2, 1, 1);
    n3 = initialize_node(n3, 3, 2, 2);
    n5 = initialize_node(n5, 5, 1, 3);
    n7 = initialize_node(n7, 7, 2, 4);
    n8 = initialize_node(n8, 8, 1, 5);
    n9 = initialize_node(n9, 9, 0, 6);
    n10 = initialize_node(n10, 10, 0, 7);



    // linking the nodes
    n1->next[0] = n2;
    n1->next[1] = n9;
    n1->next[2] = n10;

    n3->next[0] = n8;
    n3->next[1] = n10;

    n5->next[0] = n1;

    n7->next[0] = n1;
    n7->next[1] = n8;

    n8->next[0] = n9;
    
    // creating and initializing the adjecency matrices 
    int adjecency_matrix1[8][8];
    int adjecency_matrix2[8][8];
    initialize_2d_matrix(adjecency_matrix1);
    initialize_2d_matrix(adjecency_matrix2);

    // Marking with the root pointers 
    Node *root1;
    Node *root2;
    root1=n5;
    n5->marked=1;  // n5 is marked up as it is being pointed by a pointer 
    root2=n1;
    n1->marked=1;   // n1 is marked up as it is being pointed by a pointer

    //building Adjeecncy matrices with root pointers 
    built_Adjacency_Matrix(root1,adjecency_matrix1);
    built_Adjacency_Matrix(root2,adjecency_matrix2);
    printf("Adjecency Matrices of Root1     : \n");
    printMatrix(adjecency_matrix1);
    printf("Adjecency Matrices of Root2     : \n");
    printMatrix(adjecency_matrix2);

    // Refrence count mechanism  
    printf("Applying Refrence count Mechanism on Heap Memory   :  \n\n");
    Node *node_list[8] = {n1, n2, n3, n5, n7, n8, n9, n10};
    int res_matrix[8][8];
    addMatrices(adjecency_matrix1,adjecency_matrix2,res_matrix);
    refrence_count_mechanism(res_matrix,node_list);
    printf("\n\n\n");

   
    printf("Applying Mark and Sweep Mechanism on Heap Memory   :  \n\n");
    printf("Starting Mark Phase  : \n");
    markPhase(res_matrix,node_list);
    printf("\n\n");
    printf("Starting sweep Phase  : \n");
    sweepPhase(node_list);

}