#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "psumemory.h"

#define BLOCK_SIZE 40//This is the size of the struct
#define PAGE_SIZE getpagesize()

int MODE;
void *first_block = NULL;
int SPACE_SIZE;
s_block head;//head represents the first information block of the link list
s_block new_base;//new_base represents the new block after split the block
s_block old_base;//old_base represents the allocated block's information
s_block free_base;//free_base represents the point of the block to be freed.

int psumeminit(int algo, int sizeOfRegion)
{
	MODE = algo;
	int page_num = (int)sizeOfRegion/PAGE_SIZE + 4;//It should be 1 here, however, as the struct take some spaces, I add some other space to keep the size of data space 
	first_block = mmap(NULL, page_num * PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1,0);
	if(first_block == (void*) -1)
		return (-1);
	else
	{
		SPACE_SIZE = page_num * PAGE_SIZE - BLOCK_SIZE;
		head = first_block;
		head->size = SPACE_SIZE;
		head->free_fig = 1;
		head->next = NULL;
	}

	return 0;
}

s_block get_block(void *p)//This function is to get the pointer of the struct
{
	char *temp;
	temp = p - BLOCK_SIZE;

	return temp;
}

int valid_addr(void *p)//This function is to test if the pointer is in the range of the space
{
	if(first_block)
	{
		if(p >= first_block && p < first_block + SPACE_SIZE)
			return p;
	}

	return 0;
}

s_block merge(s_block b)//This function is to merge two continous free blocks into one.
{
	if(valid_addr(b->next))
	{
		if(b->next && b->next->free_fig)
		{
			b->size += BLOCK_SIZE + b->next->size;
			b->next = b->next->next;
			if(valid_addr(b->next))
				b->next->prev = b;
		} 
    }
	return (b);
}


void *psumalloc(int size)
{
	s_block temp = NULL;
	s_block find_base = head;
	size = size + BLOCK_SIZE;

	if(first_block)
	{
		int result = 0;//the result will be used to store the judge result.
		if(find_base->next == NULL && find_base->free_fig && find_base->size > size)//If it is the first time to allocate space for data.
			temp = find_base;
		else
		{
			while(find_base)
			{
				result = find_base->free_fig && find_base->size >= size;//If it is free and the space size is enough
				if(result)
				{
					temp = find_base;//It shows that this space can be allocated;
					break;
				}
				else{//If this space cannot allocate, go to the next space
					if(valid_addr(find_base->next))
					{
						find_base = find_base->next;
						continue;
					}
					else
					{
						return (NULL);//we cannot find the space
						break;
					}
				}
			}

			while(valid_addr(find_base->next))
			{
				if(MODE == 0)//Best-fit;
				{
					result = find_base->next->free_fig && find_base->next->size >= size && find_base->next->size < temp->size;
				}
				else if(MODE == 1)//Worst-fit;
				{
					result = find_base->next->free_fig && find_base->next->size > temp->size;
				}
				else
					return (NULL);
				if(result)//take the new space pointer
				{
					temp = find_base->next;
					find_base = find_base->next;
					continue;
				}
				else
				{
					find_base = find_base->next;
					continue;
				}
			}
		}

			if(temp)//spilt the space if the space size is larger than request
			{
				old_base = temp;
				old_base->free_fig = 0;
				if((temp->size - size) >= (BLOCK_SIZE + 4))
				{
					new_base = old_base->data + size;
					new_base->size = temp->size - size - BLOCK_SIZE;
					if(temp->next)
						new_base->next = temp->next;
					else
						new_base->next = NULL;

					new_base->free_fig = 1;
					old_base->size = size;
					old_base->next = new_base;
				}
			}
		return (old_base->data);//return the pointer at the beginning of the data
	}

	else
		return (NULL);
}

int psufree(void *ptr)
{
	if(valid_addr(ptr))
	{
		free_base = get_block(ptr);
		if(free_base->free_fig == 0)
		{
			free_base->free_fig = 1;
			if(free_base == first_block){//It means the block to free is the head
				if(free_base->next && free_base->next->free_fig)
					merge(free_base);
			}
			else{
				if(valid_addr(free_base->prev))
				  {
					if(free_base->prev->free_fig)
						free_base = merge(free_base->prev);
				  }
				if(valid_addr(free_base->next))
				  {
					if(free_base->next->free_fig)
						merge(free_base);
			      }	
		    }
	    }
		else
			return (-1);
	}
	else
		return (-1);

	return 0;
}
