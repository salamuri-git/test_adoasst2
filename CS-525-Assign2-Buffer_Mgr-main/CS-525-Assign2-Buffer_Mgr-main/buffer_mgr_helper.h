#ifndef BUFFER_MGR_HELPER_H
#define BUFFER_MGR_HELPER_H

#include "buffer_mgr.h"

extern RC shutdownBuffPoolHelper(BM_BufferPool *const bm);
extern RC forceFlushPoolHelper(BM_BufferPool *const bm);
extern RC initBuffPoolHelper(BM_BufferPool *const bm, const char *const pageFileName,
                         const int numberPgs, ReplacementStrategy strategy,
                         void *stratData);
#endif