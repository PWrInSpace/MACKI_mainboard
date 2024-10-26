// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sd_card_wrapper.h"

#include "macki_log.h"
#include "sdkconfig.h"

#define TAG "SD_CARD_WRAPPER"

static sdmmc_card_t card;

static const spi_host_device_t host = SPI2_HOST;

static sd_card_t sd_card = {
    .card = &card,
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS_PIN_NUM,
    .card_detect_pin = CONFIG_SD_CD_PIN_NUM,
    .mounted = false,
};

static sd_card_config_t sd_card_config = {
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS_PIN_NUM,
    .cd_pin = CONFIG_SD_CD_PIN_NUM,
};

spi_bus_config_t bus = {.miso_io_num = CONFIG_SD_MOSI_PIN_NUM,
                        .mosi_io_num = CONFIG_SD_MISO_PIN_NUM,
                        .sclk_io_num = CONFIG_SD_SCLK_PIN_NUM,
                        .quadwp_io_num = -1,
                        .quadhd_io_num = -1,
                        .max_transfer_sz = 4000};

static struct {
  char filenames[SD_CARD_FILE_MAX][SD_CARD_WRAPPER_MAX_FILE_NAME_LENGTH];
  bool files_initialized;
} sd_wrapper_info = {
    .filenames = {[LOGS_FILE] = "logs_",
                  [SENSOR_CONTINUOUS_DATA_FILE] = "sensor_continuous_data_",
                  [SENSOR_SINGLE_SHOT_DATA_FILE] = "sensor_single_shot_data_",
                  [MOTOR_CONTROLLER_FILE] = "motor_controller_"},
    .files_initialized = false,
};

sd_card_wrapper_status_t sd_card_wrapper_init(void) {
  if (SD_init(&sd_card, &sd_card_config, &bus) != SD_CARD_OK) {
    return SD_CARD_WRAPPER_INIT_ERROR;
  }

  sd_card_wrapper_status_t ret = search_files_and_create_full_filenames();
  if (ret != SD_CARD_WRAPPER_OK) {
    return ret;
  }

  return SD_CARD_WRAPPER_OK;
}

sd_card_wrapper_status_t search_files_and_create_full_filenames() {
  bool _files_initialized = true;
  for (uint8_t i = 0; i < SD_CARD_FILE_MAX; ++i) {
    bool ret = create_path_to_file(&sd_card, sd_wrapper_info.filenames[i],
                                   SD_CARD_WRAPPER_MAX_FILE_NAME_LENGTH);
    if (!ret) {
      MACKI_LOG_ERROR(TAG, "Couldn't create filename for file %s",
                      sd_wrapper_info.filenames[i]);
      _files_initialized = false;
    }
  }

  sd_wrapper_info.files_initialized = _files_initialized;
  return _files_initialized ? SD_CARD_WRAPPER_OK
                            : SD_CARD_WRAPPER_FILES_INITIALIZATION_FAILED;
}

void sd_card_on_log_received(char* data, size_t length) {
  if (!sd_wrapper_info.files_initialized) {
    MACKI_LOG_ERROR(TAG, "Files not initialized, failed to save on SD");
    return;
  }

  SD_write(&sd_card, sd_wrapper_info.filenames[LOGS_FILE], (const char*)data,
           length);
}

void sd_card_on_sensor_continuous_data_received(char* data, size_t length) {
  if (!sd_wrapper_info.files_initialized) {
    MACKI_LOG_ERROR(TAG, "Files not initialized, failed to save on SD");
    return;
  }

  SD_write(&sd_card, sd_wrapper_info.filenames[SENSOR_CONTINUOUS_DATA_FILE],
           (const char*)data, length);
}

void sd_card_on_sensor_single_shot_data_received(char* data, size_t length) {
  if (!sd_wrapper_info.files_initialized) {
    MACKI_LOG_ERROR(TAG, "Files not initialized, failed to save on SD");
    return;
  }

  SD_write(&sd_card, sd_wrapper_info.filenames[SENSOR_SINGLE_SHOT_DATA_FILE],
           (const char*)data, length);
}

void sd_card_on_motor_controller_data_received(char* data, size_t length) {
  if (!sd_wrapper_info.files_initialized) {
    MACKI_LOG_ERROR(TAG, "Files not initialized, failed to save on SD");
    return;
  }

  SD_write(&sd_card, sd_wrapper_info.filenames[MOTOR_CONTROLLER_FILE],
           (const char*)data, length);
}