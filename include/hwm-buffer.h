/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef HWM_BUFFER_H
#define HWM_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @file This file provides a “high-water mark” buffer.  This allows
 * you to easily work with data records that contain variable-sized
 * fields.
 */

typedef struct hwm_buffer
{
    /**
     * The allocated size of buf.  Its value is undefined if buf is
     * NULL.
     */
    size_t  allocated_size;

    /**
     * The size of the current data item in buf.  This might be less
     * than allocated_size.
     */

    size_t  current_size;

    /**
     * The actual data buffer.
     */

    void  *buf;
} hwm_buffer_t;


/**
 * Return the actual data pointer of an HWM buffer.
 */

#define hwm_buffer_data(hwm) ((const void *) (hwm)->buf)


/**
 * Return the actual data pointer of an HWM buffer, cast to (char *).
 */

#define hwm_buffer_str(hwm) ((const char *) (hwm)->buf)


/**
 * Initialize a new HWM buffer that has already been allocated (on the
 * stack, for instance).
 */

void
hwm_buffer_init(hwm_buffer_t *hwm);

/**
 * Finalize an HWM buffer.  Doesn't deallocate the buffer, so this is
 * safe to call on stack-allocated buffers.
 */

void
hwm_buffer_done(hwm_buffer_t *hwm);

/**
 * Create a new HWM buffer on the heap.  Return NULL if we can't
 * allocate a new instance.
 */

hwm_buffer_t *
hwm_buffer_new();

/**
 * Finalize and free a heap-allocated HWM buffer.
 */

void
hwm_buffer_free(hwm_buffer_t *hwm);

/**
 * Return whether the buffer is empty.
 */

bool
hwm_buffer_is_empty(hwm_buffer_t *hwm);

/**
 * Ensure that the HWM buffer has enough allocated space to store a
 * value of size bytes.  If we can't allocate enough space, return
 * false.  Otherwise, return true.
 */

bool
hwm_buffer_ensure_size(hwm_buffer_t *hwm, size_t size);

/**
 * Copy data into the HWM buffer from some other source.  You don't
 * need to call hwm_buffer_ensure_size() first; this function does
 * that itself.  If the copying fails for some reason, we return
 * false.
 */

bool
hwm_buffer_load_mem(hwm_buffer_t *hwm, const void *src, size_t size);

/**
 * Appends data into the HWM buffer from some other source.  You don't
 * need to call hwm_buffer_ensure_size() first; this function does
 * that itself.  If the copying fails for some reason, we return
 * false.
 */

bool
hwm_buffer_append_mem(hwm_buffer_t *hwm, const void *src, size_t size);

/**
 * Copy data out of the HWM buffer into some other destination.  We
 * will not copy more than max_size bytes — if there's more data in
 * the HWM buffer, it will be clipped.
 */

void
hwm_buffer_unload_mem(hwm_buffer_t *hwm, void *dest, size_t max_size);

/**
 * Copy data into the HWM buffer from a NUL-terminated string.  The
 * NUL terminator is included in the buffer.  You don't need to call
 * hwm_buffer_ensure_size() first; this function does that itself.  If
 * the copying fails for some reason, we return false.
 */

bool
hwm_buffer_load_str(hwm_buffer_t *hwm, const char *src);

/**
 * Appends data into the HWM buffer from a NUL-terminated string.  The
 * final byte of the original buffer is overwritten, since we assume
 * that the buffer originally contains a NUL-terminated string.  The
 * NUL terminator from the appended string is included in the buffer.
 * You don't need to call hwm_buffer_ensure_size() first; this
 * function does that itself.  If the copying fails for some reason,
 * we return false.
 */

bool
hwm_buffer_append_str(hwm_buffer_t *hwm, const char *src);

/**
 * Copy data into the HWM buffer from another HWM buffer.  You don't
 * need to call hwm_buffer_ensure_size() first; this function does
 * that itself.  If the copying fails for some reason, we return
 * false.
 */

bool
hwm_buffer_load_buf(hwm_buffer_t *hwm, const hwm_buffer_t *src);


#endif /* HWM_BUFFER_H */
