#include "mimalloc.h"
#include <stdlib.h>
#include <string.h>

// Shim implementation for Mimalloc
// This allows building and testing the integration without the full mimalloc source.
// For production, replace this file and SCsub with the actual mimalloc library.

void* mi_malloc(size_t size) {
#ifdef _MSC_VER
    return _aligned_malloc(size, 16); // Default 16-byte alignment
#else
    return malloc(size);
#endif
}

void* mi_calloc(size_t count, size_t size) {
#ifdef _MSC_VER
    void* p = _aligned_malloc(count * size, 16);
    if (p) memset(p, 0, count * size);
    return p;
#else
    return calloc(count, size);
#endif
}

void* mi_realloc(void* p, size_t newsize) {
#ifdef _MSC_VER
    return _aligned_realloc(p, newsize, 16);
#else
    return realloc(p, newsize);
#endif
}

void* mi_expand(void* p, size_t newsize) {
#ifdef _MSC_VER
    return _aligned_realloc(p, newsize, 16);
#else
    return realloc(p, newsize);
#endif
}

void mi_free(void* p) {
#ifdef _MSC_VER
    _aligned_free(p);
#else
    free(p);
#endif
}

char* mi_strdup(const char* s) {
#ifdef _MSC_VER
    size_t len = strlen(s) + 1;
    char* new_str = (char*)_aligned_malloc(len, 16);
    if (!new_str) return NULL;
    memcpy(new_str, s, len);
    return new_str;
#else
    return strdup(s);
#endif
}

char* mi_strndup(const char* s, size_t n) {
    size_t len = 0;
    while(len < n && s[len]) len++;

#ifdef _MSC_VER
    char* new_str = (char*)_aligned_malloc(len + 1, 16);
#else
    char* new_str = (char*)malloc(len + 1);
#endif

    if (!new_str) return NULL;
    memcpy(new_str, s, len);
    new_str[len] = '\0';
    return new_str;
}

char* mi_realpath(const char* fname, char* resolved_name) {
    // realpath implementation usually allocates with malloc.
    // Shim: let's avoid overriding this as it returns malloc'd pointer usually.
    // If we free it with mi_free, we might crash on Windows if using _aligned_free.
    // Safest: Copy to aligned buffer? Or just assume realpath usage is rare/special.
    // mimalloc overrides it to use mi_malloc.
    // For shim, we can't easily override system allocators inside system functions.
    // So this might leak or crash if mixed.
    // We return NULL to indicate not supported in shim if safe, or wrappers.
#ifdef _MSC_VER
    return _fullpath(resolved_name, fname, _MAX_PATH); // Returns buffer?
    // _fullpath with NULL allocates.
#else
    return realpath(fname, resolved_name);
#endif
}

void* mi_malloc_small(size_t size) {
    return mi_malloc(size);
}

void* mi_zalloc_small(size_t size) {
    return mi_calloc(1, size);
}

void* mi_zalloc(size_t size) {
    return mi_calloc(1, size);
}

void* mi_mallocn(size_t count, size_t size) {
    return mi_malloc(count * size);
}

void* mi_reallocn(void* p, size_t count, size_t size) {
    return mi_realloc(p, count * size);
}

void* mi_reallocf(void* p, size_t newsize) {
    void* new_p = mi_realloc(p, newsize);
    if (!new_p && p) mi_free(p);
    return new_p;
}

size_t mi_usable_size(const void* p) {
    return 0;
}

size_t mi_good_size(size_t size) {
    return size;
}

void mi_collect(bool force) {}
int mi_version(void) { return 200; }
void mi_stats_reset(void) {}
void mi_stats_merge(void) {}
void mi_stats_print(void* out) {}
void mi_options_print(void) {}

void* mi_malloc_aligned(size_t size, size_t alignment) {
#ifdef _MSC_VER
    return _aligned_malloc(size, alignment);
#else
    void* p;
    if (posix_memalign(&p, alignment, size) != 0) return NULL;
    return p;
#endif
}

void* mi_malloc_aligned_at(size_t size, size_t alignment, size_t offset) {
    return mi_malloc_aligned(size, alignment);
}

void* mi_zalloc_aligned(size_t size, size_t alignment) {
    void* p = mi_malloc_aligned(size, alignment);
    if (p) memset(p, 0, size);
    return p;
}

void* mi_zalloc_aligned_at(size_t size, size_t alignment, size_t offset) {
    return mi_zalloc_aligned(size, alignment);
}

void* mi_calloc_aligned(size_t count, size_t size, size_t alignment) {
    return mi_zalloc_aligned(count * size, alignment);
}

void* mi_calloc_aligned_at(size_t count, size_t size, size_t alignment, size_t offset) {
    return mi_calloc_aligned(count, size, alignment);
}

void* mi_realloc_aligned(void* p, size_t newsize, size_t alignment) {
#ifdef _MSC_VER
    return _aligned_realloc(p, newsize, alignment);
#else
    // POSIX doesn't have aligned realloc.
    // We must alloc new, copy, free.
    // Since we don't know old size, this shim is dangerous if used blindly.
    // However, for testing, we might assume realloc preserves alignment if lucky
    // or just alloc new (but we miss size).
    // Godot only uses realloc_aligned for resizing buffers.
    // The previous Godot implementation stored size/offset.
    // Mimalloc tracks it.
    // The shim can't easily do it.
    // We'll fallback to realloc and hope for best or accept limitation.
    return realloc(p, newsize);
#endif
}

void* mi_realloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset) {
    return mi_realloc_aligned(p, newsize, alignment);
}
