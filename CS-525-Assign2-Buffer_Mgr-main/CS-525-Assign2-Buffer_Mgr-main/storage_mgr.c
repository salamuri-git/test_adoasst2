#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "storage_mgr.h"
#include "dberror.h"


extern void initStorageManager()
{
  printf("**********Storage Manager V0.0.1 initialized**********\n");
}
// Creation of the page file
extern RC createPageFile(char *fName) {
    // Check if file name is NULL
     FILE *pFile = fopen(fName,"w");
    
    SM_PageHandle str = malloc(PAGE_SIZE); //apply for a page using malloc in C
    if (str==NULL)
        return RC_WRITE_FAILED;
    memset(str,'\0',PAGE_SIZE); //fill a page full of '\0'
    
    fprintf(pFile,"%d\n",1); //write total pages=1
    fwrite(str, sizeof(char), PAGE_SIZE, pFile);
    fclose(pFile);
    
    free(str);
    str=NULL;
    return RC_OK;
}

extern RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{

  FILE *pFile = fopen(fileName,"r+");
    if (!pFile)
        return RC_FILE_NOT_FOUND;
    //write fHandle
    int total_pages;
    fscanf(pFile,"%d\n",&total_pages);
    
    fHandle->fileName=fileName;
    fHandle->totalnumberPgs=total_pages; //total numbers, not offset
    fHandle->curPagePos=0;  //but this is an offset
    fHandle->mgmtInfo=pFile; //POSIX file descriptor
    
    return RC_OK;
}

extern RC closePageFile(SM_FileHandle *fHandle)
{
  // Check if the file handle is NULL or if the file is not initialized
   int fail=fclose(fHandle->mgmtInfo); //succeed when 0
    if (fail)
        return RC_FILE_HANDLE_NOT_INIT;
    
    //free(fHandle->fileName);
    fHandle->fileName= NULL;
    //free(fHandle->mgmtInfo);
    fHandle->mgmtInfo= NULL;
    return RC_OK;
  }

  

extern RC destroyPageFile(char *fileName)
{
    int fail=remove(fileName); //succeed=0 error=-1
    if (fail)
        return RC_FILE_NOT_FOUND;
    
    return RC_OK;
}

// Author: Rana Feyza Soylu
extern RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  if (pageNum>fHandle->totalnumberPgs || pageNum<0)
        return RC_READ_NON_EXISTING_PAGE;
    if (fHandle->mgmtInfo==NULL)
        return RC_FILE_NOT_FOUND;
    
    fseek(fHandle->mgmtInfo, 5+pageNum*PAGE_SIZE, SEEK_SET);
    fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo); //return total number of elements
    fHandle->curPagePos=pageNum;
    
    return RC_OK;
}

// Author: Rana Feyza Soylu
int getBlockPos (SM_FileHandle *fHandle){
    return fHandle->curPagePos;
}
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return readBlock(0, fHandle, memPage);
}
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return readBlock(fHandle->curPagePos-1, fHandle, memPage);
}
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return readBlock(fHandle->curPagePos+1, fHandle, memPage);
}
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return readBlock(fHandle->totalnumberPgs-1, fHandle, memPage); //convert to offset
}


RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (pageNum > (fHandle->totalnumberPgs) || pageNum<0)
        return RC_WRITE_FAILED;
    
    int fail = fseek(fHandle->mgmtInfo, 5+pageNum*PAGE_SIZE, SEEK_SET);
    if (fail)
        return RC_WRITE_FAILED;
    
    fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
    fHandle->curPagePos = pageNum;
    return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    SM_PageHandle str = (char *) calloc(PAGE_SIZE,1); //apply for a page filled with zero bytes
    //append the page
    
    RC return_value = writeBlock(fHandle->totalnumberPgs, fHandle, str);
    //if append new, totalnumberPgs can be used as an offset
    if (return_value!=RC_OK)
    {
        free(str);
        str=NULL;
        return return_value;
    }
    fHandle->curPagePos=fHandle->totalnumberPgs;
    fHandle->totalnumberPgs+=1;
    //change total pages in file
    rewind(fHandle->mgmtInfo);//reset file pointer
    fprintf(fHandle->mgmtInfo,"%d\n",fHandle->totalnumberPgs);
    fseek(fHandle->mgmtInfo,5+(fHandle->curPagePos)*PAGE_SIZE,SEEK_SET); //recover file pointer
    
    free(str);
    str=NULL;
    return RC_OK;
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)

{
    if (fHandle->totalnumberPgs < numberOfPages)
    {
        int diff = numberOfPages - fHandle->totalnumberPgs;
        RC returnCode;
        int i;
        for (i=0; i < diff; i++)
        {
            returnCode = appendEmptyBlock(fHandle);
            if (returnCode!=RC_OK)
                return returnCode;
        }
    }
    return RC_OK;
}
