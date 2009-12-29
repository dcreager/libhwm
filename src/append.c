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
#include <string.h>

#include <hwm-buffer.h>


/**
 * A helper method for the append family of functions.  Ensures that
 * the buffer is at least as large as new_size.  In addition, if the
 * buffer currently points at some other piece of memory, rather than
 * at its own internal buffer, we copy that data into the internal
 * buffer.  The caller is responsible for ensuring that <code>new_size
 * >= hwm->current_size</code>.
 */

static bool
grow_and_copy(hwm_buffer_t *hwm, size_t new_size)
{
    /*
     * Make sure we've got enough space in the internal buffer to copy
     * the data over.
     */

    if (!hwm_buffer_ensure_size(hwm, new_size))
        return false;

    /*
     * If we're currently pointing at some outside memory, we need to
     * copy it into our buffer first.
     */

    if (hwm->data != hwm->buf)
    {
        if (hwm->current_size > 0)
            memcpy(hwm->buf, hwm->data, hwm->current_size);

        hwm->data = hwm->buf;
    }

    /*
     * At this point, we know the data is stored in our internal
     * buffer, so it's safe to modify.
     */

    return true;
}


void *
_hwm_buffer_writable_mem(hwm_buffer_t *hwm)
{
    if (grow_and_copy(hwm, hwm->current_size))
        return hwm->buf;
    else
        return NULL;
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
     * Make sure we've allocated enough space and that we're pointing
     * at the internal buffer, returning an error code if we can't.
     */

    if (!grow_and_copy(hwm, new_size))
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
     * Next, determine how much of the current buffer that we need to
     * keep.  In most cases, this is current_size - 1, since we want
     * to overwrite any existing NUL terminator.  If the buffer is
     * empty, though, there's no NUL terminator to overwrite.
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
     * Make sure we've allocated enough space and that we're pointing
     * at the internal buffer, returning an error code if we can't.
     */

    if (!grow_and_copy(hwm, new_size))
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


void *
_hwm_buffer_append_list_elem(hwm_buffer_t *hwm, size_t elem_size)
{
    /*
     * First figure out how many elements are currently in the list.
     * We can't use hwm_buffer_current_list_size, since that expects
     * the element type, and we have the size of the element type.
     */

    size_t  current_list_size =
        hwm->current_size / elem_size;

    /*
     * Next figure out how big the new list should be.
     */

    size_t  new_size =
        (current_list_size + 1) * elem_size;

    /*
     * Make sure we've allocated enough space and that we're pointing
     * at the internal buffer, returning an error code if we can't.
     */

    if (!grow_and_copy(hwm, new_size))
    {
        return NULL;
    }

    /*
     * If that worked, increment the current size, return a pointer to
     * the new element.  (current_list_size is the size the list was
     * before we appended a new element.)
     */

    hwm->current_size = new_size;
    return (hwm->buf + (current_list_size * elem_size));
}
