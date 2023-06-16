/*------------------------------------------------------------------------------------
 *  Malloc Lab 
 *  Reference Solution 1: 
 *        single free block list with LIFO
 *        double link list
 *        boundary tag 
 *        immediate coalescing       
 *
 *  Tiankai Tu
 *  tutk@cs.cmu.edu
 *------------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "memlib.h"
#include "mm.h"

team_t team = {
    /* Team name to be displayed on webpage */
    "explicit first fit",
    /* First member full name */
    "Harry Q. Bovik",
    /* First member email address */
    "bovik",
    /* Second member full name (leave blank if none) */
    "Andrew Carnegie",
    /* Second member email address (blank if none) */
    "ac"
};


typedef struct listHead {
    void * ptrFirstFreeBlock;
} listHead;

typedef struct freeStruct {
    int size;
    void *pNext;
    void *pPrev;
} freeStruct;

static void *searchFreeList(int reqSize);
static void insertFreeBlock(void *ptrFreeBlock);
static void removeFreeBlock(void *ptrFreeBlock);
static void coalesceFreeBlock(void *ptrBlock);
static void requestMoreSpace(int reqSize);

static void * searchFreeList(int reqSize)
{   
    int boolInFreeList = 0;
    void* ptrFreeBlock;
    int blockSize;

    ptrFreeBlock = (listHead *)(((listHead *)mem_heap_lo())->ptrFirstFreeBlock);

    while ((boolInFreeList == 0)&& (ptrFreeBlock != NULL)){
	blockSize = (*(int *)ptrFreeBlock)&(~(0x7));
	if (blockSize >= reqSize) {
	    boolInFreeList = 1;
	}else
	    ptrFreeBlock = ((freeStruct *)ptrFreeBlock)->pNext;
    }
    if (boolInFreeList == 0) return NULL;
    else return ptrFreeBlock;
}

           
static void insertFreeBlock(void *ptrFreeBlock)
{
    void * ptrPrevBlock;
    void * ptrNextBlock;
  
    ptrPrevBlock = mem_heap_lo();
    ptrNextBlock = ((listHead *)ptrPrevBlock)->ptrFirstFreeBlock;

    /* update the double linked list 
       with LIFO scheme*/
    ((freeStruct *)ptrFreeBlock)->pNext = ptrNextBlock;
    if (ptrNextBlock != NULL) ((freeStruct *)ptrNextBlock)->pPrev = ptrFreeBlock;

    ((freeStruct *)ptrFreeBlock)->pPrev = ptrPrevBlock;
    ((listHead *)mem_heap_lo())->ptrFirstFreeBlock = ptrFreeBlock;
}      


static void removeFreeBlock(void *ptrFreeBlock)
{
    void *pNextFree, *pPrevFree;
  
    pNextFree = ((freeStruct *)ptrFreeBlock)->pNext;
    pPrevFree = ((freeStruct *)ptrFreeBlock)->pPrev;

    /* housekeeping the double linked list*/
    if (pNextFree != NULL) {
	((freeStruct *)pNextFree)->pPrev = pPrevFree;
    }
    if ((unsigned int)pPrevFree == (unsigned int)mem_heap_lo())
	((listHead *)mem_heap_lo())->ptrFirstFreeBlock = pNextFree;
    else
	((freeStruct *)pPrevFree)->pNext = pNextFree;
}

