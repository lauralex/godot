/**************************************************************************/
/*  memory_wrapper.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#pragma once

// Include the mimalloc API
#include "thirdparty/mimalloc/mimalloc.h"

// Undefine the default macros from core/os/memory.h if they exist
#ifdef memalloc
#undef memalloc
#endif

#ifdef memrealloc
#undef memrealloc
#endif

#ifdef memfree
#undef memfree
#endif

#ifdef memalloc_zeroed
#undef memalloc_zeroed
#endif

// Map to mimalloc functions
#define memalloc(m_size) mi_malloc(m_size)
#define memrealloc(m_mem, m_size) mi_realloc(m_mem, m_size)
#define memfree(m_mem) mi_free(m_mem)
#define memalloc_zeroed(m_size) mi_zalloc(m_size)
