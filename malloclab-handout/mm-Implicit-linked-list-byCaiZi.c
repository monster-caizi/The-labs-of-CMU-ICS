/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

//#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/



/*
 *  For the small end storage pattern, the ptr is near the tail
 *      —————————————————————  高位
 *      |    pending （4位）  |
 *      —————————————————————
 *      |    head （4位）    |
 *      —————————————————————
 *      |      有效载荷      |
 *      —————————————————————
 *      |       填充        |
 *      —————————————————————
 *      |    pending （4位）  |
 *      —————————————————————
 *      |    foot（4位）     |
 *      —————————————————————  低位

 *
 *
 */


team_t team = {
        /* Team name */
        "CaiTeam",
        /* First member's full name */
        "caizi",
        /* First member's email address */
        "xxxx@gmail.com",
        /* Second member's full name (leave blank if none) */
        "",
        /* Second member's email address (leave blank if none) */
        ""

};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


#define SetFree 0xFFFFFFFE
#define Free 0x00000000
#define Allocated 0x00000001


/* 
 * mm_init - initialize the malloc package.
 */


void *ListHead = NULL, *ListTail = NULL;

void MergeBack(void *ptr) {
    //Judge the next block
    void *TailNextPoint, *HeadNextPoint, *TailPoint;
    (char *) TailNextPoint = (char *) ptr + SIZE_T_SIZE;
    if ((*(int *) TailNextPoint & Allocated) == 0)
        return;

    // Set the new head
    int numberNext = *(int *) TailNextPoint >> 3;
    (char *) HeadNextPoint = (char *) TailFrontPoint + SIZE_T_SIZE + numberNext;
    int number = ((*(int *) ptr >> 3) + (*(int *) TailNextPoint >> 3) + SIZE_T_SIZE * 2);
    *(int *) HeadNextPoint = number << 3;

    //Set the new tail
    (char *) TailPoint = (char *) prt - ((*(int *) ptr >> 3) + SIZE_T_SIZE);
    *(int *) TailPoint = number << 3;


    return;
}

void *MergeFront(void *ptr) {
    //Judge the front block
    void *HeadFrontPoint, *TailFrontPoint, *HeadPoint;
    (char *) HeadFrontPoint = (char *) ptr - SIZE_T_SIZE;
    if ((*(int *) HeadFrontPoint & Allocated) == 0)
        return ptr;

    // Set the new head
    int number = ((*(int *) ptr >> 3) + (*(int *) HeadFrontPoint >> 3) + SIZE_T_SIZE * 2);
    (char *) HeadPoint = (char *) prt + ((*(int *) ptr >> 3) + SIZE_T_SIZE);
    *(int *) HeadPoint = number << 3;

    //Set the new tail
    (char *) TailFrontPoint = (char *) HeadFrontPoint - ((*(int *) HeadFrontPoint >> 3) + SIZE_T_SIZE);
    *(int *) TailFrontPoint = number << 3;


    return TailFrontPoint;
}

