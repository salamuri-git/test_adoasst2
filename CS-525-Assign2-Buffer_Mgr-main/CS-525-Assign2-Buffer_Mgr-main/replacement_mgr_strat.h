#ifndef REPLACEMENT_MGR_STRAT_H
#define REPLACEMENT_MGR_STRAT_H

#include "buffer_mgr.h"

// Replacement Strategies

extern RC FIFO (BM_BufferPool *const bm, BM_PageHandle *const page,
           const pgNum pageNum, bool fromLRU);
extern RC LRU (BM_BufferPool *const bm, BM_PageHandle *const page,
          const pgNum pageNum);
extern RC CLOCK (BM_BufferPool *const bm, BM_PageHandle *const page,
            const pgNum pageNum);
extern RC LRUK(BM_BufferPool *const bm, BM_PageHandle *const page,
           const pgNum pageNum);
#endif