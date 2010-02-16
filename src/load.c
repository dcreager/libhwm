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
            /*
             * realloc might change the hwm->buf pointer.  The current
             * data lives in our memory region, then hwm->data will
             * equal hwm->buf, and we'll have to update it as well.
             * If the current data is outside our memory region, we
             * should *not* update hwm->data.
             */

            void  *old_buf = hwm->buf;

            hwm->buf = realloc(hwm->buf, size);
            hwm->allocated_size = size;
            hwm->allocation_count++;

            if (hwm->data == old_buf)
                hwm->data = hwm->buf;
        }
    }

    /*
     * Return whether or not the allocation was successful.  (If we
     * already had enough space, that counts as successful.)
     */

    return (hwm->buf != NULL);
}


bool
hwm_buffer_clear(hwm_buffer_t *hwm)
{
    /*
     * Reset the size counter to 0, and make sure that our data
     * pointer is pointing at the local buffer.
     */

    hwm->data = hwm->buf;
    hwm->current_size = 0;

    return true;
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
    hwm->data = hwm->buf;
    hwm->current_size = size;
    return true;
}


void
hwm_buffer_point_at_mem(hwm_buffer_t *hwm, const void *src, size_t size)
{
    hwm->data = src;
    hwm->current_size = size;
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
    hwm->data = hwm->buf;
    hwm->current_size = size;
    return true;
}


void
hwm_buffer_point_at_str(hwm_buffer_t *hwm, const char *src)
{
    hwm->data = src;
    hwm->current_size = strlen(src) + 1;
}


bool
hwm_buffer_load_buf(hwm_buffer_t *hwm, const hwm_buffer_t *src)
{
    return hwm_buffer_load_mem(hwm, src->buf, src->current_size);
}
