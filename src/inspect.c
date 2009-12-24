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


void *
_hwm_buffer_writable_mem(hwm_buffer_t *hwm)
{
    /*
     * If we're currently pointing at some outside memory, we need to
     * copy it into our buffer first.
     */

    if ((hwm->current_size > 0) && (hwm->data != hwm->buf))
    {
        /*
         * Make sure we've got enough space in the internal buffer to
         * copy the data over.
         */

        if (!hwm_buffer_ensure_size(hwm, hwm->current_size))
            return NULL;

        /*
         * And then copy the data over.
         */

        memcpy(hwm->buf, hwm->data, hwm->current_size);
        hwm->data = hwm->buf;
    }

    /*
     * At this point, we know the data is stored in our internal
     * buffer, so it's safe to modify.
     */

    return hwm->buf;
}


bool
hwm_buffer_is_empty(hwm_buffer_t *hwm)
{
    return (hwm->data == NULL) || (hwm->current_size == 0);
}
