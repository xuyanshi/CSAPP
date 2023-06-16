/*
 *  malloc.c:  A Reasonably Efficient Best-Fit Allocator.
 *
 *  By:  Vinny Furia / vmf@andrew.cmu.edu
 *       Nick Carter / nbc@andrew.cmu.edu
 *
 *  CS213 - Carnegie Mellon University - November 2000
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "memlib.h"
#include "mm.h"

/*  Team Information  */
team_t team = {
    "Tree-based free list (CMU CS 213 students,  Fall 2000)",
    "Vinny Furia",
    "vmf@andrew.cmu.edu",
    "Nick Carter",
    "nbc@andrew.cmu.edu"
};

//int split_parity = 1;

/*  Compilation Options - to be removed eventually   */
#define _DEBUG_          0     /* Prints out lots of crap */


/*  Typedefs for Heap Data Structures  */
typedef struct List {
    void** BLAH;
    struct List* Prev;
    struct List* Next;
    void** BLAH2;
} List;

typedef struct Node {
    struct Node* Left;
    struct Node* Right;
    int Color;
    struct Node* Parent;
} Node;


/*  Named Constants  */
#define HEAP_INITSIZE    8232   /* multiple of 8 */
#define HEAP_GROWSIZE    4096   /* multiple of 8 */
#define REALLOC_GROWSIZE   2048  /* multiple of 8 */
//#define REALLOC_BUBBLE   128     /* blocksize     */
//#define REALLOC_BUBBLEINCREMENT 128
#define RED              1
#define BLACK            0
#define FALSE            0


/*  Masks for Boundary Tags  */
#define SIZE_MASK        0xFFFFFFF8
#define TREE_MASK        4
#define BLOB_MASK        2
#define FREE_MASK        1
#define IN_TREE          (FREE_MASK|TREE_MASK)
#define IN_BLOB          (FREE_MASK|BLOB_MASK)
#define ALLOCATED        0


/*  Named Heap Locations  */
#define treeroot         ((Node**)(char *)mem_heap_lo())
#define blobroot         ((List**)(char *)mem_heap_lo()+1)
#define boundtag_lo      (((int*)((char *)mem_heap_lo()+16)))
#define boundtag_hi      (((int*)((char *)mem_heap_hi()-3 ))) 
#define split_parity     (*((int*)(char *)mem_heap_lo()+12))

/*  Macros for Boundary Tags  */

//  The following 2 are just for use in definitions
#define __LowTag(p)      (*((int*)(p)-1))
#define __HiPrevTag(p)   (*((int*)(p)-2))

//  The next few can be safely used with all block pointers
#define Size(p)          (__LowTag(p) & SIZE_MASK)
#define IsFree(p)        (__LowTag(p) & FREE_MASK)
#define IsInBlob(p)      (__LowTag(p) & BLOB_MASK)
#define IsInTree(p)      (__LowTag(p) & TREE_MASK)
#define NextBlock(p)     ((char *)(p) + Size(p))
#define PrevFree(p)      (__HiPrevTag(p) & FREE_MASK)
#define NextFree(p)      (IsFree(NextBlock(p)))

//  PrevBlock(p) is undefined for the first addressable block.
#define PrevBlock(p)     ((char *)(p) - (__HiPrevTag(p) & SIZE_MASK))


/*  Tree Node Macros  */
#define IsRed(p)        ((p!=NULL) && (((Node*)p)->Color == RED))




