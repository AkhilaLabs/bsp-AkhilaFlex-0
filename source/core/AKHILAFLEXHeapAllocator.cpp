/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/**
  * A simple 32 bit block based memory allocator. This allows one or more memory segments to
  * be designated as heap storage, and is designed to run in a static memory area or inside the standard C
  * heap for use by the AKHILAFLEX runtime. This is required for several reasons:
  *
  * 1) It reduces memory fragmentation due to the high churn sometime placed on the heap
  * by ManagedTypes, fibers and user code. Underlying heap implentations are often have very simplistic
  * allocation pilicies and suffer from fragmentation in prolonged use - which can cause programs to
  * stop working after a period of time. The algorithm implemented here is simple, but highly tolerant to
  * large amounts of churn.
  *
  * 2) It allows us to reuse the 8K of SRAM set aside for SoftDevice as additional heap storage
  * when BLE is not in use.
  *
  * 3) It gives a simple example of how memory allocation works! :-)
  *
  * P.S. This is a very simple allocator, therefore not without its weaknesses. Why don't you consider
  * what these are, and consider the tradeoffs against simplicity...
  *
  * @note The need for this should be reviewed in the future, if a different memory allocator is
  * made availiable in the mbed platform.
  *
  * TODO: Consider caching recently freed blocks to improve allocation time.
  */

#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXHeapAllocator.h"
#include "AKHILAFLEXDevice.h"
#include "AKHILAFLEXCompat.h"
#include "ErrorNo.h"

#if CONFIG_ENABLED(AKHILAFLEX_HEAP_ENABLED)

// A list of all active heap regions, and their dimensions in memory.
HeapDefinition heap[AKHILAFLEX_MAXIMUM_HEAPS] = { };
uint8_t heap_count = 0;
extern "C" int __end__;

#if CONFIG_ENABLED(AKHILAFLEX_DBG) && CONFIG_ENABLED(AKHILAFLEX_HEAP_DBG)
// Diplays a usage summary about a given heap...
void AKHILAFLEX_heap_print(HeapDefinition &heap)
{
	uint32_t	blockSize;
	uint32_t	*block;
    int         totalFreeBlock = 0;
    int         totalUsedBlock = 0;
    int         cols = 0;

    if (heap.heap_start == NULL)
    {
        if(SERIAL_DEBUG) SERIAL_DEBUG->printf("--- HEAP NOT INITIALISED ---\n");
        return;
    }

    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("heap_start : %p\n", heap.heap_start);
    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("heap_end   : %p\n", heap.heap_end);
    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("heap_size  : %d\n", (int)heap.heap_end - (int)heap.heap_start);

	// Disable IRQ temporarily to ensure no race conditions!
    __disable_irq();

	block = heap.heap_start;
	while (block < heap.heap_end)
	{
		blockSize = *block & ~AKHILAFLEX_HEAP_BLOCK_FREE;
        if(SERIAL_DEBUG) SERIAL_DEBUG->printf("[%c:%d] ", *block & AKHILAFLEX_HEAP_BLOCK_FREE ? 'F' : 'U', blockSize*AKHILAFLEX_HEAP_BLOCK_SIZE);
        if (cols++ == 20)
        {
            if(SERIAL_DEBUG) SERIAL_DEBUG->printf("\n");
            cols = 0;
        }

        if (*block & AKHILAFLEX_HEAP_BLOCK_FREE)
            totalFreeBlock += blockSize;
        else
            totalUsedBlock += blockSize;

		block += blockSize;
    }

	// Enable Interrupts
    __enable_irq();

    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("\n");

    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("mb_total_free : %d\n", totalFreeBlock*AKHILAFLEX_HEAP_BLOCK_SIZE);
    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("mb_total_used : %d\n", totalUsedBlock*AKHILAFLEX_HEAP_BLOCK_SIZE);
}


// Diagnostics function. Displays a usage summary about all initialised heaps.
void AKHILAFLEX_heap_print()
{
    for (int i=0; i < heap_count; i++)
    {
        if(SERIAL_DEBUG) SERIAL_DEBUG->printf("\nHEAP %d: \n", i);
        AKHILAFLEX_heap_print(heap[i]);
    }
}
#endif

