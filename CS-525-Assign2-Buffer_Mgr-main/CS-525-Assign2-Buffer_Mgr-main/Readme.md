# CS 525 Buffer Manager

    Contributors:
    - Abhijnan Acharya A20536263
    - Tanushree Halder A20554883
    - Roshan Hyalij A20547441
    - Rana Feyza Soylu A20465152

---

---
# Buffer_Mgr.c

**function BM initBufferPool**

This file describes the `initBufferPool` function for initializing a Buffer Manager buffer pool.

**Function Signature:**

```c
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numberPgs, ReplacementStrategy strategy,
                  void *stratData);
```

**Purpose:** Initializes a `BM_BufferPool` struct for managing a pool of memory pages for disk I/O buffering.

**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `pageFileName`: Name of the file.
- `numberPgs`: Number of pages in the buffer pool.
- `strategy`: Replacement strategy to use (e.g., `FIFO`, `LRU`).
- `stratData`: Strategy-specific data.

**Return:**

- `RC_OK`: Initialization successful.

**Details:**

1. Checks for null `bm` and initializes properties.
2. Allocates memory for the frame array.
3. Initializes each frame.
4. Sets management data and returns status.



---

**function shutDownBufferPool**

This file describes the 'shutDownBufferPool' function for shutting down a buffer pool.

**Function Signature:**

```c
extern RC shutdownBufferPool(BM_BufferPool *const bm);
```

**Purpose:** Destroys the buffer pool and deallocates all the resources associated with the buffer pool. 

**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `RC_OK`: Initialization successful.

**Details:**

1. Initializes pointer to a frame and assigns it the value of bm->managementData
2. Calls a function named forceFlushPool passing the bm pointer as an argument.
3. Checks if the fix count of the current page frame is not equal to zero. If any page frame has a non-zero fix count, it means that some client is currently using that page, so the function returns RC_PAGES_IN_BUFFER.*/
4. Sets the managementData field of the buffer pool structure pointed to by bm to NULL. This indicates that the buffer pool is now empty.
5. Returns status.


---

**function flushFrame**

This file describes the 'flushFrame' function which flushes a frame.

**Function Signature:**

```c
void flushFrame(BM_BufferPool *const bm, Frame *pageFrame, int pageNum);
```

**Purpose:** Flushes one page frame to the disk.

**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `pageFrame`: Data from the given frame.
- `pageNum`: The number of the page frame to be flushed.

**Return:**

- `void`: Nothing is returned.

**Details:**

1. Opens the page file linked to the buffer pool.
2. Using Writeblock, the data from the page frame is written to the matching page number.
3. The frame is designated as clean by changing the dirtyCount field to 0.
4. Modifies a counter to keep track of how many pages have been written to disk.



---

**function forceFlushPool**

This file describes the 'flushFrame' function which flushes each frame in the buffer pool using the flushFrame function.

**Function Signature:**

```c
extern RC forceFlushPool(BM_BufferPool *const bm);
```

**Purpose:** Causes all dirty pages (with fix count 0) from the buffer pool to be written to disk.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `RC_OK`: Initialization successful.

**Details:**

1. Iterates over each frame in the buffer pool.
2. For each frame, if it's not currently in use and has been modified, it calls flushFrame to write its data to disk.
3. The function returns RC_OK, ensuring all dirty pages are successfully flushed to maintain data consistency.


---

**function markDirty**

This file describes the 'markDirty' function which marks a page as dirty.

**Function Signature:**

```c
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page);
```

**Purpose:** This function marks a given page as dirty.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Given page to check if dirty or not.

**Return:**

- `RC_OK`: Initialization successful.
- `RC_READ_NON_EXISTING_PAGE`: Tried to read a non existent page error.

**Details:**

1. Checks if buffer manager, page file, or page handle pointers are NULL.
2. Marks the frame as dirty since the page has been modified
3. Returns either true or page not found error



---

**function unpinPage**

This file describes the 'unpinPage ' function which unpins a page.

**Function Signature:**

```c
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
```

**Purpose:** This function unpins a given page.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Given page to unpin.

**Return:**

- `RC_OK`: Initialization successful.
- `RC_READ_NON_EXISTING_PAGE`: Tried to read a non existent page error.



---

**function forcePage**

This file describes the 'forcePage ' function which writes the current content of the page back to the page file on disk.

**Function Signature:**

```c
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
```

**Purpose:** This function writes the current content of the page back to the page file on disk.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Given page to write back to disk.

**Return:**

- `RC_OK`: Operation successful.
- `RC_FILE_NOT_FOUND`: Tried to read a non existent page file error.