static void coalesceFreeBlock(void *ptrBlock)
{
    void *ptrCurrentBlock, *ptrHeaderBlock, *ptrFooterBlock;
    void *ptrBoundaryTag;
    void *ptrFreeBlock;
    int prevTotalSize = 0 ,nextTotalSize = 0, totalSize;
    int thisSize;

    thisSize = (*(int *)ptrBlock) & (~0x7);

    /* coalesce with any proceding free block */
    ptrCurrentBlock = ptrBlock;
    while ((((*(int *)ptrCurrentBlock))&(0x2))==0){ 
	/*previous block is free*/
	int size;

	/* remove the previous block from it's size class list*/
	ptrBoundaryTag = ((char *)ptrCurrentBlock-4);

	size = (*(int *)ptrBoundaryTag) &(~0x7);
	ptrFreeBlock = (char *)ptrCurrentBlock-size;
	removeFreeBlock(ptrFreeBlock);

	/* update current block position and update the header content */
	prevTotalSize += size;
	ptrCurrentBlock = ptrFreeBlock;
    }
    ptrHeaderBlock = ptrCurrentBlock;

    /* coalesce with any following free block */
    ptrCurrentBlock = (char *)ptrBlock+thisSize;
    while (((*(int *)ptrCurrentBlock) &(0x1))==0){/* current block is free*/
	int size;
  
	size = (*(int *)ptrCurrentBlock) & (~0x7);
	removeFreeBlock(ptrCurrentBlock);
    
	nextTotalSize += size;
	ptrCurrentBlock = (char *)ptrCurrentBlock+size;
    }
    ptrFooterBlock = (char *)ptrCurrentBlock - 4;
  
    /* insert the big contiguous chunk*/
    totalSize = prevTotalSize + nextTotalSize + thisSize;
    if (totalSize != thisSize) {
	/* we shall remove "ptrBlock" to generate the new larger block*/
	removeFreeBlock(ptrBlock);

	*(int *)ptrHeaderBlock = totalSize|0x2;
	*(int *)ptrFooterBlock = totalSize|0x2;
	insertFreeBlock(ptrHeaderBlock);
    }
    return;
}


static void requestMoreSpace(int reqSize)
{
    int pagesize = mem_pagesize();
    int numPages = (reqSize + pagesize -1)/pagesize;
    void *ptrNewBlock;
    int totalSize = numPages*pagesize;
    unsigned int prevLastWordMask;

    ptrNewBlock =  (void *)((unsigned int)mem_sbrk(totalSize) - 4);
    if (ptrNewBlock == (void *)-1) {
	printf("ERROR: mem_sbrk failed in requestMoreSpace\n");
	exit(0);
    }

    /* initialize header, inherit bit 1 from the previously useless last word*/
    /* however, reset the fake in use bit at bit 0*/
    prevLastWordMask = (*(int *)ptrNewBlock) & 0x2;
    ((freeStruct *)ptrNewBlock)->size = totalSize | prevLastWordMask;
    /* initialize footer*/
    *(int *)((char *)((char *)(ptrNewBlock) + totalSize) - 4) = 
	     totalSize | prevLastWordMask;

    /* initialize "new" useless last word
       the previous block is free at this moment
       but this word is useless, so its use bit is set*/
    *(int *)((char *)ptrNewBlock + totalSize) = 0x1;

    insertFreeBlock(ptrNewBlock);
    /* immediate coalesce of newly allocated memory space*/
    coalesceFreeBlock(ptrNewBlock);
}


int mm_init (void)
{
    void *ptrFirstFreeBlock;
    int initsize;
    int totalSize;

    initsize = 4+16+4;
    if (mem_sbrk(initsize) == (void *)-1) {
	printf("ERROR: mem_sbrk failed in mm_init\n");
	exit(1);
    }

    ptrFirstFreeBlock = (char *)mem_heap_lo() + 4;
    ((listHead *)mem_heap_lo())->ptrFirstFreeBlock = ptrFirstFreeBlock;

    totalSize = initsize - 4 - 4;
    /* initialize the header and footer*/
    ((freeStruct *)ptrFirstFreeBlock)->size = totalSize|(0x2);
    ((freeStruct *)ptrFirstFreeBlock)->pNext = NULL;
    ((freeStruct *)ptrFirstFreeBlock)->pPrev = mem_heap_lo();
    *(int *)((char *)((char *)ptrFirstFreeBlock+totalSize)-4) = totalSize|(0x2);

    *(int *)((char *)mem_heap_hi() - 3) = 0x1;
    return 0;
}


