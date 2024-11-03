// Copyright 2022 PWrInSpace. Kuba

#include "spi_sd_driver.h"

#include <string.h>
#include <sys/stat.h>

#include "esp_log.h"

#define TAG "SPI_SD_DRIVER"

sd_card_status_t SD_init(sd_card_t *sd_card, sd_card_config_t *cfg,
                         spi_bus_config_t *bus_cfg) {
  esp_err_t ret =
      spi_bus_initialize(sd_card->spi_host, bus_cfg, SDSPI_DEFAULT_DMA);
  if (ret != ESP_OK) {
    return SD_CARD_ERROR;
  }

  sd_card->spi_host = cfg->spi_host;
  sd_card->cs_pin = cfg->cs_pin;
  sd_card->card_detect_pin = cfg->cd_pin;
  sd_card->mount_point = cfg->mount_point;
  sd_card->initialized = true;

  sd_card_status_t mount = SD_mount(sd_card);
  if (mount != SD_CARD_OK) {
    return mount;
  }

  sd_card->mounted = true;
  return SD_CARD_OK;
}

sd_card_status_t SD_mount(sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }

  if (sd_card->mounted == true) {
    return SD_CARD_OK;
  }

  esp_err_t res;
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  // host.slot = sd_card->spi_host;
  // host.max_freq_khz = SDMMC_FREQ_PROBING;
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = sd_card->cs_pin;
  slot_config.host_id = host.slot;

  res = esp_vfs_fat_sdspi_mount(sd_card->mount_point, &host, &slot_config,
                                &mount_config, &sd_card->card);
  if (res != ESP_OK) {
    if (res == ESP_FAIL) {
      ESP_LOGD(TAG,
               "Failed to mount filesystem. "
               "If you want the card to be formatted, set the"
               "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    }
    return SD_CARD_MOUNT_ERROR;
  }
  sd_card->mounted = true;
  return SD_CARD_OK;
}

sd_card_status_t SD_file_exists(const char *file_name, sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }
  struct stat st;
  int ret = stat(file_name, &st);
  if (ret == 0) {
    return SD_CARD_FILE_EXISTS;
  }
  return SD_CARD_FILE_DOESNT_EXIST;
}

sd_card_status_t SD_remount(sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }
  sd_card_status_t res;
  res = SD_unmount(sd_card);
  if (res != SD_CARD_OK) {
    return SD_CARD_ERROR;
  }

  res = SD_mount(sd_card);
  return res;
}

sd_card_status_t SD_unmount(sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }
  if (sd_card->mounted == false) {
    return SD_CARD_OK;
  }

  esp_err_t res;
  res = esp_vfs_fat_sdcard_unmount(sd_card->mount_point, sd_card->card);
  if (res != ESP_OK) {
    return SD_CARD_UNMOUNT_ERROR;
  }
  sd_card->mounted = false;
  return SD_CARD_OK;
}

sd_card_status_t SD_write(sd_card_t *sd_card, const char *path,
                          const char *data, size_t length) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }

  if (sd_card->mounted == false) {
    if (SD_mount(sd_card) != SD_CARD_OK) {
      return SD_CARD_MOUNT_ERROR;
    }
  }

  if (sdmmc_get_status(sd_card->card) != ESP_OK) {
    SD_remount(sd_card);
  }

  FILE *file = fopen(path, "a");
  if (file == NULL) {
    return SD_CARD_WRITE_ERROR;
  }

  int written_bytes = 0;
  written_bytes = fprintf(file, data, sd_card->card->cid.name);
  fclose(file);

  if (written_bytes < 1) {
    return SD_CARD_WRITE_ERROR;
  }

  return SD_CARD_OK;
}

bool SD_is_ok(sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return false;
  }
  esp_err_t res = sdmmc_get_status(sd_card->card);
  if (res != ESP_OK) {
    return false;
  }

  return true;
}

sd_card_status_t SD_card_detect(sd_card_t *sd_card) {
  if (!sd_card->initialized) {
    return SD_CARD_UNINITIALIZED_ERROR;
  }
  if (sd_card->card_detect_pin == 0) {
    return SD_CARD_CD_UNUSED;
  }

  if (gpio_get_level(sd_card->card_detect_pin) == 0) {
    return SD_CARD_CARD_DETECTED;
  }

  return SD_CARD_CARD_NOT_DETECTED_ERROR;
}

bool create_path_to_file(sd_card_t *sd_card, char *file_path, size_t size) {
  char *path = (char *)calloc(size, sizeof(char));
  int ret_snprintf = 0;
  for (int i = 0; i < 1000; ++i) {
    ret_snprintf = snprintf(path, size, "%s%d.txt", file_path, i);
    if (ret_snprintf == size) {
      free(path);
      return false;
    }
    sd_card_status_t ret = SD_file_exists(path, sd_card);
    if (ret == SD_CARD_FILE_DOESNT_EXIST) {
      memcpy(file_path, path, size);
      free(path);
      return true;
    } else if (ret == SD_CARD_UNINITIALIZED_ERROR) {
      return false;
    }
  }

  free(path);
  return false;
}