/*
 * Invariants:
 *  - The word starting at (mem_heap_lo()) is a pointer to the root
 *    node of the freetree.
 *    If this value is null, then there are no free blocks.
 *  - The word starting at (mem_heap_lo()+8) is a fake upper boundary
 *    tag, with size 0, and flagged as allocated.
 *  - The word starting at (deseg_hi-3) is a fake lower boundary
 *    tag, with size 0, and flagged as allocated.
 *  - Every block has a "front" and "rear" tag.  This tag is 4 bytes
 *    (1 word) being an integer of the size of the block.
 *    The semantics of these tags are as follows:
 *      - By masking off the least significant 3 bits, we get
 *        the size of the block in bytes.
 *      - Combinations of the last 3 bits are interpreted as follows:
 *          0 0 0  =>  Block is allocated
 *          1 1 1  =>  Block is a red node in the free tree.
 *          1 0 1  =>  Block is a black node in the free tree.
 *          0 0 1  =>  Block is on the trailing list of some
 *                     node in the free tree.
 *          0 1 1  =>  Block is free but not in the tree; instead
 *                     it is in the doubly-linked list of recently
 *                     freed nodes (the blob).
 *  - Free blocks have front and rear tags as described above, and
 *    the front tag is followed by three pointers.  These pointers are
 *    (in order):  Left Child, Right Child, List Next.  Each of these
 *    is one word in length (4 bytes).  All Free blocks will be
 *    maintained within a Red-Black tree with blocks of the same
 *    size being in an address ordered linked list.
 *  - Allocated Blocks will have front and rear tags as described
 *    previously with block data between these tags.
 *
 *  - At a
 *
 * Policies:
 *
 *  - Free blocks are coalesced immediately and added to the freetree.
 *  - Allocation is done best-fit.  When there are several candidates for
 *    the best-fit, then the lowest-addressed block is chosen.
 *
 * Conventions:
 *
 *  - Block pointers are treated as (int*)'s
 */

void setTags (void* block, size_t size, int flags)
{
    int* tag1 = (int*)block - 1;
    int* tag2 = (int*)((char *)block+size)-2;
    *tag1 = *tag2 = (size | flags);
}


/*
 * void left_rotate(Node* x)
 *
 *       x        -->        y
 *      / \                 / \
 *     T1  y      -->      x  T3
 *        / \             / \
 *       T2 T3    -->    T1 T2
 *
 * Precondition: x must be non-null with a non-null right child.
 */
void left_rotate (Node* x) {
    Node* y = x->Right;

    x->Right = y->Left;
    if ( y->Left != NULL )
	y->Left->Parent = x;

    //Set the parent to point to y instead of x
    y->Parent = x->Parent;
    if ( x->Parent == NULL )
	*treeroot = y;
    else
	if ( x == x->Parent->Left )
	    //x was on the left of its parent
	    x->Parent->Left = y;
	else
	    //x must have been on the right
	    x->Parent->Right = y;

    y->Left = x;
    x->Parent = y;
}

/*
 * void right_rotate(Node* x)
 *
 *           x      -->       y
 *          / \              / \
 *         y  T3    -->     T1  x
 *        / \                  / \   
 *       T1 T2      -->       T2 T3
 *
 * Precondition: x must be non-null with a non-null left child.
 */
void right_rotate (Node* x) {
    Node* y = x->Left;
    
    x->Left = y->Right;
    if (y->Right != NULL)
	y->Right->Parent = x;
    
    y->Parent = x->Parent;
    
    // Set the parent to point to y instead of x
    if (x->Parent == NULL) 
	*treeroot = y;
    else
	if (x == x->Parent->Left)
	    // x was on the left of its parent
	    x->Parent->Left = y;
	else
	    // x must have been on the right
	    x->Parent->Right = y;
    
    y->Right = x;
    x->Parent = y;
}

/*  Color Functions  */
int isRed (Node* x) {
    return (x != NULL) && (x->Color == RED);
}

int isBlack (Node* x) {
    return (x == NULL) || (x->Color == BLACK);
}

void setblack (Node* x) {
    if (x!=NULL)
	x->Color = BLACK;
}

void setred (Node* x) {
    // NULL nodes are never red.
    x->Color = RED;
}

/*
 * int isLess(Node* x, Node* y)
 *
 * This expression defines the order by which
 * the tree is constructed.  x and y may not be NULL.
 *
 */
int isLess (Node* x, Node *y) {
    return (Size(x) < Size(y)) || ((Size(x) == Size(y)) && x<y);
}

/*
 * int tree_insert(Node* x)
 *
 * Preconditions:
 *     - x should be an established node
 *       (i.e. size flags, IN_TREE set) that
 *       does not appear already in the tree.
 *
 * Postconditions:
 *     Inserts the node into the tree as follows:
 *     - If the tree is currently empty, then we set x
 *       to be the root, color it black, and return 0.
 *     - Otherwise, we insert x as a leaf, color it red,
 *       and return 1.
 *
 */
