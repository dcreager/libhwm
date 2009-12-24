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
     * If we're currently pointing at some outside memory, we need to
     * copy it into our buffer first.
     */

    if ((hwm->current_size > 0) && (hwm->data != hwm->buf))
    {
        memcpy(hwm->buf, hwm->data, hwm->current_size);
    }

    /*
     * Once we've got the space, copy the data over.
     */

    memcpy(hwm->buf + hwm->current_size, src, size);
    hwm->data = hwm->buf;
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
     * If we're currently pointing at some outside memory, we need to
     * copy it into our buffer first.
     */

    if ((modified_current_size > 0) && (hwm->data != hwm->buf))
    {
        memcpy(hwm->buf, hwm->data, modified_current_size);
    }

    /*
     * Once we've got the space, copy the data over.  Note that
     * current_size includes the byte used to store the NUL
     * terminator.
     */

    memcpy(hwm->buf + modified_current_size, src, size);
    hwm->data = hwm->buf;
    hwm->current_size = modified_current_size + size;
    return true;
}
