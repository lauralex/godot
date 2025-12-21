/**************************************************************************/
/*  frame_allocator.cpp                                                   */
/**************************************************************************/

#include "frame_allocator.h"
#include "core/os/memory.h"
#include "core/error/error_macros.h"

FrameAllocatorPage::FrameAllocatorPage(size_t p_size) {
	size = p_size;
	data = (uint8_t *)memalloc(size);
	offset = 0;
	next = nullptr;
}

FrameAllocatorPage::~FrameAllocatorPage() {
	if (data) {
		memfree(data);
	}
}

FrameAllocator::FrameAllocator(size_t p_page_size_mb) {
	page_size = p_page_size_mb * 1024 * 1024;
	first_page = memnew(FrameAllocatorPage(page_size));
	head = first_page;
}

FrameAllocator::~FrameAllocator() {
	FrameAllocatorPage *p = first_page;
	while (p) {
		FrameAllocatorPage *next = p->next;
		memdelete(p);
		p = next;
	}
	first_page = nullptr;
	head = nullptr;
}

void *FrameAllocator::alloc(size_t p_bytes) {
	size_t align = 16;
	size_t padding = 0;

	if (head->offset % align != 0) {
		padding = align - (head->offset % align);
	}

	if (head->offset + padding + p_bytes > head->size) {
		// Try next page
		if (head->next) {
			head = head->next;
			head->offset = 0;
			padding = 0;
		} else {
			// Allocate new page
			size_t new_size = page_size;
			if (p_bytes + align > new_size) {
				new_size = p_bytes + align; // Accommodate large alloc
			}
			FrameAllocatorPage *new_page = memnew(FrameAllocatorPage(new_size));
			head->next = new_page;
			head = new_page;
			padding = 0;
		}

		// If the reused page is too small (allocation larger than page_size)
		if (head->size < p_bytes + padding) {
             // Create a large page and insert it
             size_t huge_size = p_bytes + align;
             FrameAllocatorPage *huge_page = memnew(FrameAllocatorPage(huge_size));

             // Insert huge_page into the list after current head (which is empty/unused for this alloc)
             // Actually, if we just switched to 'head' and it's too small, we can just use 'head' for small allocs later?
             // But 'alloc' must return now.
             // We can insert huge_page *after* head, and advance to it.
             FrameAllocatorPage *next_backup = head->next;
             head->next = huge_page;
             huge_page->next = next_backup;
             head = huge_page;
             padding = 0;
		}
	}

	head->offset += padding;
	void *ptr = head->data + head->offset;
	head->offset += p_bytes;

	return ptr;
}

void FrameAllocator::reset() {
	head = first_page;
	FrameAllocatorPage *p = first_page;
	while (p) {
		p->offset = 0;
		p = p->next;
	}
}

size_t FrameAllocator::get_capacity() const {
	size_t c = 0;
	FrameAllocatorPage *p = first_page;
	while (p) {
		c += p->size;
		p = p->next;
	}
	return c;
}

size_t FrameAllocator::get_usage() const {
	size_t u = 0;
	FrameAllocatorPage *p = first_page;
	while (p) {
		u += p->offset;
		p = p->next;
	}
	return u;
}