/**
  * Create and initialise a given memory region as for heap storage.
  * After this is called, any future calls to malloc, new, free or delete may use the new heap.
  * The heap allocator will attempt to allocate memory from heaps in the order that they are created.
  * i.e. memory will be allocated from first heap created until it is full, then the second heap, and so on.
  *
  * @param start The start address of memory to use as a heap region.
  *
  * @param end The end address of memory to use as a heap region.
  *
  * @return AKHILAFLEX_OK on success, or AKHILAFLEX_NO_RESOURCES if the heap could not be allocated.
  *
  * @note Only code that #includes AKHILAFLEXHeapAllocator.h will use this heap. This includes all AKHILAFLEX runtime
  * code, and user code targetting the runtime. External code can choose to include this file, or
  * simply use the standard heap.
  */
int AKHILAFLEX_create_heap(uint32_t start, uint32_t end)
{
    HeapDefinition *h = &heap[heap_count];

    // Ensure we don't exceed the maximum number of heap segments.
    if (heap_count == AKHILAFLEX_MAXIMUM_HEAPS)
        return AKHILAFLEX_NO_RESOURCES;

    // Sanity check. Ensure range is valid, large enough and word aligned.
    if (end <= start || end - start < AKHILAFLEX_HEAP_BLOCK_SIZE*2 || end % AKHILAFLEX_HEAP_BLOCK_SIZE != 0 || start % AKHILAFLEX_HEAP_BLOCK_SIZE != 0)
        return AKHILAFLEX_INVALID_PARAMETER;

	// Disable IRQ temporarily to ensure no race conditions!
    __disable_irq();

    // Record the dimensions of this new heap
    h->heap_start = (uint32_t *)start;
    h->heap_end = (uint32_t *)end;

    // Initialise the heap as being completely empty and available for use.
    *h->heap_start = AKHILAFLEX_HEAP_BLOCK_FREE | (((uint32_t) h->heap_end - (uint32_t) h->heap_start) / AKHILAFLEX_HEAP_BLOCK_SIZE);
    heap_count++;

	// Enable Interrupts
    __enable_irq();

#if CONFIG_ENABLED(AKHILAFLEX_DBG) && CONFIG_ENABLED(AKHILAFLEX_HEAP_DBG)
    AKHILAFLEX_heap_print();
#endif

    return AKHILAFLEX_OK;
}

/**
  * Attempt to allocate a given amount of memory from a given heap area.
  *
  * @param size The amount of memory, in bytes, to allocate.
  * @param heap The heap to allocate memory from.
  *
  * @return A pointer to the allocated memory, or NULL if insufficient memory is available.
  */
void *AKHILAFLEX_malloc(size_t size, HeapDefinition &heap)
{
	uint32_t	blockSize = 0;
	uint32_t	blocksNeeded = size % AKHILAFLEX_HEAP_BLOCK_SIZE == 0 ? size / AKHILAFLEX_HEAP_BLOCK_SIZE : size / AKHILAFLEX_HEAP_BLOCK_SIZE + 1;
	uint32_t	*block;
	uint32_t	*next;

	if (size <= 0)
		return NULL;

	// Account for the index block;
	blocksNeeded++;

	// Disable IRQ temporarily to ensure no race conditions!
    __disable_irq();

	// We implement a first fit algorithm with cache to handle rapid churn...
    // We also defragment free blocks as we search, to optimise this and future searches.
	block = heap.heap_start;
	while (block < heap.heap_end)
	{
		// If the block is used, then keep looking.
		if(!(*block & AKHILAFLEX_HEAP_BLOCK_FREE))
		{
			block += *block;
			continue;
		}

		blockSize = *block & ~AKHILAFLEX_HEAP_BLOCK_FREE;

		// We have a free block. Let's see if the subsequent ones are too. If so, we can merge...
		next = block + blockSize;

		while (*next & AKHILAFLEX_HEAP_BLOCK_FREE)
		{
			if (next >= heap.heap_end)
				break;

			// We can merge!
			blockSize += (*next & ~AKHILAFLEX_HEAP_BLOCK_FREE);
			*block = blockSize | AKHILAFLEX_HEAP_BLOCK_FREE;

			next = block + blockSize;
		}

		// We have a free block. Let's see if it's big enough.
        // If so, we have a winner.
		if (blockSize >= blocksNeeded)
			break;

		// Otherwise, keep looking...
		block += blockSize;
	}

	// We're full!
	if (block >= heap.heap_end)
    {
        __enable_irq();
        return NULL;
    }

	// If we're at the end of memory or have very near match then mark the whole segment as in use.
	if (blockSize <= blocksNeeded+1 || block+blocksNeeded+1 >= heap.heap_end)
	{
		// Just mark the whole block as used.
		*block &= ~AKHILAFLEX_HEAP_BLOCK_FREE;
	}
	else
	{
		// We need to split the block.
		uint32_t *splitBlock = block + blocksNeeded;
		*splitBlock = blockSize - blocksNeeded;
		*splitBlock |= AKHILAFLEX_HEAP_BLOCK_FREE;

		*block = blocksNeeded;
	}

	// Enable Interrupts
    __enable_irq();

	return block+1;
}

