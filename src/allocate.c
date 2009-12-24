/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

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
    hwm->data = NULL;
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
    hwm->data = NULL;
    hwm->buf = NULL;
}


void
hwm_buffer_free(hwm_buffer_t *hwm)
{
    hwm_buffer_done(hwm);
    free(hwm);
}
