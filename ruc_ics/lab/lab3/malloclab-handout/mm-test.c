/*
 * mm-test.c - Buggy version that produces overlapping blocks
 *  
 * Used for testing the driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* declare mm_check as static */
static int mm_check(void);

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

team_t team = {
    /* Team name you want displayed on the Web page */
    "test",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
  mem_sbrk(64000);
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if ((int)p < 0)
	return NULL;
    else {
        *(size_t *)p = size;
        return mem_heap_lo();

    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/*
 * mm_check - Does not currently check anything
 */
static int mm_check(void)
{
  return 1;
}










