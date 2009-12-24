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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <check.h>

#include <hwm-buffer.h>


/*-----------------------------------------------------------------------
 * Sample data
 */

const char  *DATA_01 = "0123456789";
size_t  LENGTH_01 = 10;

const char  *DATA_02 = "01234567890123456789";
size_t  LENGTH_02 = 20;


/*-----------------------------------------------------------------------
 * Helper functions
 */

#define fail_unless_buf_matches(buffer, other, size)            \
    {                                                           \
        fail_if(hwm_buffer_data(buffer, void) == NULL,          \
                "Data doesn't match: buffer unallocated");      \
        fail_unless((buffer)->current_size == size,             \
                    "Data doesn't match: wrong size (%zu)",     \
                    (buffer)->current_size);                    \
        fail_unless(memcmp(hwm_buffer_data(buffer, void),       \
                           other, size) == 0,                   \
                    "Data doesn't match: different contents");  \
    }

#define fail_unless_memeq(src, dest, size)              \
    {                                                   \
        if (memcmp(src, dest, size) != 0)               \
        {                                               \
            char pretty[((size) * 3) + 1];              \
            char *ptr;                                  \
            int  i;                                     \
                                                        \
            ptr = pretty;                               \
            for (i = 0; i < (size); i++)                \
            {                                           \
                snprintf(ptr, 4, " %02x", (src)[i]);    \
                ptr += 3;                               \
            }                                           \
                                                        \
            fail("Data doesn't match:%s", pretty);      \
        }                                               \
    }


/*-----------------------------------------------------------------------
 * Test cases
 */


START_TEST(test_starts_empty)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_is_empty(&buf),
                "HWM buffer should start empty");
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_alloc_fail)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_if(hwm_buffer_ensure_size(&buf, (size_t) -1),
            "Didn't receive error code when allocating -1 bytes");
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_realloc_fail)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_ensure_size(&buf, 10),
                "Couldn't allocate 10 bytes");
    fail_if(hwm_buffer_ensure_size(&buf, (size_t) -1),
            "Didn't receive error code when reallocating -1 bytes");
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_mem_01)
{
    hwm_buffer_t  buf;

    /*
     * This test should require one allocation.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 1,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_mem_02)
{
    hwm_buffer_t  buf;

    /*
     * This test should require two allocations.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(hwm_buffer_load_mem(&buf, DATA_02, LENGTH_02),
                "Cannot load HWM buffer");
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    fail_unless(buf.allocation_count == 2,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 2);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_mem_03)
{
    hwm_buffer_t  buf;

    /*
     * This test should require one allocation.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 1,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_allocated_size_01)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(buf.allocated_size == LENGTH_01,
                "Buffer didn't allocate the right amount memory "
                "(got %zu bytes, expected %zu)",
                buf.allocated_size, LENGTH_01);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_allocated_size_02)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(hwm_buffer_load_mem(&buf, DATA_02, LENGTH_02),
                "Cannot load HWM buffer");
    fail_unless(buf.allocated_size == LENGTH_02,
                "Buffer didn't allocate the right amount memory "
                "(got %zu bytes, expected %zu)",
                buf.allocated_size, LENGTH_02);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_allocated_size_03)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless(buf.allocated_size == LENGTH_01,
                "Buffer didn't allocate the right amount memory "
                "(got %zu bytes, expected %zu)",
                buf.allocated_size, LENGTH_01);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_unload_mem_01)
{
    hwm_buffer_t  buf;
    char  dest[LENGTH_01];

    /*
     * This test relies on the fact that DATA_01 is a prefix of
     * DATA_02.  It tests the clipping behavior of
     * hwm_buffer_unload_mem.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_02, LENGTH_02),
                "Cannot load HWM buffer");
    hwm_buffer_unload_mem(&buf, dest, LENGTH_01);
    fail_unless_memeq(dest, DATA_01, LENGTH_01);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_mem_01)
{
    hwm_buffer_t  buf;

    /*
     * Appending to an empty buffer should be the same as loading.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_mem_02)
{
    hwm_buffer_t  buf;

    /*
     * This test relies on the fact that DATA_02 is two copies of
     * DATA_01.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_str_01)
{
    hwm_buffer_t  buf;

    /*
     * This test should require one allocation.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_str(&buf, DATA_01),
                "Cannot load HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_str_02)
{
    hwm_buffer_t  buf;

    /*
     * This test should require two allocations.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_str(&buf, DATA_01),
                "Cannot load HWM buffer");
    fail_unless(hwm_buffer_load_str(&buf, DATA_02),
                "Cannot load HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_str_01)
{
    hwm_buffer_t  buf;

    /*
     * Appending to an empty buffer should be the same as loading.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_append_str(&buf, DATA_01),
                "Cannot append HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_str_02)
{
    hwm_buffer_t  buf;

    /*
     * This test relies on the fact that DATA_02 is two copies of
     * DATA_01.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_append_str(&buf, DATA_01),
                "Cannot append HWM buffer");
    fail_unless(hwm_buffer_append_str(&buf, DATA_01),
                "Cannot append HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_load_buf_01)
{
    hwm_buffer_t  buf1;
    hwm_buffer_t  buf2;

    hwm_buffer_init(&buf1);
    fail_unless(hwm_buffer_load_mem(&buf1, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");

    hwm_buffer_init(&buf2);
    fail_unless(hwm_buffer_load_buf(&buf2, &buf1),
                "Cannot copy buffer");

    fail_unless_buf_matches(&buf2, DATA_01, LENGTH_01);

    hwm_buffer_done(&buf1);
    hwm_buffer_done(&buf2);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("hwm-buffer");

    TCase  *tc = tcase_create("hwm-buffer");
    tcase_add_test(tc, test_starts_empty);
    tcase_add_test(tc, test_alloc_fail);
    tcase_add_test(tc, test_realloc_fail);
    tcase_add_test(tc, test_load_mem_01);
    tcase_add_test(tc, test_load_mem_02);
    tcase_add_test(tc, test_load_mem_03);
    tcase_add_test(tc, test_allocated_size_01);
    tcase_add_test(tc, test_allocated_size_02);
    tcase_add_test(tc, test_allocated_size_03);
    tcase_add_test(tc, test_unload_mem_01);
    tcase_add_test(tc, test_append_mem_01);
    tcase_add_test(tc, test_append_mem_02);
    tcase_add_test(tc, test_load_str_01);
    tcase_add_test(tc, test_load_str_02);
    tcase_add_test(tc, test_append_str_01);
    tcase_add_test(tc, test_append_str_02);
    tcase_add_test(tc, test_load_buf_01);
    suite_add_tcase(s, tc);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
