// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sdkconfig.h"
#include "spi_sd_driver.h"
#include "unity.h"

#define TEST_ASSERT_SD_CARD_OK(condition) \
  TEST_ASSERT_EQUAL(SD_CARD_OK, condition)

static sdmmc_card_t card;

// TODO(Glibus): When we get 2nd revision, change the host to SDSPI_HOST_DEFAULT
// or sth like that :D
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

// TODO(Glibus): When we get 2nd revision, change the MOSI and MISO pins
spi_bus_config_t bus = {.miso_io_num = CONFIG_SD_MOSI_PIN_NUM,
                        .mosi_io_num = CONFIG_SD_MISO_PIN_NUM,
                        .sclk_io_num = CONFIG_SD_SCLK_PIN_NUM,
                        .quadwp_io_num = -1,
                        .quadhd_io_num = -1,
                        .max_transfer_sz = 4000};

const char* path = SD_CREATE_FILE_PREFIX("test.txt");

TEST_CASE("SD card init test", "[SPI_SD_DRIVER]") {
  // Card detect pin initialization
  gpio_config_t io_conf = {
      .pin_bit_mask = 1ULL << sd_card.card_detect_pin,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  TEST_ASSERT_EQUAL(ESP_OK, gpio_config(&io_conf));

  TEST_ASSERT_SD_CARD_OK(SD_init(&sd_card, &sd_card_config, &bus));
}

TEST_CASE("SD card mount test", "[SPI_SD_DRIVER]") {
  TEST_ASSERT_SD_CARD_OK(SD_mount(&sd_card));
}

TEST_CASE("SD card write test", "[SPI_SD_DRIVER]") {
  const char* data = "test";
  size_t length = strlen(data);
  TEST_ASSERT_SD_CARD_OK(SD_write(&sd_card, path, data, length));
}

TEST_CASE("SD file exists test", "[SPI_SD_DRIVER]") {
  TEST_ASSERT_EQUAL(true, SD_file_exists(path));
  TEST_ASSERT_EQUAL(false, SD_file_exists(SD_CREATE_FILE_PREFIX("test_2.txt")));
}

TEST_CASE("SD ok test", "[SPI_SD_DRIVER]") {
  TEST_ASSERT_EQUAL(true, SD_is_ok(&sd_card));
}

TEST_CASE("SD Card detect test", "[SPI_SD_DRIVER]") {
  // In this case, the card is not detected
  TEST_ASSERT_EQUAL(SD_CARD_CD_UNUSED, SD_card_detect(&sd_card));
}

TEST_CASE("SD unmount test", "[SPI_SD_DRIVER]") {
  TEST_ASSERT_SD_CARD_OK(SD_unmount(&sd_card));
}