int tree_insert (Node* x) {
  Node* current = *treeroot;

  //Empty tree --> update root pointer
  if (current == NULL) {
      *treeroot = x;
      x->Parent = NULL;
      x->Right = x->Left = NULL;
      x->Color = BLACK;
      return 0;
  }

  //Non-empty tree --> insert as leaf
  while(1) {
      if (isLess(x,current)) {
	  //x belongs in the left child of current node
	  if (current->Left != NULL)
	      current = current->Left;
	  else {
	      current->Left = x;
	      x->Parent = current;
	      x->Right = x->Left = NULL;
	      x->Color = RED;
	      return 1; 
	  }
      }
      else {
	  //x belongs in the right child of current node
	  if (current->Right != NULL)
	      current = current->Right;
	  else {
	      current->Right = x;
	      x->Parent = current;
	      x->Right = x->Left = NULL;
	      x->Color = RED;
	      return 1;
	  }
      }
  }
}

void freetree_insert (void* ptr, size_t size) {
    Node* x;
    Node* y;

    //Write the tags for the block
    setTags(ptr, size, IN_TREE);
    x = (Node*)ptr;

    //Do a tree insertion
    if (tree_insert(x) == 0){
	// No further work needed.
	return;
    }
    
    /* Move up the tree to restore the red/black property.       */
    /* Invariant: x is red, and red/black properties are every-  */
    /*            where satisfied, except maybe between x and    */
    //            x->Parent.                                     */
    while ( (x->Parent != NULL)
	    && (isRed(x->Parent))
	    && (x->Parent->Parent != NULL)) {
	if ( x->Parent == x->Parent->Parent->Left ) {
	    /* If x's parent is a left, y is x's right 'uncle' */
	    y = x->Parent->Parent->Right;
	    if ( isRed(y) ) {
		/* case 1 - change the colours */
		setblack(x->Parent);  
		setblack(y);
		setred(x->Parent->Parent);
		/* Move x up the tree */
		x = x->Parent->Parent;
	    }
	    else {
		/* y is a black node */
		if ( x == x->Parent->Right ) {
		    /* and x is to the right */ 
		    /* double-rotate . . .  */
		    left_rotate( x->Parent );
		    right_rotate( x->Parent );
		    setblack( x->Left );
		}
		else
		{
		    /* single-rotate */
		    setblack(x);
		    x = x->Parent;
		    right_rotate( x->Parent );
		}
	    }
	}
	else {
	    /* If x's parent is a right, y is x's left 'uncle' */
	    y = x->Parent->Parent->Left;
	    if ( isRed(y) ) {
		/* case 1 - change the colours */
		setblack(x->Parent);
		setblack(y);
		setred(x->Parent->Parent);
		/* Move x up the tree */
		x = x->Parent->Parent;
	    }
	    else {
		/* y is a black node */
		if ( x == x->Parent->Left ) {
		    /* and x is to the left */
		    /* double rotate */
		    right_rotate( x->Parent );
		    left_rotate( x->Parent );
		    setblack(x->Right);
		}
		else {
		    /* single rotate */
		    setblack(x);
		    x = x->Parent;
		    left_rotate( x->Parent );
		}
	    }
	}
    }
    /* Colour the root black */
    setblack(*treeroot);
}


Node* freetree_locate(int size) {
    Node* best = NULL;
    Node* current = *treeroot;

    //Find the smallest (with respect to tree-order) element
    //for which size <= Size(current), assuming that size-
    //insufficiency is preserved by the tree-order.
    while(current != NULL) {
	if (size <= Size(current)) {
	    best = current;
	    current = current->Left;
	}
	else
	    current = current->Right;
    }
    return best;
}

int freetree_locatemax()
{
    Node* n = *treeroot;
    if (n == NULL)
	return 0;
    else
    {
	while (n->Right)
	    n = n->Right;
    }
    return Size(n);
}


void left_child_is2x(Node* x);
void right_child_is2x(Node* x);

