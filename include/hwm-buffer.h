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
 *
 * <h1>Normal operation</h1>
 *
 * In normal operation, the HWM buffer maintains its own buffer of
 * memory.  You can “load” data into the buffer, which copies the data
 * into the buffer from some other source.  In doing so, the HWM
 * functions will make sure that there is enough allocated memory to
 * store the data, reallocating the buffer larger if needed.
 *
 * You can also “append” data to the buffer; as its name implies, this
 * appends the copied data to the end of the buffer, keeping what was
 * already there.  We ensure that there's enough space for the new
 * appended data.
 *
 * You can “unload” data from the buffer, copying its contents into
 * some other memory region.  To do so, you must provide the size of
 * the destination region, so that we can make sure not to overflow in
 * the case that the HWM contains more data than that.
 *
 * <h1>String functions</h1>
 *
 * The functions that contain “str” in their name can be used to
 * handle NUL-terminated strings.  Loading a string into a buffer
 * means that you don't have to specify its length; this can be
 * calculated by looking for the NUL terminator.  When appending a
 * string, we assume that both the buffer and the source are
 * NUL-terminated strings; therefore, we overwrite the final character
 * (if any) of the buffer's original contents, on the assumption that
 * this is a NUL terminator.
 *
 * <h1>Pointing to other memory</h1>
 *
 * Loading data into a buffer incurs the cost of copying the data from
 * its source.  If you know that the buffer will only be used in a
 * read-only fashion, however, this is overkill.  There are also
 * “point” functions for this use case.  Instead of copying the data
 * into the buffer, these functions just have the buffer point at the
 * original copy of the data.  As long as you only call
 * hwm_buffer_data() and hwm_buffer_string() to access the data, no
 * copying will be performed.  If you perform any action that requires
 * modifying the data (which includes the “append” and “writable”
 * functions), the original data will be copied into memory that the
 * buffer controls.
 */


/**
 * A high-water mark buffer.  The fields of the struct are considered
 * private — you should not access them directly.  Instead, use one of
 * the accessor macros defined below.  The struct is fully defined in
 * this header file so that you can define one directly on the stack;
 * to allow this, the compiler needs to know the size of the struct,
 * which requires its full definition.
 */

typedef struct hwm_buffer
{
    /**
     * The allocated size of buf.  Its value is undefined if buf is
     * NULL.
     */
    size_t  allocated_size;

    /**
     * The size of the current data item in buf.  If the buffer has
     * been populated using the “load” or “append” functions, this
     * must be <= allocated_size.
     */

    size_t  current_size;

    /**
     * The number of times the data buffer has been allocated or
     * reallocated.
     */

    unsigned int  allocation_count;

    /**
     * The pointer to the data contained in the buffer.  When the
     * buffer has been populated using the “point” functions, this is
     * a pointer to memory that isn't controlled by the buffer.  When
     * the buffer has been populated using the “load” or “append”
     * functions, this is a pointer to the memory that we control.
     */

    const void  *data;

    /**
     * The memory region that the buffer controls.
     */

    void  *buf;
} hwm_buffer_t;


/**
 * Return a non-writable pointer to the data stored in the buffer.
 * You must specify the type of the contained data.
 */

#define hwm_buffer_mem(hwm, type) ((const type *) (hwm)->data)


/**
 * Return a writable pointer to the data stored in the buffer.  You
 * must specify the type of the contained data.  If the buffer is
 * currently pointing at some other memory region, then it will first
 * be copied into the buffer so that it's safe to modify.  If we need
 * to copy the data, but can't, we return NULL.
 */

#define hwm_buffer_writable_mem(hwm, type) \
    ((type *) _hwm_buffer_writable_mem(hwm))

void *
_hwm_buffer_writable_mem(hwm_buffer_t *hwm);


/**
 * Return a non-writeable pointer to the data stored in the buffer,
 * cast to <code>(const char *)</code>.
 */

#define hwm_buffer_str(hwm) hwm_buffer_mem(hwm, char)


/**
 * Return a writable pointer to the data stored in the buffer, cast to
 * <code>(char *)</code>.  You must specify the type of the contained
 * data.  If the buffer is currently pointing at some other memory
 * region, then it will first be copied into the buffer so that it's
 * safe to modify.  If we need to copy the data, but can't, we return
 * NULL.
 */

#define hwm_buffer_writable_str(hwm) hwm_buffer_writable_mem(hwm, char)


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
 * Point the HWM buffer at data in another region of memory.  This
 * doesn't involve allocating the buffer's own memory, so it cannot
 * fail.
 */

void
hwm_buffer_point_at_mem(hwm_buffer_t *hwm, const void *src, size_t size);


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
 * Point the HWM buffer at a NUL-terminated string in another region
 * of memory.  This doesn't involve allocating the buffer's own
 * memory, so it cannot fail.
 */

void
hwm_buffer_point_at_str(hwm_buffer_t *hwm, const char *src);


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