/**
  * Attempt to allocate a given amount of memory from any of our configured heap areas.
  *
  * @param size The amount of memory, in bytes, to allocate.
  *
  * @return A pointer to the allocated memory, or NULL if insufficient memory is available.
  */
void *malloc(size_t size)
{
    static uint8_t initialised = 0;
    void *p;

    if (!initialised)
    {
        heap_count = 0;

        if(AKHILAFLEX_create_heap((uint32_t)(&__end__), (uint32_t)(AKHILAFLEX_HEAP_END)) == AKHILAFLEX_INVALID_PARAMETER)
            AKHILAFLEX_panic(AKHILAFLEX_HEAP_ERROR);

        initialised = 1;
    }

    // Assign the memory from the first heap created that has space.
    for (int i=0; i < heap_count; i++)
    {
        p = AKHILAFLEX_malloc(size, heap[i]);
        if (p != NULL)
            break;
    }

    if (p != NULL)
    {
#if CONFIG_ENABLED(AKHILAFLEX_DBG) && CONFIG_ENABLED(AKHILAFLEX_HEAP_DBG)
            if(SERIAL_DEBUG) SERIAL_DEBUG->printf("malloc: ALLOCATED: %d [%p]\n", size, p);
#endif
        return p;
    }

    // We're totally out of options (and memory!).
#if CONFIG_ENABLED(AKHILAFLEX_DBG) && CONFIG_ENABLED(AKHILAFLEX_HEAP_DBG)
    if(SERIAL_DEBUG) SERIAL_DEBUG->printf("malloc: OUT OF MEMORY [%d]\n", size);
#endif

#if CONFIG_ENABLED(AKHILAFLEX_PANIC_HEAP_FULL)
	AKHILAFLEX_panic(AKHILAFLEX_OOM);
#endif

    return NULL;
}

/**
  * Release a given area of memory from the heap.
  *
  * @param mem The memory area to release.
  */
void free(void *mem)
{
	uint32_t	*memory = (uint32_t *)mem;
	uint32_t	*cb = memory-1;

#if CONFIG_ENABLED(AKHILAFLEX_DBG) && CONFIG_ENABLED(AKHILAFLEX_HEAP_DBG)
    if (heap_count > 0)
        if(SERIAL_DEBUG) SERIAL_DEBUG->printf("free:   %p\n", mem);
#endif
    // Sanity check.
	if (memory == NULL)
       return;

    // If this memory was created from a heap registered with us, free it.
    for (int i=0; i < heap_count; i++)
    {
        if(memory > heap[i].heap_start && memory < heap[i].heap_end)
        {
            // The memory block given is part of this heap, so we can simply
	        // flag that this memory area is now free, and we're done.
            if (*cb == 0 || *cb & AKHILAFLEX_HEAP_BLOCK_FREE)
                AKHILAFLEX_panic(AKHILAFLEX_HEAP_ERROR);

	        *cb |= AKHILAFLEX_HEAP_BLOCK_FREE;
            return;
        }
    }

    // If we reach here, then the memory is not part of any registered heap.
    AKHILAFLEX_panic(AKHILAFLEX_HEAP_ERROR);
}

void* calloc (size_t num, size_t size)
{
    void *mem = malloc(num*size);

    if (mem)
        memclr(mem, num*size);

    return mem;
}

void* realloc (void* ptr, size_t size)
{
    void *mem = malloc(size);

    // handle the simplest case - no previous memory allocted.
    if (ptr != NULL && mem != NULL)
    {

        // Otherwise we need to copy and free up the old data.
        uint32_t *cb = ((uint32_t *)ptr) - 1;
        uint32_t blockSize = *cb & ~AKHILAFLEX_HEAP_BLOCK_FREE;

        memcpy(mem, ptr, min(blockSize * sizeof(uint32_t), size));
        free(ptr);
    }

    return mem;
}

// make sure the libc allocator is not pulled in
void *_malloc_r(struct _reent *, size_t len)
{
    return malloc(len);
}

void _free_r(struct _reent *, void *addr)
{
    free(addr);
}

void *
_realloc_r (struct _reent *ptr, void *old, size_t newlen)
{
    (void) ptr;
    return realloc (old, newlen);
}

#else

int AKHILAFLEX_create_heap(uint32_t start, uint32_t end)
{
    (void) start;
    (void) end;

    return AKHILAFLEX_OK;
}

#endif
