// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "ring_buffer.h"
#include "sdkconfig.h"
#include "unity.h"

#define RING_BUFFER_TEST_ELEMENT 2137
#define RING_BUFFER_TEST_ELEMENT_ALT 420
static ring_buffer_t test_ring_buffer;

static void ring_buffer_reset() {
  // void** data = malloc(sizeof(void*));
  // while (!ring_buffer_is_empty(&test_ring_buffer)) {
  //   ring_buffer_pop(&test_ring_buffer, data);
  // }
  ring_buffer_init(&test_ring_buffer, CONFIG_MAX_RING_BUFFER_SIZE);
}

TEST_CASE("ring_buffer_init Test Fail", "[RING_BUFFER]") {
  ring_buffer_status_t ret =
      ring_buffer_init(&test_ring_buffer, CONFIG_MAX_RING_BUFFER_SIZE + 10);
  TEST_ASSERT_EQUAL(RING_BUFFER_ERROR, ret);
}

TEST_CASE("ring_buffer_init Test Success", "[RING_BUFFER]") {
  ring_buffer_status_t ret =
      ring_buffer_init(&test_ring_buffer, CONFIG_MAX_RING_BUFFER_SIZE);
  TEST_ASSERT_EQUAL(CONFIG_MAX_RING_BUFFER_SIZE, test_ring_buffer.size);
  TEST_ASSERT_EQUAL(0, test_ring_buffer.head);
  TEST_ASSERT_EQUAL(0, test_ring_buffer.tail);
  TEST_ASSERT_EQUAL(0, test_ring_buffer.count);
  TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
}

TEST_CASE("ring_buffer_push Test Successful", "[RING_BUFFER]") {
  ring_buffer_status_t ret =
      ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
  TEST_ASSERT_EQUAL(1, test_ring_buffer.count);
  TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
}

TEST_CASE("ring_buffer_pop Test Successful", "[RING_BUFFER]") {
  void** data = malloc(sizeof(void*));
  ring_buffer_status_t ret = ring_buffer_pop(&test_ring_buffer, data);
  TEST_ASSERT_EQUAL(RING_BUFFER_TEST_ELEMENT, *data);
  TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
}

TEST_CASE("ring_buffer_push Test Overflow", "[RING_BUFFER]") {
  ring_buffer_reset();
  // First we're pushing a different value for the sake of the next test
  ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT_ALT);

  // filling the ring buffer to the brim
  for (uint16_t i = 0; i < CONFIG_MAX_RING_BUFFER_SIZE - 2; ++i) {
    ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
  }
  // pushing so the ring buffer becomes empty - still should return OK
  ring_buffer_status_t ret;
  ret =
      ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
  TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
  // next push should just return RING_BUFFER_FULL
  // additionally the count and head should be at CONFIG_MAX_RING_BUFFER_SIZE
  ret =
      ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
  TEST_ASSERT_EQUAL(RING_BUFFER_FULL, ret);
  TEST_ASSERT_EQUAL(CONFIG_MAX_RING_BUFFER_SIZE, test_ring_buffer.count);
  TEST_ASSERT_EQUAL(0, test_ring_buffer.head);
}

TEST_CASE("ring_buffer_pop Test Successful", "[RING_BUFFER]") {
  // ring buffer is pre-filled in from the last test
  void* data = malloc(sizeof(void*));
  ring_buffer_status_t ret = ring_buffer_pop(&test_ring_buffer, &data);
  TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
  TEST_ASSERT_EQUAL(RING_BUFFER_TEST_ELEMENT_ALT, data);

  // now popping all elements until end - it should all look g
  for (uint16_t i = 0; i < CONFIG_MAX_RING_BUFFER_SIZE - 1; ++i) {
    ret = ring_buffer_pop(&test_ring_buffer, &data);
    TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
    TEST_ASSERT_EQUAL(RING_BUFFER_TEST_ELEMENT, data);
  }
  // now trying to pop element from empty buffer, should return
  // RING_BUFFER_EMPTY
  ret = ring_buffer_pop(&test_ring_buffer, &data);
  TEST_ASSERT_EQUAL(RING_BUFFER_EMPTY, ret);
}

TEST_CASE("ring_buffer_push through the head and tail", "[RING_BUFFER]") {
  // reset the ring buffer
  ring_buffer_reset();
  // push elements until the head is in the middle of the buffer
  for (uint16_t i = 0; i < CONFIG_MAX_RING_BUFFER_SIZE / 2; ++i) {
    ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
  }
  // pop elements until the tail is in the middle of the buffer
  void* data = malloc(sizeof(void*));
  for (uint16_t i = 0; i < CONFIG_MAX_RING_BUFFER_SIZE / 2; ++i) {
    ring_buffer_pop(&test_ring_buffer, &data);
  }

  // now we verify that the buffer is empty
  TEST_ASSERT_EQUAL(RING_BUFFER_EMPTY, ring_buffer_is_empty(&test_ring_buffer));
  ring_buffer_status_t ret;

  // push elements until the head is at the end of the buffer
  // during the whole process, the return should be RING_BUFFER_OK
  // and there should be no memory leak (optional xD)
  for (uint16_t i = 0; i < CONFIG_MAX_RING_BUFFER_SIZE; ++i) {
    ret = ring_buffer_push(&test_ring_buffer, (void*)RING_BUFFER_TEST_ELEMENT);
    TEST_ASSERT_EQUAL(RING_BUFFER_OK, ret);
  }
}

TEST_CASE("ring_buffer_is_empty Test success", "[RING_BUFFER]") {
  ring_buffer_reset();
  ring_buffer_status_t ret = ring_buffer_is_empty(&test_ring_buffer);
  TEST_ASSERT_EQUAL(RING_BUFFER_EMPTY, ret);
}
