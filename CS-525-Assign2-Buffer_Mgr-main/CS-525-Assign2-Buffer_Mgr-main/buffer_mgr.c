#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <string.h>
#include <stdlib.h>
#include "replacement_mgr_strat.h"
#include "buffer_mgr_helper.h"

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numberPgs, ReplacementStrategy strategy,
                  void *stratData)
{
    return initBuffPoolHelper(bm, pageFileName, numberPgs, strategy, stratData);
}

RC shutdownBufferPool(BM_BufferPool *const bm)
{
    return shutdownBuffPoolHelper(bm);
}

RC forceFlushPool(BM_BufferPool *const bm)
{
    return forceFlushPoolHelper(bm);
}

RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    // Buffer *buffpointer = bm->managementData;
    // Frame *frmpointer = buffpointer->head;
    // Check if bm pointer is not NULL
    if (bm == NULL)
    {
        RC_message = "ER: NULL Buffer pool pointer!";
        printError(*RC_message);
        return RC_BUFFER_NOT_INIT;
    }

    // Retrieve buffer pointer
    Buffer *buffpointer = bm->managementData;

    // Check if buffer pointer is not NULL
    if (buffpointer == NULL)
    {
        RC_message = "ER: NULL MANAGEMENT DATA!";
        printError(*RC_message);
        return RC_MGMT_POINTER_NULL;
    }

    // Retrieve frame pointer
    Frame *frmpointer = buffpointer->head;

    // Check if frame pointer is not NULL
    if (frmpointer == NULL)
    {
        RC_message = "ER: NULL Frame pointer!";
        printError(*RC_message);
        return RC_FRAME_POINTER_FAILURE;
    }

    do
    {
        frmpointer = frmpointer->next;
        if (frmpointer == buffpointer->head)
        {
            RC_message = "ERR:TRYING TO READ NON_EXISTENT PAGE!!!";
            printError(*RC_message);
            return RC_READ_NON_EXISTING_PAGE;
        }

    } while (frmpointer->currpage != page->pageNum);

    frmpointer->dirtyFlag = !false;
    return RC_OK;
}

RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    // Retrieve buffer and frame pointers
    Buffer *buffpointer = bm->managementData;
    Frame *frmpointer = buffpointer->head;

    // Search for the page in the buffer pool
    for (int i = 0; i <= 1000; i++)
    {
        if (frmpointer->currpage == page->pageNum)
        {
            break;
        }
        frmpointer = frmpointer->next;
        // If we've traversed the entire buffer without finding the page, return error
        if (frmpointer == buffpointer->head)
        {
            RC_message = "ERR: READING NOT EXISTENT PAGE!!";
            printError(*RC_message);
        }
    }

    // If the page is found, decrement its fix count
    if (frmpointer->fixCount > 0)
    {
        frmpointer->fixCount = frmpointer->fixCount - 1;
        // If fix count reaches 0, mark the page as unpinned
        if (frmpointer->fixCount == 0)
            frmpointer->refbit = false;
    }
    else
    {
        // Handle the case where the page is not found (should not happen)
        RC_message = "ER: ATTEMPTING TO UNPIN NON-EXISTENT PAGE!";
        printError(*RC_message);
        return RC_READ_NON_EXISTING_PAGE;
    }

    return RC_OK;
}

RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    // Open the page file
    SM_FileHandle fileHandler;
    RC rcProgram = openPageFile(bm->pageFile, &fileHandler);
    if (rcProgram != RC_OK)
    {
        RC_message = "ERR: FILE NOT FOUND";
        printError(*RC_message);
        return RC_FILE_NOT_FOUND; // Return error if file not found
    }

    // Write the page to disk
    rcProgram = writeBlock(page->pageNum, &fileHandler, page->data);
    if (rcProgram != RC_OK)
    {
        // Print error message and close the file if write fails
        RC_message = "ERR:ERROR WRITING PAGE TO FILE";
        printError(*RC_message);
        closePageFile(&fileHandler);
        return RC_WRITE_FAILED;
    }

    // Increment write count and close the page file
    ((Buffer *)bm->managementData)->writeCount = ((Buffer *)bm->managementData)->writeCount + 1;
    closePageFile(&fileHandler);

    return RC_OK; // Return success
}
RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
           const pgNum pageNum)
{
    if (pageNum < 0)
    {
        RC_message = "ERR: INVALID PAGE NUMBER";
        printError(*RC_message);
        return RC_INVALID_PAGE_VALUE;
    }

    int strategy = bm->strategy;
    if (strategy == RS_FIFO)
        return FIFO(bm, page, pageNum, false);
    else if (strategy == RS_LRU)
        return LRU(bm, page, pageNum);
    else if (strategy == RS_CLOCK)
        return CLOCK(bm, page, pageNum);
    else if (strategy == LRUK)
        return LRUK(bm, page, pageNum);
    else
    {
        RC_message = "ERR:UNKNOWN STRATEGY!!";
        printError(*RC_message);
        return RC_IM_KEY_NOT_FOUND;
    }
}

pgNum *getFrameContents(BM_BufferPool *const bm)
{
    int pageCount = bm->numberPgs;
    // pgNum *pageptr = calloc(bm->numberPgs, sizeof(int));
    pgNum *pageptr = (pgNum *)malloc(bm->numberPgs * sizeof(int));
    if (pageptr != NULL)
    {
        memset(pageptr, 0, bm->numberPgs * sizeof(int));
    }
    else
    {
        RC_message = "ERR: PAGE POINTER MEM ALLOC FAILURE";
        printError(*RC_message);
        return RC_MEM_ALLOC_FAILED;
    }
    Buffer *buffpointer = bm->managementData;
    Statlist *statListPtr = buffpointer->statListHead;
    int idx = 0;
    while (idx < pageCount)
    {
        pageptr[idx] = statListPtr->fpt->currpage;
        statListPtr = statListPtr->next;
        idx++;
    }
    return pageptr;
}

bool *getDirtyFlags(BM_BufferPool *const bm)
{
    bool *pageptr = calloc(bm->numberPgs, sizeof(bool));
    int pageC = (bm->numberPgs); // Set page count
    Buffer *buffpointer = bm->managementData;
    Statlist *spt = buffpointer->statListHead;
    int idx = 0;
    while (idx < pageC)
    {
        if (spt->fpt->dirtyFlag)
            pageptr[idx] = true;
        spt = spt->next;
        idx++;
    }
    return pageptr;
}

int *getFixCounts(BM_BufferPool *const bm)
{
    pgNum *pageNoPtr = calloc(bm->numberPgs, sizeof(int));

    // Check if memory allocation was successful
    if (pageNoPtr == NULL)
    {
        RC_message = "ERROR: MEM ALLOC FAILED!";
        printError(*RC_message);
        return RC_MEM_ALLOC_FAILED;
    }
    Buffer *buffpointer = bm->managementData;
    int pageC = (bm->numberPgs); // Set page count
    Statlist *statPointer = buffpointer->statListHead;
    int i = 0;
    while (i < pageC)
    {
        pageNoPtr[i] = statPointer->fpt->fixCount;
        statPointer = statPointer->next;
        i++;
    }
    return pageNoPtr;
}
int getNumReadIO(BM_BufferPool *const bm)
{
    return ((Buffer *)bm->managementData)->readCount;
}

int getNumWriteIO(BM_BufferPool *const bm)
{
    return ((Buffer *)bm->managementData)->writeCount;
}