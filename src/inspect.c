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
#include <stdint.h>
#include <stdio.h>
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


void
hwm_buffer_fprint(FILE *stream, hwm_buffer_t *hwm)
{
    const uint8_t  *data = hwm_buffer_mem(hwm, uint8_t);
    size_t  i;

    for (i = 0; i < hwm->current_size; i++)
    {
        /*
         * The beginning of each line (except for the first) is
         * preceded by a newline.  The beginning of each line
         * (including the first) begins with a space.
         */

        if ((i % 16) == 0)
        {
            if (i > 0)
                fprintf(stream, "\n ");
            else
                fprintf(stream, " ");
        }

        /*
         * In between the eighth and ninth bytes of each line, there
         * are two additional spaces.
         */

        if ((i % 16) == 8)
            fprintf(stream, "  ");

        /*
         * And now, finally, the current byte.
         */

        fprintf(stream, " %02x", data[i]);
    }

    /*
     * If we printed out any bytes, the final line should end in a
     * newline.
     */

    if (hwm->current_size > 0)
        fprintf(stderr, "\n");
}
