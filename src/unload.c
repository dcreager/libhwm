/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <hwm-buffer.h>


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

    memcpy(dest, hwm->data, size);
}
