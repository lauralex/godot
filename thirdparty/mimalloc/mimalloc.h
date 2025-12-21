#pragma once
#ifndef MIMALLOC_H
#define MIMALLOC_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Standard malloc interface
void* mi_malloc(size_t size);
void* mi_calloc(size_t count, size_t size);
void* mi_realloc(void* p, size_t newsize);
void* mi_expand(void* p, size_t newsize);
void mi_free(void* p);
char* mi_strdup(const char* s);
char* mi_strndup(const char* s, size_t n);
char* mi_realpath(const char* fname, char* resolved_name);

// Extended functionality (simplified for shim)
void* mi_malloc_small(size_t size);
void* mi_zalloc_small(size_t size);
void* mi_zalloc(size_t size);

void* mi_mallocn(size_t count, size_t size);
void* mi_reallocn(void* p, size_t count, size_t size);
void* mi_reallocf(void* p, size_t newsize);

size_t mi_usable_size(const void* p);
size_t mi_good_size(size_t size);

void mi_collect(bool force);
int mi_version(void);
void mi_stats_reset(void);
void mi_stats_merge(void);
void mi_stats_print(void* out);
void mi_options_print(void);

// Aligned allocation
void* mi_malloc_aligned(size_t size, size_t alignment);
void* mi_malloc_aligned_at(size_t size, size_t alignment, size_t offset);
void* mi_zalloc_aligned(size_t size, size_t alignment);
void* mi_zalloc_aligned_at(size_t size, size_t alignment, size_t offset);
void* mi_calloc_aligned(size_t count, size_t size, size_t alignment);
void* mi_calloc_aligned_at(size_t count, size_t size, size_t alignment, size_t offset);
void* mi_realloc_aligned(void* p, size_t newsize, size_t alignment);
void* mi_realloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset);

#ifdef __cplusplus
}
#endif

#endif
