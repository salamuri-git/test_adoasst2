#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"

#include <stdio.h>
#include <stdlib.h>

Frame *checkIfPinned(BM_BufferPool *const bm, const pgNum pageNum)
{
    // Retrieve the buffer pool's management data
    Buffer *bf = (Buffer *)bm->managementData;

    // Start from the head of the buffer pool
    Frame *currentFrame = bf->head;

    // Traverse through the buffer pool circularly
    while (currentFrame != NULL)
    {
        // Check if the current frame matches the requested page number
        if (currentFrame->currpage == pageNum)
        {
            // If so, increment the fix count and return the frame
            currentFrame->fixCount++;
            return currentFrame;
        }

        // Move to the next frame in the buffer pool
        currentFrame = currentFrame->next;

        // If we have reached back to the head of the buffer pool, stop
        if (currentFrame == bf->head)
            break;
    }

    // If the requested page number is not found in any frame, return NULL
    return NULL;
}

int pinThispage(BM_BufferPool *const bm, Frame *pt, pgNum pageNum)
{
    // Retrieve the buffer pool's management data
    Buffer *bf = (Buffer *)bm->managementData;
    SM_FileHandle fHandle;

    // Open the page file
    RC rt_value = openPageFile(bm->pageFile, &fHandle);
    if (rt_value != RC_OK)
        return rt_value;

    // Ensure capacity for the page
    rt_value = ensureCapacity(pageNum, &fHandle);
    if (rt_value != RC_OK)
    {
        closePageFile(&fHandle);
        return rt_value;
    }

    // If the frame is dirty, write its contents back to disk
    if (pt->dirtyFlag)
    {
        rt_value = writeBlock(pt->currpage, &fHandle, pt->data);
        if (rt_value != RC_OK)
        {
            closePageFile(&fHandle);
            return rt_value;
        }
        pt->dirtyFlag = false;
        bf->writeCount++;
    }

    // Read the requested page into the frame's data
    rt_value = readBlock(pageNum, &fHandle, pt->data);
    if (rt_value != RC_OK)
    {
        closePageFile(&fHandle);
        return rt_value;
    }

    // Increment read count, set current page, and increment fix count
    bf->readCount++;
    pt->currpage = pageNum;
    pt->fixCount++;

    // Close the page file
    closePageFile(&fHandle);

    return RC_OK;
}

/* Replacement Functions*/

RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page,
           const pgNum pageNum, bool fromLRU)
{
    // If called from LRU and not from LRU mode, check if page is already pinned
    if (!fromLRU && checkIfPinned(bm, pageNum))
        return RC_OK;

    // Load buffer pool management data
    Buffer *bf = bm->managementData;
    Frame *pt = bf->head;

    // Find the first available frame in FIFO manner
    bool notFound = true;
    do
    {
        if (pt->fixCount == 0)
        {
            notFound = false;
            break;
        }
        pt = pt->next;
    } while (pt != bf->head);

    // If no available frame found, return error
    if (notFound)
        return RC_IM_NO_MORE_ENTRIES;

    // Pin the page using FIFO strategy
    RC rt_value = pinThispage(bm, pt, pageNum);
    if (rt_value != RC_OK)
        return rt_value;

    // Assign page number and data
    page->pageNum = pageNum;
    page->data = pt->data;

    // Update circular queue pointers
    if (pt == bf->head)
        bf->head = pt->next;
    pt->prev->next = pt->next;
    pt->next->prev = pt->prev;
    pt->prev = bf->tail;
    bf->tail->next = pt;
    bf->tail = pt;
    pt->next = bf->head;
    bf->head->prev = pt;

    return RC_OK;
}

RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page,
          const pgNum pageNum)
{
    // Check if the page is already pinned
    Frame *ptr = checkIfPinned(bm, pageNum);

    if (ptr)
    { // If already pinned, change its priority in LRU
        Buffer *bf = bm->managementData;

        // Move the pinned page to the tail of the list
        if (ptr == bf->head)
        {
            // Adjust pointers to detach the page from its current position
            bf->head = ptr->next;
        }
        // Update pointers to add the page to the tail
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        ptr->prev = bf->tail;
        bf->tail->next = ptr;
        bf->tail = ptr;
        ptr->next = bf->head;
        bf->head->prev = ptr;

        // Assign page number and data
        page->pageNum = pageNum;
        page->data = ptr->data;
    }
    else
    { // If not pinned, use FIFO pinning
        return FIFO(bm, page, pageNum, true);
    }

    return RC_OK;
}

RC CLOCK(BM_BufferPool *const bm, BM_PageHandle *const page,
            const pgNum pageNum)
/*use pointer to scan. No need to reorder queue*/
{
    if (checkIfPinned(bm, pageNum))
        return RC_OK;
    Buffer *bf = bm->managementData;
    Frame *pt = bf->pointer->next;
    bool notfind = true;

    while (pt != bf->pointer)
    {
        if (pt->fixCount == 0)
        {
            if (!pt->refbit) // refbit = 0
            {
                notfind = false;
                break;
            }
            pt->refbit = false; // on the way set all bits to 0
        }
        pt = pt->next;
    };

    if (notfind)
        return RC_IM_NO_MORE_ENTRIES; // no avaliable Frame

    RC rt_value = pinThispage(bm, pt, pageNum);
    if (rt_value != RC_OK)
        return rt_value;

    bf->pointer = pt;
    page->pageNum = pageNum;
    page->data = pt->data;

    return RC_OK;
}



   extern RC LRUK(BM_BufferPool *const bm, BM_PageHandle *const page,
               const pgNum pageNum) {
    Frame *ptr = checkIfPinned(bm, pageNum);

    if (ptr) { // Page is already pinned, update its position in LRU-K
        Buffer *bf = bm->managementData;

        // Move the accessed page to the top of the LRU-K stack
        if (ptr != bf->head) {
            // Adjust pointers to detach the page from its current position
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;

            // Update pointers to add the page to the top
            ptr->next = bf->head;
            bf->head->prev = ptr;
            ptr->prev = NULL;
            bf->head = ptr;
        }

        // Assign page number and data
        page->pageNum = pageNum;
        page->data = ptr->data;
    } else { // Page not pinned, use FIFO pinning
        return FIFO(bm, page, pageNum, true);
    }

    return RC_OK;
}
