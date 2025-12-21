/**************************************************************************/
/*  frame_allocator.h                                                     */
/**************************************************************************/

#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include "core/typedefs.h"

struct FrameAllocatorPage {
	uint8_t *data = nullptr;
	size_t size = 0;
	size_t offset = 0;
	FrameAllocatorPage *next = nullptr;

	FrameAllocatorPage(size_t p_size);
	~FrameAllocatorPage();
};

class FrameAllocator {
	FrameAllocatorPage *head = nullptr;
	FrameAllocatorPage *first_page = nullptr;
	size_t page_size = 0;

public:
	FrameAllocator(size_t p_page_size_mb = 4);
	~FrameAllocator();

	void *alloc(size_t p_bytes);
	void reset();

	size_t get_capacity() const;
	size_t get_usage() const;
};

#endif // FRAME_ALLOCATOR_H
