// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

typedef enum {
  TMP1075_TEMP_REG = 0x00U,
  TMP1075_CFGR_REG = 0x01U,
  TMP1075_DEVICE_ID_REG = 0x0FU,
} tmp1075_registers_t;

#define TMP1075_CFGR_OS_ENABLE 1U << 7U
#define TMP1075_CFGR_OS_DISABLE 0U << 7U

#define TMP1075_CFGR_CONVERSION_RATE_27_5 0U << 6U | 0U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_55 0U << 6U | 1U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_110 1U << 6U | 0U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_220 1U << 6U | 1U << 5U

#define TMP1075_CFGR_ALERT_CONSECUTIVE_UNUSED 0U << 4U | 0U << 3U

#define TMP1075_CFGR_ALERT_POLARITY_LOW 0U << 2U
#define TMP1075_CFGR_ALERT_POLARITY_HIGH 1U << 2U

#define TMP1075_CFGR_ALERT_MODE_COMPARATOR 0U << 1U
#define TMP1075_CFGR_ALERT_MODE_INTERRUPT 1U << 1U

#define TMP1075_CFGR_CONTINUOUS_CONVERSION 0U << 0U
#define TMP1075_CFGR_SHUTDOWN_MODE 1U << 0U