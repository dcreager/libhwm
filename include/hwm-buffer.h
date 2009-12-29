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
#include <stdio.h>

/**
 * @mainpage High-water mark buffers
 *
 * Please see the hwm-buffer.h file for details about the available
 * functions in this library.
 *
 * @section normal Normal operation
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
 * @section string String functions
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
 * @section pointing Pointing to other memory
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
 *
 * @section lists List functions
 *
 * It's also possible to use an HWM buffer as an expandable array,
 * much like the Java <code>ArrayList</code> class.  The list macros
 * each take in an additional parameter, which is the type of the list
 * elements.  This allows you to specify the size of the list in terms
 * of a number of elements.
 */

/**
 * @file
 *
 * This file provides a “high-water mark” buffer.  This allows you to
 * easily work with data records that contain variable-sized fields.
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
     *
     * @private
     */
    size_t  allocated_size;

    /**
     * The size of the current data item in buf.  If the buffer has
     * been populated using the “load” or “append” functions, this
     * must be <= allocated_size.
     *
     * @private
     */

    size_t  current_size;

    /**
     * The number of times the data buffer has been allocated or
     * reallocated.
     *
     * @private
     */

    unsigned int  allocation_count;

    /**
     * The pointer to the data contained in the buffer.  When the
     * buffer has been populated using the “point” functions, this is
     * a pointer to memory that isn't controlled by the buffer.  When
     * the buffer has been populated using the “load” or “append”
     * functions, this is a pointer to the memory that we control.
     *
     * @private
     */

    const void  *data;

    /**
     * The memory region that the buffer controls.
     *
     * @private
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

/**
 * Does the actual work for hwm_buffer_writable_mem().  This function
 * returns a <code>void *</code>; the hwm_buffer_writable_mem() macro
 * casts to the desired return type.
 *
 * @private
 */

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
 * Return a non-writable pointer to the specified element of the list
 * stored in the buffer.  You must specify the type of the contained
 * data, in addition to the index of the element that you want.
 */

#define hwm_buffer_list_elem(hwm, type, elem) \
    (((const type *) (hwm)->data) + (elem))


/**
 * Return a writable pointer to the specified element of the list
 * stored in the buffer.  You must specify the type of the contained
 * data, in addition to the index of the element that you want.  If
 * the buffer is currently pointing at some other memory region, then
 * it will first be copied into the buffer so that it's safe to
 * modify.  If we need to copy the data, but can't, we return NULL.
 */

#define hwm_buffer_writable_list_elem(hwm, type, elem)    \
    (((type *) _hwm_buffer_writable_mem(hwm)) + (elem))


/**
 * Return the number of list elements that are currently contained in
 * the buffer.
 */

#define hwm_buffer_current_list_size(hwm, type) \
    ((hwm)->current_size / sizeof(type))


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
 * Ensure that the HWM buffer has enough allocated space to store the
 * given number of elements of the specified type.  If we can't
 * allocate enough space, return false.  Otherwise, return true.
 */

#define hwm_buffer_ensure_list_size(hwm, type, elems) \
    (hwm_buffer_ensure_size((hwm), sizeof(type) * (elems)))


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


/**
 * Append one list element to the buffer, returning a pointer to it.
 * If we need to expand the buffer, but can't, we return NULL.
 */

#define hwm_buffer_append_list_elem(hwm, type)  \
    ((type *) _hwm_buffer_append_list_elem(hwm, sizeof(type)))

/**
 * Does the actual work for hwm_buffer_append_list_elem().  This
 * function returns a <code>void *</code>; the
 * hwm_buffer_append_list_elem() macro casts to the desired return
 * type.
 *
 * @private
 */

void *
_hwm_buffer_append_list_elem(hwm_buffer_t *hwm, size_t elem_size);


/**
 * Print the contents of the buffer to the specified stream.  The data
 * is printed in the following format:
 *
 * <pre>
 *   00 01 02 03 04 05 06 07   08 09 0a 0b 0c 0d 0e 0f
 *     <i>etc.</i></pre>
 */

void
hwm_buffer_fprint(FILE *stream, hwm_buffer_t *hwm);


#endif /* HWM_BUFFER_H */