void *mm_malloc (size_t size)
{
    int reqSize;
    void * ptrFreeBlock = NULL;
    int blockSize;
    void * ptrResult;
    unsigned int oldBit1Mask;

    if (size == 0) return NULL;

    if (size <= 12) reqSize = 16;
    else reqSize = 8 * ((size+4+7)/8);

    ptrFreeBlock = searchFreeList(reqSize);
    if (ptrFreeBlock == NULL){
	requestMoreSpace(reqSize);
	ptrFreeBlock = searchFreeList(reqSize);
    }
  
    blockSize = ((freeStruct *)ptrFreeBlock)->size & (~0x7);
    oldBit1Mask = ((freeStruct *)ptrFreeBlock)->size &(0x2);
    if (blockSize - reqSize >= 16){
	void *splitFreeBlock;

	splitFreeBlock = (char *)ptrFreeBlock + reqSize;
	removeFreeBlock(ptrFreeBlock);

	*(int *)ptrFreeBlock = reqSize|(oldBit1Mask)|0x5;
    
	/* update the free split block header and footer*/
	((freeStruct *)splitFreeBlock)->size =(blockSize - reqSize)|0x2;
	*(int *)((char *)((char *)ptrFreeBlock + blockSize) - 4) = 
	    ((blockSize-reqSize) |0x2);

	insertFreeBlock(splitFreeBlock);
    }
    else{
	/* update the allocated block header*/
	*(int *)ptrFreeBlock |= 0x5;
	/* update the adjacent block header, no matter it's in use, 
	   free or useless */
	*(int *)((char *)ptrFreeBlock+blockSize) |= 0x2;

	/* remove the chosen block from its free list */
	removeFreeBlock(ptrFreeBlock);
    }

    ptrResult = (char *)ptrFreeBlock + 4;
    return ptrResult;
}


void *mm_realloc(void *ptr, size_t size)
{
    unsigned int bit0Mask;
    int reqSize;
    int currentTotalSize,currentPayloadSize;
    int adjustedSize;
    int copySize;
    void *oldHeader;
    void *newPtr;
    void *newHeader;

    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) {
	mm_free(ptr);
	return NULL;
    }
  
    oldHeader = (char *)ptr - 4;
    bit0Mask = (*(int *)oldHeader) & 0x1;
    if (bit0Mask != 1 ) /* no warning is given here*/
	return NULL;

    if (size <= 12) reqSize = 16;
    else reqSize = 8 * ((size+4+7)/8);

    currentTotalSize = (*(int *)oldHeader) & (~0x7);
    currentPayloadSize = currentTotalSize - 4;

    if (currentTotalSize >= reqSize){
	/* now check if the trailer word can fit in or not*/
	if (currentPayloadSize>=(size+4)){
	    /* done, no movement necessary*/
	    /* create/update header and trailer */
	    *(int *)(oldHeader) &= (~0x4);
	    *(int *)((char *)oldHeader+currentPayloadSize) = size;
	    return ptr;
	}
    }

    /* the realloc cannot fit into the original block */
    /* allocate a new block with larger size than requested and do memcopy*/
    adjustedSize = (int) size * 2 + 4;

  
    newPtr = mm_malloc(adjustedSize);
    newHeader = (char *)newPtr-4;
    *(int *)newHeader &= (~0x4);
    reqSize = (*(int *)newHeader)&(~0x7);
    *(int *)((char *)((char *)newHeader+reqSize) - 4) = size;
  
    if (((*(int *)(oldHeader))&0x4) == 0){ /* already realloc block */
	/* reduce the number of memcopy */
	copySize = *(int *)((char *)((char *)ptr + currentPayloadSize) - 4);
    }
    else copySize = currentPayloadSize; /* at maximum two word overhead*/

    memcpy(newPtr,ptr,copySize);
    mm_free(ptr);
    return newPtr;
}



void mm_free (void *ptr)
{
    int payloadSize;
    void * ptrBoundaryTag;
    void * ptrNextBlock;
    void * ptrFreeBlock;

    ptrFreeBlock = (char *)ptr -4;
  
    payloadSize = ((*(int *)ptrFreeBlock)&(~0x7)) - 4;
    ptrBoundaryTag = (char *)ptrFreeBlock + payloadSize;
    ptrNextBlock = (char *)ptr + payloadSize;

    *(int *)(ptrFreeBlock) = *(int *)ptrBoundaryTag = (*(int *)(ptrFreeBlock))&(~0x5);
    (*(int *)ptrNextBlock) &= (~0x2);

    insertFreeBlock(ptrFreeBlock);

    /* immediate coalesce */
    coalesceFreeBlock(ptrFreeBlock);
}