**Details:**
1. Attempts to open the page file.
2. Writes the page data to disk.
3. Closes the file handle after successful write.




---

**function getFrameContents**

This file describes the 'getFrameContents' function which returns an array with the number of the page frames.

**Function Signature:**

```c
extern RC pgNum *getFrameContents (BM_BufferPool *const bm);
```

**Purpose:** This function returns an array of pgNums (of size numberPgs) where the ith element is the number of the page stored in the ith page frame. An empty page frame is represented using the constant NO PAGE.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `pageNumArr`: Returns the page number array.

**Details:**
1. Fetches frames from managementData.
2. Sets page count.
3. Makes an array of size numberPgs to save the page numbers to.
4. Sets each value in page num array to the current page number.
5. Returns array with page numbers.

   

---

**function getDirtyFlags**

This file describes the 'getDirtyFlags' function which gets the dirty flags.

**Function Signature:**

```c
bool *getDirtyFlags (BM_BufferPool *const bm);
```

**Purpose:** This function returns an array of bools (of size numberPgs) where the ith element is TRUE if the page stored in the ith page frame is dirty. Empty page frames are considered as clean.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `dirtyPagesArr`: Returns the dirty pages array.

**Details:**
1. Fetches frames from managementData.
2. Sets page count.
3. Makes an empty boolean array to represent whether each page frame is dirty or not.
4. Sets each frame to dirty or not dirty.
5. Returns array with dirty pages.


---

**function getFixCounts**

This file describes the 'getFixCounts' function which gets the fix count.

**Function Signature:**

```c
int *getFixCounts (BM_BufferPool *const bm);
```

**Purpose:** This function returns an array of ints (of size numberPgs) where the ith element is the fix count of the page stored in the ith page frame. Return 0 for empty page frames.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `fixCount`: Returns the fix count array.

**Details:**
1. Fetches frames from managementData.
2. Sets page count.
3. Makes an empty int array to store the fix counts for each page frame.
4. Sets fix count of each frame.
5. Returns array with fix counts.




---

**function getNumReadIO**

This file describes the 'getNumReadIO' function which returns the number of pages that has been read from disk.

**Function Signature:**

```c
int getNumReadIO (BM_BufferPool *const bm);
```

**Purpose:** This function returns the number of pages that have been read from disk since a buffer pool has been initialized. 


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `((Buffer *)bm->managementData)->readCount`

**Details:**

1. Access the management data (managementData) of the given buffer pool (bm), assuming it is of type Buffer.
2. Retrieve the readCount attribute from the Buffer structure, which represents the count of read I/O operations.
3. Return the obtained readCount value as the result of the function.



---

**function getNumWriteIO**

This file describes the 'getNumReadIO' function returns the number of pages written to the page file.

**Function Signature:**

```c
int getNumWriteIO (BM_BufferPool *const bm);
```

**Purpose:** This function returns the number of pages written to the page file since the buffer pool has been initialized.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.

**Return:**

- `((Buffer *)bm->managementData)->writeCount`

**Details:**
1. Access the management data (managementData) of the given buffer pool (bm), assuming it is of type Buffer.
2. Retrieve the writeCount attribute from the Buffer structure, which represents the count of write I/O operations.
3. Return the obtained writeCount value as the result of the function.

---

---
# replacement_mgr_strat.c

**Replacement Functions**

**function FIFO**

This file describes the 'FIFO' function implements the First-In-First-Out (FIFO) page replacement strategy for a buffer pool.
**Function Signature:**

```c
RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page,
           const pgNum pageNum, bool fromLRU)
```

**Purpose:** This function implements the First-In-First-Out (FIFO) page replacement strategy in the context of a buffer pool.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Page that is given
- `pageNum`: Current page number we are working on.
- `fromLRU`: bool value

**Return:**

- `RC_OK`: Returns with successful operation of code

**Details:**
1. If called from LRU and not from LRU mode, check if page is already pinned
2. Load buffer pool management data
3. Find the first available frame in FIFO manner
4. If no available frame found, return error
5. Pin the page using FIFO strategy
6. Assign page number and data
7. Update circular queue pointers
8. Return succesful return code

   
---

**function LRU**

This file describes the 'FIFO' function implements the Least Recently Used (LRU) page replacement strategy within a buffer pool.
**Function Signature:**

```c
RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page,
          const pgNum pageNum)
```

**Purpose:** The purpose of this code is to implement the Least Recently Used (LRU) page replacement strategy in the context of a buffer pool. It prioritizes the pinning of a specified page by updating its position in the LRU order if it is already pinned. If the page is not currently in the buffer pool, it falls back to using the First-In-First-Out (FIFO) strategy for pinning the page.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Page that is given
- `pageNum`: Current page number we are working on.