void *FindFit(size_t size) {
    if (ListHead == ListTail)
        return NULL;

    //Find the best block
    void *point = ListHead, *BestFit = NULL;
    size_t currentFit = 0;
    while (point != ListTail) {  //need to test
        if ((*(int *) point & Allocated) == 0)
            (char *) point += (*(int *) point >> 3 + 2 * SIZE_T_SIZE);
        else {
            int number = *(int *) point >> 3 + 2 * SIZE_T_SIZE;
            if (number >= size && number < currentFit) {
                (char *) BestFit = (char *) point;
                currentFit = number;
            }
            (char *) point += number;

        }
    }

    if (BestFit == NULL)
        return NULL;

    //Reset the best block
    if (currentFit <= size + SIZE_T_SIZE * 2) {
        *(int *) BestFit = *(int *) BestFit | Allocated;
        (char *) point = (char *) BestFit + (int) currentFit + SIZE_T_SIZE;
        *(int *) point = *(int *) BestFit;
    } else {
        //Set new allocated block
        *(int *) BestFit = newsize << 3 | Allocated;
        (char *) point = (char *) BestFit + (int) newsize + SIZE_T_SIZE;
        *(int *) point = *(int *) BestFit;

        //Set new free block
        (char *) point = (char *) point + SIZE_T_SIZE;
        *(int *) point = (currentFit - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;
        (char *) point = (char *) point + (int) (currentFit - newsize - SIZE_T_SIZE);
        *(int *) point = (currentFit - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;
    }

    return BestFit;
}

int mm_init(void) {
    int newsize = ALIGN(0 + SIZE_T_SIZE * 2);

    void *point, *p;
    p = mem_sbrk(newsize);
    ListHead = ListTail = p;
    (char *) point = (char *) (ListHead);
    int number = 0x00000000 | Allocated;
    *(int *) point = number;
    (char *) point = (char *) point + SIZE_T_SIZE;
    *(int *) point = number;
//    printf("data1 = %d, data2 = %d\n",*((int*)ListHead),*((int*)point));
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    if (size == 0)
        return NULL;
    int newsize = ALIGN(size + SIZE_T_SIZE * 2);

    //Find Fit Block
    void *p = FindFit(newsize);
    if (p != NULL)
        return (void *) ((char *) p + SIZE_T_SIZE);


    //Creat new block
    p = mem_sbrk(newsize);
    if (p == (void *) -1)
        return NULL;
    else {
        ListTail = p;
        *(int *) p = size << 3 | Allocated;
        return (void *) ((char *) p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
    if (ptr == NULL)
        return;

    void *TailPoint, *HeadPoint;
    //Free the block
    //First set the tail
    (char *) TailPoint = (char *) ptr - SIZE_T_SIZE;
    *(int *) TailPoint = *(int *) TailPoint & SetFree;
    //Then set the head
    int number = *(int *) TailPoint >> 3;
    (char *) HeadPoint = (char *) TailPoint + (number + SIZE_T_SIZE);
    *(int *) HeadPoint = *(int *) HeadPoint & SetFree;

    //Merge the front block and back block
    MergeBack(TailPoint);
    MergeFront(TailPoint);

    return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {


    //Special case
    if (size == 0) {
        mm_free(oldptr);
        return NULL;
    }
    if (ptr == NULL) {
        newptr = mm_malloc(size);
        return newptr;
    }

    void *oldptr = ptr - SIZE_T_SIZE, *point;
    size_t copySize;

    //Reduce the block
    int newsize = ALIGN(size + SIZE_T_SIZE * 2);
    int oldBlockSize = *(int *) oldptr >> 3;
    if (oldBlockSize > newsize + 2 * SIZE_T_SIZE) {
        //Set new allocated block
        *(int *) oldptr = newsize << 3 | Allocated;
        (char *) point = (char *) oldptr + (int) newsize + SIZE_T_SIZE;
        *(int *) point = *(int *) oldptr;

        //Set new free block
        (char *) point = (char *) point + SIZE_T_SIZE;
        *(int *) point = (oldBlockSize - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;
        (char *) point = (char *) point + (int) (oldBlockSize - newsize - SIZE_T_SIZE);
        *(int *) point = (oldBlockSize - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;

        MergeBack(point);

        return oldptr;
    }

    //Find in the back block if it is free
    void *HeadOldNextPtr, *point;
    HeadOldNextPtr = (char *) oldptr + *(int *) oldptr >> 3 + 2 * SIZE_T_SIZE;
    if ((*(int *) HeadOldNextPtr & Allocated) == 0 &&
        (*(int *) HeadOldNextPtr >> 3 + oldBlockSize + 2 * SIZE_T_SIZE) > size) {

        int currentNumber = (*(int *) HeadOldNextPtr >> 3 + oldBlockSize + 2 * SIZE_T_SIZE);
        if (currentNumber <= size + SIZE_T_SIZE * 2) {
            *(int *) oldptr = currentNumber << 3 | Allocated;
            (char *) point = (char *) HeadOldNextPtr + *(int *) HeadOldNextPtr >> 3 + SIZE_T_SIZE;
            *(int *) point = *(int *) oldptr;
        } else {
            //Set new allocated block
            *(int *) oldptr = newsize << 3 | Allocated;
            (char *) point = (char *) oldptr + (int) newsize + SIZE_T_SIZE;
            *(int *) point = *(int *) oldptr;

            //Set new free block
            (char *) point = (char *) point + SIZE_T_SIZE;
            *(int *) point = (currentNumber - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;
            (char *) point = (char *) point + (int) (currentNumber - newsize - SIZE_T_SIZE);
            *(int *) point = (currentNumber - newsize - 2 * SIZE_T_SIZE) << 3 & SetFree;

//            MergeBack(point);
        }

        return oldptr;
    }

    //Finding in the front block will not be considered, which may cause data copy error.


    //Find in the other free block
    void *newptr = FindFit(newsize);

    //Create new block
    if (newptr == NULL)
        newptr = mm_malloc(newsize);

    if (newptr == NULL)
        return NULL;

    //copy the data
    copySize = *(int *) ((char *) oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);//need to improve
    return newptr;
}










