// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "i2c_driver.h"
#include "sdkconfig.h"
#include "unity.h"

#define TEST_ASSERT_I2C_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(I2C_DRIVER_OK, condition)

i2c_driver_t driver =
    {
        .config =
            {
                .mode = I2C_MODE_MASTER,
                .sda_io_num = CONFIG_I2C_SDA_PIN_NUM,
                .scl_io_num = CONFIG_I2C_SCL_PIN_NUM,
                .sda_pullup_en = GPIO_PULLUP_ENABLE,
                .scl_pullup_en = GPIO_PULLUP_ENABLE,
                .master.clk_speed = 100000,
            },
        .port = I2C_NUM_0,
        .rx_buf_setting = I2C_BUF_DISABLE,
        .tx_buf_setting = I2C_BUF_DISABLE,
};

TEST_CASE("I2C driver init test", "[I2C_DRIVER]") {
  TEST_ASSERT_I2C_DRIVER_OK(i2c_driver_init(&driver));
}

TEST_CASE("I2C driver deinit test", "[I2C_DRIVER]") {
  TEST_ASSERT_I2C_DRIVER_OK(i2c_driver_deinit(&driver));
}

/*!
 * @note Other tests with sending/receiving data
 * will be implemented for ICs
 */
