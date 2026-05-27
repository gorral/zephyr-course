/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md.
 */

#include <errno.h>

#include <zephyr/ztest.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
ARG_UNUSED(f);
rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_init, test_fresh_state)
{
zassert_true(rb_is_empty(), "Fresh buffer must be empty");
zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
zassert_equal(rb_push(99), 0, "Push should succeed");
zassert_false(rb_is_empty(), "Buffer should not be empty after push");
zassert_equal(rb_count(), 1, "Buffer count should be 1 after push");

zassert_equal(rb_init(4), 0, "Reinit should succeed");
zassert_true(rb_is_empty(), "Reinit should clear state");
zassert_equal(rb_count(), 0, "Reinit should reset count");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
int value = 0;

zassert_equal(rb_push(42), 0, "Push should succeed");
zassert_equal(rb_pop(&value), 0, "Pop should succeed");
zassert_equal(value, 42, "Popped value should match pushed value");
zassert_true(rb_is_empty(), "Buffer should be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
int value = 0;

zassert_equal(rb_push(1), 0, "Push 1 should succeed");
zassert_equal(rb_push(2), 0, "Push 2 should succeed");
zassert_equal(rb_push(3), 0, "Push 3 should succeed");

zassert_equal(rb_pop(&value), 0, "First pop should succeed");
zassert_equal(value, 1, "First popped value should be 1");

zassert_equal(rb_pop(&value), 0, "Second pop should succeed");
zassert_equal(value, 2, "Second popped value should be 2");

zassert_equal(rb_pop(&value), 0, "Third pop should succeed");
zassert_equal(value, 3, "Third popped value should be 3");

zassert_true(rb_is_empty(), "Buffer should be empty after three pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
zassert_equal(rb_push(1), 0, "Push 1 should succeed");
zassert_equal(rb_push(2), 0, "Push 2 should succeed");
zassert_equal(rb_push(3), 0, "Push 3 should succeed");
zassert_equal(rb_push(4), 0, "Push 4 should succeed");

zassert_true(rb_is_full(), "Buffer should be full after 4 pushes");
zassert_equal(rb_push(99), -ENOSPC, "Push past capacity should return -ENOSPC");
zassert_equal(rb_count(), 4, "Rejected push should not change count");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
int value = 0;

zassert_equal(rb_push(7), 0, "Push should succeed");

zassert_equal(rb_peek(&value), 0, "First peek should succeed");
zassert_equal(value, 7, "First peek should return 7");
zassert_equal(rb_count(), 1, "Peek should not consume data");

value = 0;

zassert_equal(rb_peek(&value), 0, "Second peek should succeed");
zassert_equal(value, 7, "Second peek should still return 7");
zassert_equal(rb_count(), 1, "Second peek should not consume data");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
zassert_equal(rb_pop(NULL), -EINVAL, "rb_pop(NULL) should return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
zassert_false(rb_is_full(), "Fresh buffer should not be full");

zassert_equal(rb_push(1), 0, "Push 1 should succeed");
zassert_equal(rb_push(2), 0, "Push 2 should succeed");
zassert_equal(rb_push(3), 0, "Push 3 should succeed");
zassert_equal(rb_push(4), 0, "Push 4 should succeed");

zassert_true(rb_is_full(), "Buffer should be full after 4 pushes");
zassert_equal(rb_count(), 4, "Buffer count should be 4 after fill");
}