//left child is a double-black node.  Fix it.
void left_child_is2x(Node* x){
    Node* sis = x->Right;

    if (sis->Color == RED)
    {
	left_rotate(x);
	x->Color = !(x->Color);
	sis->Color = !(sis->Color);
	sis = x->Right;
    }

    //Now sis is black.  Let's check its children.
    if (isBlack(sis->Right) && isBlack(sis->Left))
    {
	sis->Color = RED;
	if (x->Color == RED)
	{
	    x->Color = BLACK;
	    return;  //done!
	}
	else
	{
	    //move violation up to parent, if any.
	    //if node is root, it's already black, and we're done.
	    if (x->Parent != NULL)
	    {
		if (x->Parent->Left == x)
		    left_child_is2x(x->Parent);
		else
		    right_child_is2x(x->Parent);
	    }
	    return;
	}
    }

    if (isBlack(sis->Right))  //farther child is black
    {
	//make it so that the farther child is red
	right_rotate(sis);
	sis->Color = RED;   //used to be black, old sis
	sis = x->Right;
	sis->Color = BLACK;  //used to be red.  New sis
    }

    //now we know that sis->Right is red. This is fixable.
    left_rotate(x);
    sis->Color = x->Color;      //just to copy.
    x->Color = BLACK;           //was indeterminate.
    sis->Right->Color = BLACK;  //was red.
    return;
}


void right_child_is2x(Node* x){
    Node* sis = x->Left;

    if (sis->Color == RED)
    {
	right_rotate(x);
	x->Color = !(x->Color);
	sis->Color = !(sis->Color);
	sis = x->Left;
    }

    //Now sis is black.  Let's check its children.
    if (isBlack(sis->Right) && isBlack(sis->Left))
    {
	sis->Color = RED;
	if (x->Color == RED)
	{
	    x->Color = BLACK;
	    return;  //done!
	}
	else
	{
	    //move violation up to parent, if any.
	    //if node is root, it's already black, and we're done.
	    if (x->Parent != NULL)
	    {
		if (x->Parent->Left == x)
		    left_child_is2x(x->Parent);
		else
		    right_child_is2x(x->Parent);
	    }
	    return;
	}
    }

    if (isBlack(sis->Left))  //farther child is black
    {
	//make it so that the farther child is red
	left_rotate(sis);
	sis->Color = RED;   //used to be black, old sis
	sis = x->Left;
	sis->Color = BLACK;  //used to be red.  New sis
    }

    //now we know that sis->Left is red. This is fixable.
    right_rotate(x);
    sis->Color = x->Color;      //just to copy.
    x->Color = BLACK;           //was indeterminate.
    sis->Left->Color = BLACK;   //was red.
    return;
}

void freetree_delete( Node* z ) {
    
    /*****************************
     *  delete node z from tree  *
     *****************************/

    if (z == NULL)
	return;
    
    if ((z->Left == NULL || z->Right == NULL) && z->Color == RED)
    {
	Node* child = z->Left ? z->Left : z->Right;  //is black

	if (child)
	    child->Parent = z->Parent;
	
	if (z->Parent == NULL)
	    *treeroot = child;
	else if (z->Parent->Left == z)
	    z->Parent->Left = child;
	else
	    z->Parent->Right = child;
	return;
    }
    else if ((z->Left == NULL || z->Right == NULL) && z->Color == BLACK)
    {
	Node* child = z->Left ? z->Left : z->Right;
	if (child)
	    child->Parent = z->Parent;

	if (z->Parent == NULL)
	{
	    *treeroot = child;
	    setblack(child);
	    return;
	}
	else if (z->Parent->Left == z)
	{
	    z->Parent->Left = child;
	    left_child_is2x(z->Parent);
	    return;
	}
	else
	{
	    z->Parent->Right = child;
	    right_child_is2x(z->Parent);
	    return;
	}
    }
    else if (z->Right->Left == NULL && z->Right->Color == RED)
    {
	//We know that z->Left is non-null
	z->Right->Left = z->Left;
	z->Left->Parent = z->Right;
	z->Right->Parent = z->Parent;
	z->Right->Color = BLACK;

	if (z->Parent == NULL)
	    *treeroot = z->Right;
	else if (z->Parent->Left == z)
	    z->Parent->Left = z->Right;
	else
	    z->Parent->Right = z->Right;
	return;
    }
    else if (z->Right->Left == NULL && z->Right->Color == BLACK)
    {
	z->Right->Left = z->Left;
	z->Left->Parent = z->Right;
	z->Right->Parent = z->Parent;
	z->Right->Color = z->Color;

	if (z->Parent == NULL)
	    *treeroot = z->Right;
	else if (z->Parent->Left == z)
	    z->Parent->Left = z->Right;
	else
	    z->Parent->Right = z->Right;

	right_child_is2x(z->Right);
	return;
    }
    else
    {
	Node* y = z->Right->Left;
	Node  y2;
	while (y->Left)
	    y = y->Left;

	y2 = *y;
	*y = *z;
	if (z->Parent == NULL)
	    *treeroot = y;
	else if (z->Parent->Left == z)
	    z->Parent->Left = y;
	else
	    z->Parent->Right = y;
	y->Left->Parent = y;
	y->Right->Parent = y;

	//now y has replaced z.  Clean up y2, where y used to be.
	y2.Parent->Left = y2.Right;
	if (y2.Right)
	    y2.Right->Parent = y2.Parent;
	if (y2.Color == RED)
	    return;
	else
	{
	    left_child_is2x(y2.Parent);
	    return;
	}
    }
}





















