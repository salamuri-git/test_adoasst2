#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern RC shutdownBuffPoolHelper(BM_BufferPool *const bm)
{
    // write dirty pages back to disk
    RC rcProgram = forceFlushPool(bm);
    if (rcProgram != RC_OK){
        RC_message="Force Flush Failed";
        printError(*RC_message);
    }
        return rcProgram;
    // free up resources
    Buffer *buffpointer = bm->managementData;
    Frame *frmpointer = buffpointer->head;

    while (frmpointer != buffpointer->tail)
    {
        frmpointer = frmpointer->next;
        free(buffpointer->head);
        buffpointer->head = frmpointer;
    }
    free(buffpointer->tail);
    free(buffpointer);

    bm->numberPgs = 0;
    bm->pageFile = NULL;
    bm->managementData = NULL;
    return RC_OK;
}

extern RC forceFlushPoolHelper(BM_BufferPool *const bm)
{
    // Write all dirty pages to disk
    // Check for pinned pages and DO NOT ADD THEM TO DISK
    
    Buffer *buffpointer = bm->managementData;

    SM_FileHandle fileHandler;
    RC rcProgram = openPageFile(bm->pageFile, &fileHandler);
    if (rcProgram != RC_OK)
        return rcProgram;

    Frame *frmpointer = buffpointer->head;
    do
    {
        // If the page is dirty, write it to disk
        if (frmpointer->dirtyFlag)
        {
            rcProgram = writeBlock(frmpointer->currpage, &fileHandler, frmpointer->data);
            if (rcProgram != RC_OK){
                RC_message="ERROR:FAILED TO WRITE DIRTY PAGE\n";
                printError(*RC_message);
                return rcProgram;
            }
            // Reset dirty flag after writing to disk
            frmpointer->dirtyFlag = false;
            buffpointer->writeCount++;
        }
        frmpointer = frmpointer->next;
    } while (frmpointer != buffpointer->head);

    closePageFile(&fileHandler);
    return RC_OK;
}

extern RC initBuffPoolHelper(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numberPgs, ReplacementStrategy strategy,
                  void *stratData)
{
    // error check
    if (numberPgs <= 0) // input check
        return RC_WRITE_FAILED;
    // init bf:bookkeeping data
    Buffer *buffpointer = malloc(sizeof(Buffer));

    if (buffpointer == NULL)
        return RC_WRITE_FAILED;
    buffpointer->numFrames = numberPgs;
    buffpointer->stratData = stratData;
    buffpointer->readCount = 0;
    buffpointer->writeCount = 0;
    // create list
    Frame *newFrm = malloc(sizeof(Frame));
    Statlist *statPointer = malloc(sizeof(Statlist));
    if (newFrm == NULL)
        return RC_WRITE_FAILED;
    newFrm->currpage = NO_PAGE;
    newFrm->refbit = false;
    newFrm->dirtyFlag = false;
    newFrm->fixCount = 0;
    statPointer->fpt = newFrm;
    memset(newFrm->data, '\0', PAGE_SIZE);

    buffpointer->head = newFrm;
    buffpointer->statListHead = statPointer;

    int i = 1; // Start the loop counter at 1
    while (i < numberPgs)
    { 
        Frame *fnew = malloc(sizeof(Frame));
        Statlist *snew = malloc(sizeof(Statlist));
        if (fnew == NULL)
            return RC_WRITE_FAILED;
        fnew->currpage = NO_PAGE;
        fnew->dirtyFlag = false;
        fnew->refbit = false;
        fnew->fixCount = 0;
        memset(fnew->data, '\0', PAGE_SIZE);

        snew->fpt = fnew;
        statPointer->next = snew;
        statPointer = snew;

        newFrm->next = fnew;
        fnew->prev = newFrm;
        newFrm = fnew;
        
        i++; // Increment the loop counter
    }
    statPointer->next = NULL;
    buffpointer->tail = newFrm;
    buffpointer->pointer = buffpointer->head;

    // circular list for clock
    buffpointer->tail->next = buffpointer->head;
    buffpointer->head->prev = buffpointer->tail;

    // init bm
    bm->numberPgs = numberPgs;
    bm->pageFile = (char *)pageFileName;
    bm->strategy = strategy;
    bm->managementData = buffpointer;

    return RC_OK;
}