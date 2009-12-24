/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <hwm-buffer.h>


void
hwm_buffer_init(hwm_buffer_t *hwm)
{
    /*
     * We don't actually allocate the buffer until we first try to
     * copy data into it.
     */

    hwm->allocated_size = 0;
    hwm->current_size = 0;
    hwm->allocation_count = 0;
    hwm->buf = NULL;
}


hwm_buffer_t *
hwm_buffer_new()
{
    hwm_buffer_t  *result = NULL;

    /*
     * Try to allocate a new buffer.
     */

    result = (hwm_buffer_t *) malloc(sizeof(hwm_buffer_t));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the buffer.
     */

    hwm_buffer_init(result);
    return result;
}


void
hwm_buffer_done(hwm_buffer_t *hwm)
{
    /*
     * Free the internal buffer, if there is one.
     */

    if (hwm->buf != NULL)
        free(hwm->buf);

    /*
     * Reset the fields to zero.
     */

    hwm->allocated_size = 0;
    hwm->current_size = 0;
    hwm->allocation_count = 0;
    hwm->buf = NULL;
}


void
hwm_buffer_free(hwm_buffer_t *hwm)
{
    hwm_buffer_done(hwm);
    free(hwm);
}


bool
hwm_buffer_is_empty(hwm_buffer_t *hwm)
{
    return (hwm->buf == NULL) || (hwm->current_size == 0);
}


bool
hwm_buffer_ensure_size(hwm_buffer_t *hwm, size_t size)
{
    if (hwm->buf == NULL)
    {
        /*
         * If we haven't allocated any buffer yet, we need to use
         * malloc.
         */

        hwm->buf = malloc(size);
        hwm->allocated_size = size;
        hwm->allocation_count++;

    } else {
        /*
         * Otherwise, we need to use realloc — but only if the
         * allocated_size isn't already big enough.
         */

        if (hwm->allocated_size < size)
        {
            hwm->buf = realloc(hwm->buf, size);
            hwm->allocated_size = size;
            hwm->allocation_count++;
        }
    }

    /*
     * Return whether or not the allocation was successful.  (If we
     * already had enough space, that counts as successful.)
     */

    return (hwm->buf != NULL);
}


bool
hwm_buffer_load_mem(hwm_buffer_t *hwm, const void *src, size_t size)
{
    /*
     * First, make sure we've allocated enough space, returning an
     * error code if we can't.
     */

    if (!hwm_buffer_ensure_size(hwm, size))
    {
        return false;
    }

    /*
     * Once we've got the space, copy the data over.
     */

    memcpy(hwm->buf, src, size);
    hwm->current_size = size;
    return true;
}


bool
hwm_buffer_append_mem(hwm_buffer_t *hwm, const void *src, size_t size)
{
    size_t  new_size;

    /*
     * First, figure out how much total space we need for the old and
     * new data.
     */

    new_size = hwm->current_size + size;

    /*
     * Make sure we've allocated enough space, returning an error code
     * if we can't.
     */

    if (!hwm_buffer_ensure_size(hwm, new_size))
    {
        return false;
    }

    /*
     * Once we've got the space, copy the data over.
     */

    memcpy(hwm->buf + hwm->current_size, src, size);
    hwm->current_size += size;
    return true;
}


void
hwm_buffer_unload_mem(hwm_buffer_t *hwm, void *dest, size_t max_size)
{
    /*
     * Make sure we don't copy more than max_size bytes.
     */

    size_t  size =
        (hwm->current_size > max_size)?
        max_size:
        hwm->current_size;

    memcpy(dest, hwm->buf, size);
}


bool
hwm_buffer_load_str(hwm_buffer_t *hwm, const char *src)
{
    size_t  size;

    /*
     * First, calculate the length of the string we're about the load.
     * Add an extra byte to hold the NUL terminator.
     */

    size = strlen(src) + 1;

    /*
     * Make sure we've allocated enough space, returning an error code
     * if we can't.
     */

    if (!hwm_buffer_ensure_size(hwm, size))
    {
        return false;
    }

    /*
     * Once we've got the space, copy the data over.  Note that
     * current_size includes the byte used to store the NUL
     * terminator.
     */

    memcpy(hwm->buf, src, size);
    hwm->current_size = size;
    return true;
}


bool
hwm_buffer_append_str(hwm_buffer_t *hwm, const char *src)
{
    size_t  size;
    size_t  modified_current_size;
    size_t  new_size;

    /*
     * First, calculate the length of the string we're about the load.
     * Add an extra byte to hold the NUL terminator.
     */

    size = strlen(src) + 1;

    /*
     * Next, determine the length of the current buffer that we keep.
     * In most cases, this is current_size - 1, since we want to
     * overwrite any existing NUL terminator.  If the buffer is empty,
     * though, there's no NUL terminator to overwrite.
     */

    modified_current_size =
        (hwm->current_size == 0)?
        0:
        hwm->current_size - 1;

    /*
     * Figure out how much total space we need for the old and new
     * data.
     */

    new_size = modified_current_size + size;

    /*
     * Make sure we've allocated enough space, returning an error code
     * if we can't.
     */

    if (!hwm_buffer_ensure_size(hwm, new_size))
    {
        return false;
    }

    /*
     * Once we've got the space, copy the data over.  Note that
     * current_size includes the byte used to store the NUL
     * terminator.
     */

    memcpy(hwm->buf + modified_current_size, src, size);
    hwm->current_size = modified_current_size + size;
    return true;
}


bool
hwm_buffer_load_buf(hwm_buffer_t *hwm, const hwm_buffer_t *src)
{
    return hwm_buffer_load_mem(hwm, src->buf, src->current_size);
}