/*
 * Invariant: ptr points to a block that is free.
 * Afterwards, the pointer will be deleted from
 * the relevant data structures, but its tags
 * will not reflect the change.
 */
void delFromWherever (void *ptr)
{
    if (IsInTree(ptr))
	freetree_delete(ptr);
    else if (IsInBlob(ptr))
    {
	//The node is in the blob.  Remove it in O(1) time.
	List* L = ptr;

	if (L->Next != NULL)
	    L->Next->Prev = L->Prev;
	if (L->Prev)
	    L->Prev->Next = L->Next;
	else
	    *blobroot = L->Next;
    }
}

//Takes a block, sets its tags, and puts it in the blob
void queueNewFreeBlock(void* ptr, int size)
{
    //Mark and insert into the blob.
    List* L = ptr;
    
    setTags(ptr, size, IN_BLOB);
    L->Prev = NULL;
    L->Next = *blobroot;
    if (L->Next)
	L->Next->Prev = L;
    *blobroot = L;
}

//takes all items from the blob and inserts into the freetree
void emptyblob()
{
    /*  Move all blob-blocks into the tree  */
    List* N = *blobroot;
    while (N!=NULL)
    {
        List* temp = N->Next;
        freetree_insert(N,Size(N));
        N = temp;
    }
    *blobroot = NULL;
}

int mm_init (void)
{
    //Get the initial space we need
    if (mem_sbrk(HEAP_INITSIZE) == NULL)
        return -1;

    //We have one free block in the tree initially
    *treeroot = NULL;

    //The blob starts out empty.
    *blobroot = NULL;

    //Set tags to prevent coalescing past heap extents
    *boundtag_lo = 0;
    *boundtag_hi = 0;

    split_parity = 0;
    
    queueNewFreeBlock(boundtag_lo+2,HEAP_INITSIZE-24);

    
    return 0;
}

void mm_free (void *ptr)
{
    //coalesce immediately
    int new_size     = Size(ptr);
    void *new_block  = ptr;
    void *prev_block;
    void *next_block;
    if (NextFree(ptr))
    {
	next_block = NextBlock(ptr);
	new_size += Size(next_block);
	delFromWherever(next_block);
    }
    if (PrevFree(ptr))
    {
	prev_block = PrevBlock(ptr);
	new_size += Size(prev_block);
	new_block = prev_block;
	delFromWherever(prev_block);
    }
    //add the coalesced block to the blob.
    queueNewFreeBlock(new_block,new_size);
}