**Return:**

- `RC_OK`: Returns with successful operation of code

**Details:**
1. Check if the page is already pinned
2. If already pinned, change its priority in LRU
3. Move the pinned page to the tail of the list
4. Adjust pointers to detach the page from its current position
5. Update pointers to add the page to the tail
6. Assign page number and data
7. If not pinned, use FIFO pinning
8. Return succesful return code


---

**function CLOCK**

This file describes the 'FIFO' function implements the CLOCK page replacement strategy within a buffer pool.
**Function Signature:**

```c
RC CLOCK(BM_BufferPool *const bm, BM_PageHandle *const page,
            const pgNum pageNum)
```

**Purpose:** The purpose of this code is to implement the CLOCK page replacement strategy in the context of a buffer pool. The CLOCK algorithm scans frames in a circular manner, resetting reference bits and selecting the first unpinned frame with a reference bit of 0 to pin a specified page.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `page`: Page that is given
- `pageNum`: Current page number we are working on.

**Return:**

- `RC_OK`: Returns with successful operation of code

**Details:**
1. Check if the specified page is already pinned in the buffer pool.
2. If the page is already pinned, return RC_OK.
3. Retrieve the buffer pool's management data and initialize a pointer (pt) to the frame next to the current clock hand position.
4. Initialize a boolean variable notfind to true, indicating that an available frame has not been found yet.
5. Iterate through the frames in a circular manner until the clock hand completes one full rotation (reaches the initial position).
6. For each frame, check if it is unpinned (fixCount == 0).
7. If the frame is unpinned, check its reference bit (refbit):
8. If the reference bit is 0, set notfind to false and break out of the loop.
9. If the reference bit is 1, set the reference bit to 0 (simulating a clock hand reset).
10. If no available frame is found after the scan, return RC_IM_NO_MORE_ENTRIES, indicating that there are no unpinned frames.
11. If an available frame is found, pin the specified page using the pinThispage function.
12. Update the clock hand position to the frame where the unpinned page was found.
13. Assign the page number and data to the BM_PageHandle structure.
14. Return RC_OK to indicate a successful page pinning operation.

---

**function LRUK**

LRU-K was not implemented as it is extra credit and as such optional.

---

---

**Additional Functions**

**function checkIfPinned**

This file describes the 'checkIfPinned' function which defines a function checkIfPinned that searches for a frame in a circular buffer pool (BM_BufferPool) with a given page number (pageNum). If the frame with the specified page number is found, its fix count is incremented, and the frame is returned; otherwise, it returns NULL indicating that the requested page number is not present in any frame.

**Function Signature:**

```c
Frame *checkIfPinned(BM_BufferPool *const bm, const pgNum pageNum)
```

**Purpose:** This function checks if a particular page with a given page number is already present in the buffer pool.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `pageNum`: Current page number we are working on.

**Return:**

- `currentFrame`: Returns current frame
- `NULL`: If the requested page number is not found in any frame, return NULL

**Details:**
1. Retrieve the buffer pool's management data
2. Start from the head of the buffer pool
3. Traverse through the buffer pool circularly
4. Check if the current frame matches the requested page number
5. If so, increment the fix count and return the frame
6. Move to the next frame in the buffer pool
7. If we have reached back to the head of the buffer pool, stop
8. If the requested page number is not found in any frame, return NULL

   
---

**function pinThispage**

This file describes the 'pinThispage' function pins a specified page in a buffer pool by ensuring its presence, writing back the contents of the current frame if it's dirty, and then reading the requested page into the frame while updating relevant metadata such as fix count and read count.

**Function Signature:**

```c
int pinThispage(BM_BufferPool *const bm, Frame *pt, pgNum pageNum);
```

**Purpose:** The purpose of this code is to pin a specified page in a buffer pool, ensuring that the page is in the buffer pool's memory. The function handles operations such as opening the page file, ensuring capacity, writing back data if the current frame is dirty, reading the requested page into the frame, and updating metadata like fix count and read count.


**Parameters:**

- `bm`: Pointer to the `BM_BufferPool` struct to be initialized.
- `pageNum`: Current page number we are working on.
- `pt`: A page frame

**Return:**

- `RC_OK`: Returns with successful operation of code

**Details:**
1. Retrieve the buffer pool's management data
2. Open the page file
3. Ensure capacity for the page
4. If the frame is dirty, write its contents back to disk
5. Read the requested page into the frame's data
6. Increment read count, set current page, and increment fix count
7. Close the page file
8. Return return code if successful completion








