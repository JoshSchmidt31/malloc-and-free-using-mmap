//Includes:
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//s_block structure, used to store the information of the block
typedef struct s_block *s_block;

struct s_block{
	int size;//It is the size of the data
	s_block next;
	s_block prev;
	int free_fig;  //if free_fig is 0, it shows that the block has been used, if free_fig is 1, this block can be used.
    void *ptr;//This ptr is used to store the particular information of the block
    char data[1];//Just to show the beginning of the data block, it has no particular meaning.   
};

extern void *first_block;
//extern the head pointer;
extern int SPACE_SIZE;
//extern the size of the totol space used to malloc.
extern int MODE;
//extern the MODE which decides the best-fit or worst-fit.
extern s_block head;

s_block get_block(void *p); 
//This function is used in psufree function to get the block information
int valid_addr(void *p);
//It is used to test whether the pointer is in the space.
s_block merge(s_block b);
//After free the block, if the prev and next is also free, we need to merge them to a large block

int psumeminit(int algo, int sizeOfRegion);//To arrange a space use mmap;
void *psumalloc(int size);//malloc the space
int psufree(void *ptr);//free the space
