// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <string.h>

#include "spi_sd_driver.h"

#define SD_CARD_WRAPPER_MAX_FILE_NAME_LENGTH 64

typedef enum {
  LOGS_FILE = 0,
  SENSOR_CONTINUOUS_DATA_FILE,
  SENSOR_SINGLE_SHOT_DATA_FILE,
  MOTOR_CONTROLLER_FILE,
  SD_CARD_FILE_MAX
} sd_card_files_t;

typedef enum {
  SD_CARD_WRAPPER_OK = 0,
  SD_CARD_WRAPPER_INIT_ERROR,
  SD_CARD_WRAPPER_WRONG_FILE,
  SD_CARD_WRAPPER_FILES_INITIALIZATION_FAILED,
  SD_CARD_WRAPPER_ERROR
} sd_card_wrapper_status_t;

/*!
 * @brief Initiates the sd_card_wrapper
 * @return sd_card_wrapper_status_t - OK if operation was successful, INIT_ERROR
 * or FILES_INITIALIZATION_FAILED otherwise
 */
sd_card_wrapper_status_t sd_card_wrapper_init(void);

/*!
 * @brief Searches for files and creates full filenames
 * @return sd_card_wrapper_status_t - OK if operation was successful,
 * FILES_INITIALIZATION_FAILED otherwise
 */
sd_card_wrapper_status_t search_files_and_create_full_filenames();

void sd_card_on_log_received(char* data, size_t length);

void sd_card_on_sensor_continuous_data_received(char* data, size_t length);

void sd_card_on_sensor_single_shot_data_received(char* data, size_t length);

void sd_card_on_motor_controller_data_received(char* data, size_t length);
