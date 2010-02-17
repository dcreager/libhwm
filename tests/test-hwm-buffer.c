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

const char  *DATA_EMPTY_01 = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
size_t  LENGTH_EMPTY_01 = 10;


/*-----------------------------------------------------------------------
 * Helper functions
 */

#define fail_unless_buf_matches(buffer, other, size)            \
    {                                                           \
        fail_if(hwm_buffer_mem(buffer, void) == NULL,           \
                "Data doesn't match: buffer unallocated");      \
        fail_unless((buffer)->current_size == size,             \
                    "Data doesn't match: wrong size (%zu)",     \
                    (buffer)->current_size);                    \
                                                                \
        if (memcmp(hwm_buffer_mem(buffer, void),                \
                   other, size) != 0)                           \
        {                                                       \
            hwm_buffer_t  __expected;                           \
            hwm_buffer_init(&__expected);                       \
            hwm_buffer_point_at_mem(&__expected, other, size);  \
                                                                \
            fprintf(stderr, "Actual:\n");                       \
            hwm_buffer_fprint(stderr, buffer);                  \
            fprintf(stderr, "Expected:\n");                     \
            hwm_buffer_fprint(stderr, &__expected);             \
                                                                \
            hwm_buffer_done(&__expected);                       \
            fail("Data doesn't match: different contents");     \
        }                                                       \
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


START_TEST(test_point_at_mem_01)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_point_at_mem_02)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    hwm_buffer_point_at_mem(&buf, DATA_02, LENGTH_02);
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_point_at_mem_03)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_static_init_mem_01)
{
    hwm_buffer_t  buf = HWM_BUFFER_INIT(DATA_01, LENGTH_01);

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_static_init_mem_02)
{
    hwm_buffer_t  buf = HWM_BUFFER_INIT(DATA_01, LENGTH_01);

    hwm_buffer_point_at_mem(&buf, DATA_02, LENGTH_02);
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_static_init_mem_03)
{
    hwm_buffer_t  buf = HWM_BUFFER_INIT(DATA_01, LENGTH_01);

    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(buf.allocation_count == 0,
                "Didn't allocate the right number of times "
                "(got %u, expected %u)",
                buf.allocation_count, 0);
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


START_TEST(test_point_at_append_mem_01)
{
    hwm_buffer_t  buf;

    /*
     * We want to test that if we point at some memory, and then
     * append to it, that the end result is that we've got the two
     * pieces concatenated together.  We're first going to fill the
     * buffer with some other piece of memory — if that's in the
     * end-result buffer, then the concatenation didn't work.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_EMPTY_01, LENGTH_EMPTY_01),
                "Cannot load HWM buffer");
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);
    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_static_init_append_mem_01)
{
    hwm_buffer_t  buf = HWM_BUFFER_INIT(DATA_01, LENGTH_01);

    /*
     * We want to test that if we point at some memory, and then
     * append to it, that the end result is that we've got the two
     * pieces concatenated together.
     */

    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_clear_append_mem_01)
{
    hwm_buffer_t  buf;

    /*
     * Appending to an empty buffer should be the same as loading,
     * even if the buffer is empty because we just cleared it.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_01, LENGTH_01),
                "Cannot load HWM buffer");
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
    fail_unless(hwm_buffer_clear(&buf),
                "Cannot clear HWM buffer");
    fail_unless(hwm_buffer_append_mem(&buf, DATA_01, LENGTH_01),
                "Cannot append HWM buffer");
    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01);
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


START_TEST(test_point_at_str_01)
{
    hwm_buffer_t  buf;


    hwm_buffer_init(&buf);
    hwm_buffer_point_at_str(&buf, DATA_01);

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_point_at_str_02)
{
    hwm_buffer_t  buf;

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_str(&buf, DATA_01);
    hwm_buffer_point_at_str(&buf, DATA_02);

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


START_TEST(test_point_at_append_str_01)
{
    hwm_buffer_t  buf;

    /*
     * We want to test that if we point at some memory, and then
     * append to it, that the end result is that we've got the two
     * pieces concatenated together.  We're first going to fill the
     * buffer with some other piece of memory — if that's in the
     * end-result buffer, then the concatenation didn't work.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_mem(&buf, DATA_EMPTY_01, LENGTH_EMPTY_01),
                "Cannot load HWM buffer");
    hwm_buffer_point_at_str(&buf, DATA_01);
    fail_unless(hwm_buffer_append_str(&buf, DATA_01),
                "Cannot append HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_02, LENGTH_02+1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_clear_append_str_01)
{
    hwm_buffer_t  buf;

    /*
     * Appending to an empty buffer should be the same as loading,
     * even if the buffer is empty because we just cleared it.
     */

    hwm_buffer_init(&buf);
    fail_unless(hwm_buffer_load_str(&buf, DATA_01),
                "Cannot load HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01+1);

    fail_unless(hwm_buffer_clear(&buf),
                "Cannot clear HWM buffer");
    fail_unless(hwm_buffer_append_str(&buf, DATA_01),
                "Cannot append HWM buffer");

    /*
     * We have to include an extra byte for the NUL terminator.
     */

    fail_unless_buf_matches(&buf, DATA_01, LENGTH_01+1);
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


START_TEST(test_writable_mem_01)
{
    hwm_buffer_t  buf;
    uint8_t  *mem;

    /*
     * Point at our data string, then ask for a writable pointer.  If
     * we modify one of the characters in that writable pointer, we'd
     * better not have modified the original data string.
     */

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_mem(&buf, DATA_01, LENGTH_01);

    mem = hwm_buffer_writable_mem(&buf, uint8_t);
    fail_if(mem == NULL,
            "Cannot get writable pointer");

    mem[0] = 'Q';
    fail_if(DATA_01[0] == 'Q',
            "Changing writable pointer shouldn't change original data");

    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_writable_str_01)
{
    hwm_buffer_t  buf;
    char  *str;

    /*
     * Point at our data string, then ask for a writable pointer.  If
     * we modify one of the characters in that writable pointer, we'd
     * better not have modified the original data string.
     */

    hwm_buffer_init(&buf);
    hwm_buffer_point_at_str(&buf, DATA_01);

    str = hwm_buffer_writable_str(&buf);
    fail_if(str == NULL,
            "Cannot get writable pointer");

    str[0] = 'Q';
    fail_if(DATA_01[0] == 'Q',
            "Changing writable pointer shouldn't change original data");

    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_list_size_01)
{
    hwm_buffer_t  buf;
    unsigned int  *elem;

    hwm_buffer_init(&buf);
    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 1;

    fail_unless(hwm_buffer_current_list_size(&buf, unsigned int) == 1,
                "List is wrong size (got %zu, expected %zu)",
                hwm_buffer_current_list_size(&buf, unsigned int), 1);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_list_size_02)
{
    hwm_buffer_t  buf;
    unsigned int  *elem;

    hwm_buffer_init(&buf);

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 1;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 2;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 3;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 4;

    fail_unless(hwm_buffer_current_list_size(&buf, unsigned int) == 4,
                "List is wrong size (got %zu, expected %zu)",
                hwm_buffer_current_list_size(&buf, unsigned int), 4);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_list_01)
{
    hwm_buffer_t  buf;
    unsigned int  *elem;
    unsigned int  expected[] = { 1,2,3,4 };
    size_t  expected_size = 4 * sizeof(unsigned int);

    hwm_buffer_init(&buf);

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 1;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 2;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 3;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 4;

    fail_unless_buf_matches(&buf, expected, expected_size);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_append_list_02)
{
    hwm_buffer_t  buf;
    unsigned int  *elem;
    unsigned int  expected[] = { 1 };
    size_t  expected_size = 1 * sizeof(unsigned int);

    hwm_buffer_init(&buf);
    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 1;

    fail_unless_buf_matches(&buf, expected, expected_size);
    hwm_buffer_done(&buf);
}
END_TEST


START_TEST(test_ensure_list_size_01)
{
    hwm_buffer_t  buf;
    unsigned int  *elem;

    hwm_buffer_init(&buf);

    fail_unless(hwm_buffer_ensure_list_size(&buf, unsigned int, 2),
                "Cannot grow list");

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 1;

    elem = hwm_buffer_append_list_elem(&buf, unsigned int);
    fail_if(elem == NULL,
            "Cannot append HWM list");
    *elem = 2;

    fail_unless(hwm_buffer_current_list_size(&buf, unsigned int) == 2,
                "List is wrong size (got %zu, expected %zu)",
                hwm_buffer_current_list_size(&buf, unsigned int), 2);
    hwm_buffer_done(&buf);
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
    tcase_add_test(tc, test_load_mem_01);
    tcase_add_test(tc, test_load_mem_02);
    tcase_add_test(tc, test_load_mem_03);
    tcase_add_test(tc, test_allocated_size_01);
    tcase_add_test(tc, test_allocated_size_02);
    tcase_add_test(tc, test_allocated_size_03);
    tcase_add_test(tc, test_point_at_mem_01);
    tcase_add_test(tc, test_point_at_mem_02);
    tcase_add_test(tc, test_point_at_mem_03);
    tcase_add_test(tc, test_static_init_mem_01);
    tcase_add_test(tc, test_static_init_mem_02);
    tcase_add_test(tc, test_static_init_mem_03);
    tcase_add_test(tc, test_unload_mem_01);
    tcase_add_test(tc, test_append_mem_01);
    tcase_add_test(tc, test_append_mem_02);
    tcase_add_test(tc, test_point_at_append_mem_01);
    tcase_add_test(tc, test_static_init_append_mem_01);
    tcase_add_test(tc, test_clear_append_mem_01);
    tcase_add_test(tc, test_load_str_01);
    tcase_add_test(tc, test_load_str_02);
    tcase_add_test(tc, test_point_at_str_01);
    tcase_add_test(tc, test_point_at_str_02);
    tcase_add_test(tc, test_append_str_01);
    tcase_add_test(tc, test_append_str_02);
    tcase_add_test(tc, test_point_at_append_str_01);
    tcase_add_test(tc, test_clear_append_str_01);
    tcase_add_test(tc, test_load_buf_01);
    tcase_add_test(tc, test_writable_mem_01);
    tcase_add_test(tc, test_writable_str_01);
    tcase_add_test(tc, test_append_list_size_01);
    tcase_add_test(tc, test_append_list_size_02);
    tcase_add_test(tc, test_append_list_01);
    tcase_add_test(tc, test_append_list_02);
    tcase_add_test(tc, test_ensure_list_size_01);
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
