// // Copyright 2024 MACKI

// #include <string.h>

// #include "log_manager.h"
// #include "sdkconfig.h"
// #include "unity.h"

// typedef enum {
//   LOG_RECEIVER_DUMMY = 0,
//   LOG_RECEIVER_MAX_NUM = 1
// } log_receiver_instance_t;

// typedef enum {
//   LOG_LEVEL_INFO = 3,
// } log_level_t;

// #define TEST_TAG "TAG_ONE"
// #define TEST_MESSAGE "MESSAGE_ONE"

// const char* expected_message_without_timestamp = "TAG_ONE; MESSAGE_ONE\r\n";

// static char expected_message[100];

// static log_manager_t test_manager = {
//     .num_receivers = 0,
//     .receivers = {NULL},
//     .log_buffer = {0},
//     .initalized = false,
// };

// static struct {
//   char* data;
//   size_t length;
// } dummy_receiver_verification_struct;

// static void dummy_on_log_received(char* data, size_t length) {
//   dummy_receiver_verification_struct.data = malloc(length);
//   strncpy(dummy_receiver_verification_struct.data, data, length);
//   dummy_receiver_verification_struct.length = length;
// }

// static log_receiver_t test_receivers[LOG_RECEIVER_MAX_NUM] = {
//     {
//         .instance = LOG_RECEIVER_DUMMY,
//         .process_log = dummy_on_log_received,
//     },
// };

// TEST_CASE("log_manager_init Test", "[LOG_MANAGER]") {
//   log_manager_status_t ret = log_manager_add_receiver(
//       &test_manager, &test_receivers[LOG_RECEIVER_DUMMY]);
//   TEST_ASSERT_EQUAL(LOGGER_OK, ret);

//   ret = log_manager_init(&test_manager);
//   TEST_ASSERT_EQUAL(LOGGER_OK, ret);
// }

// TEST_CASE("log_manager_log_message and concatenate_log_string Test successful",
//           "[LOG_MANAGER]") {
//   // For the test to work well, we need to reset the system time here
//   rtc_wrapper_reset_time_base();

//   // Verify that logging a message with the log manager works
//   log_manager_status_t ret =
//       log_manager_log_message(&test_manager, TEST_TAG, TEST_MESSAGE);
//   TEST_ASSERT_EQUAL(LOGGER_OK, ret);

//   // Now we will peek into ring buffer to see if it all worked
//   void* data = malloc(sizeof(log_string_t));

//   ring_buffer_status_t rb_ret =
//       ring_buffer_peek(&test_manager.log_buffer, &data);
//   TEST_ASSERT_EQUAL(RING_BUFFER_OK, rb_ret);

//   TEST_ASSERT_EQUAL_STRING(TEST_MESSAGE, ((log_string_t*)data)->message);
//   TEST_ASSERT_EQUAL_STRING(TEST_TAG, ((log_string_t*)data)->tag);
//   TEST_ASSERT_INT32_WITHIN(50, 50, ((log_string_t*)data)->timestamp);

//   // Lets also test the message concatenation function, with the stolen
//   // timestamp so the test sie nie wyjebuje
//   log_string_t log_string = {
//       .timestamp = ((log_string_t*)data)->timestamp,
//   };

//   strncpy(log_string.message, ((log_string_t*)data)->message,
//           strlen(((log_string_t*)data)->message));
//   strncpy(log_string.tag, ((log_string_t*)data)->tag,
//           strlen(((log_string_t*)data)->tag));
//   char buffer[512];
//   ret = log_manager_concatenate_log_string(log_string, buffer);

//   snprintf(expected_message, sizeof(expected_message), "(%" PRId64 ") %s",
//            ((log_string_t*)data)->timestamp,
//            expected_message_without_timestamp);

//   TEST_ASSERT_EQUAL_STRING(expected_message, buffer);
// }

// TEST_CASE("log_manager_save_logs Test", "[LOG_MANAGER]") {
//   // Now we will save the log that is there from the previous test
//   log_manager_status_t ret = log_manager_save_logs(&test_manager);
//   TEST_ASSERT_EQUAL(LOGGER_OK, ret);

//   // Now we will verify that the data was passed to the receiver
//   TEST_ASSERT_EQUAL_STRING_LEN(expected_message,
//                                dummy_receiver_verification_struct.data,
//                                strlen(expected_message));
//   free(dummy_receiver_verification_struct.data);
// }