void *mm_realloc(void *ptr,size_t size)
{
    void* next_block = NextBlock(ptr);
    void* ptr2;
    //printf("ARRGH!\n");
    
    if (ptr == NULL)
    {
        //printf("HYARR\n");
        return mm_malloc(size);
    }
    
    if (size == 0)
    {
        //printf("GRRR\n");
        mm_free(ptr);
        return NULL;
    }

    if (Size(ptr) >= (size+8))
    {
        //printf("ARRYA\n");
        return ptr;
    }
        
    //Is grabbing the next block enough?
    if (IsFree(next_block) && (Size(next_block)+Size(ptr) >= size+8))
    {
        size = (int)(Size(next_block))+(int)(Size(ptr));   //add sizes
        delFromWherever(next_block);                       //
        setTags(ptr,size,0);
        return ptr;
    }
    //Can we expand past the next block?
    if (IsFree(next_block) && (NextBlock(next_block) > (char *)boundtag_hi))
    {
        //We can do a combination of heap expansion
        //and coalescing.  So just claim the next block
        //and add it to this block. . . this should flow
        //into the next if automatically.
        delFromWherever(next_block);
        setTags(ptr,Size(ptr)+Size(next_block),0);
        next_block = NextBlock(next_block);
    }
    //is the block the last block?
    if (next_block > (void *)boundtag_hi)  //we are the last block!
    {
        //this should be looked into . . .
        int min_size = ((size+15)&-8)+8;
        int grow_size = min_size - Size(ptr);
	
        grow_size += REALLOC_GROWSIZE;
        grow_size &= REALLOC_GROWSIZE;

        if (mem_sbrk(grow_size) == NULL)
        {
            //no more memory.  Request cannot be satisfied.
            return NULL;
        }
        *boundtag_hi = 0;

	setTags(ptr,Size(ptr)+grow_size,ALLOCATED);
	return ptr;
    }
    
    ptr2 = mm_malloc(size);
    memcpy(ptr2,ptr,Size(ptr)-8);  //should not be size!
    mm_free(ptr);

#if _DEBUG_
    printf("REALLOC: I had %p and got %p . . . \n",ptr,ptr2);
    printAllBlocks();
#endif //_DEBUG_
    return ptr2;
}


void *mm_malloc (size_t size)
{
    void* block;
    int block_size;
    void* leftover_block = NULL;
    int leftover_size;
    size = (size+7)&-8;    //round up to 8
    size += 8;    //account for tags
    if (size < 24)
        size = 24;  //min size
    //printf("malloc(0x%x)\n",size);

    //clears the blob.  Broken b/c of faulty Tinsert
    emptyblob();
    
    
    block = freetree_locate(size);
    
    if (block == NULL)
    {
        //Grow the heap.
        block = (char *)mem_heap_hi()+1;
        block_size = size;
        block_size += HEAP_GROWSIZE;
        block_size &= -HEAP_GROWSIZE;  //mangle so that we get something bigger than
                                 //we need.
        if (mem_sbrk(block_size) == NULL)
        {
            //no more memory.  Request cannot be satisfied.
            return NULL;
        }
        *boundtag_hi = 0; //*((int*)(mem_heap_hi()-3)) = NULL;     // removed int* cast VMF
        if (IsFree(PrevBlock(block)))
        {
            block = PrevBlock(block);
            block_size += Size(block);
            delFromWherever(block);
        }
    }
    else
    {
        block_size = Size(block);
        delFromWherever(block);
    }

    if ((block_size - size)>24)  // can we split it?
    {
        //NOTE: this always splits to the left.  We want a different policy.
        //split the block
        //re-insert the remainder
        if (split_parity)
        {
            leftover_size  = block_size - size;
            block_size     = size;   //cut is just right
            leftover_block = (char *)block + block_size;
            queueNewFreeBlock(leftover_block,leftover_size);
        }
        else
        {
            leftover_size = block_size - size;
            block_size    = size;
            leftover_block = (char *)block + leftover_size;
            queueNewFreeBlock(block,leftover_size);
            block = leftover_block;
        }
        split_parity = !split_parity;
    }
    
    setTags(block,block_size,0); //marks as allocated.
    //printBlock(block);
#if _DEBUG_
    if (printHeap());
    printAllBlocks();
    isRedBlack();
#endif //_DEBUG_
    return block;
}













