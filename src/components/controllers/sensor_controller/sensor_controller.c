// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "sensor_controller.h"

#include <string.h>

#include "adc_wrapper.h"
#include "macki_log.h"
#include "sensor_converters.h"
#include "sensor_driver_definitions.h"

#define TAG "SENSOR_CONTROLLER"

typedef struct {
  ads1115_driver_t* adc_expander;
  lis2dw12_driver_t* accelerometer;
  tmp1075_driver_t* temperature_sensor;
  vl53l0x_driver_t* distance_sensor;
} sensor_controller_drivers_t;

static ring_buffer_t sensor_data_buffer;

static sensor_controller_drivers_t sensor_controller_drivers = {
    .adc_expander = &adc_expander,
    .accelerometer = &accelerometer,
    .temperature_sensor = &temperature_sensor,
    .distance_sensor = &distance_sensor,
};

static sensor_controller_single_shot_data_t read_single_shot_data();
static sensor_controller_continuous_data_t read_continuous_data();

bool sensor_controller_init() {
  rtc_wrapper_init();
  init_shared_i2c_wrapper();

  bool adc_ret = adc_wrapper_init();
  if (!adc_ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize ADC wrapper");
    return false;
  }

  ring_buffer_status_t rb_ret = ring_buffer_init(&sensor_data_buffer, 256);
  if (rb_ret != RING_BUFFER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize sensor data buffer");
    return false;
  }

  bool ret = init_vl53l0x(sensor_controller_drivers.distance_sensor, 1);
  if (!ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize VL53L0X driver");
    return false;
  }
  ads1115_driver_status_t status =
      ads1115_driver_init(sensor_controller_drivers.adc_expander);
  if (status != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize ADS1115 driver");
    return false;
  }
  status = ads1115_driver_start_continuous_conversion(
      sensor_controller_drivers.adc_expander);
  if (status != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to start continuous conversion ADS1115 driver");
    return false;
  }

  lis2dw12_driver_status_t acc_status =
      lis2dw12_driver_init(sensor_controller_drivers.accelerometer);
  if (acc_status != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize LIS2DW12 driver");
    return false;
  }
  tmp1075_driver_status_t tmp_status =
      tmp1075_driver_init(sensor_controller_drivers.temperature_sensor);
  if (tmp_status != TMP1075_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize TMP1075 driver");
    return false;
  }

  MACKI_LOG_INFO(TAG, "Sensor controller initialized");
  return true;
}

// TODO(Glibus): Remove this when implementing saving data
static void clear_buffer() {
  sensor_controller_data_t* last_data =
      (sensor_controller_data_t*)malloc(sizeof(sensor_controller_data_t));
  while (ring_buffer_pop(&sensor_data_buffer, (void**)&last_data) ==
         RING_BUFFER_OK) {
    (void)last_data;
  }
}

sensor_controller_data_transmission_t sensor_controller_get_last_data() {
  sensor_controller_data_transmission_t data = {0};
  sensor_controller_data_t* last_data =
      (sensor_controller_data_t*)malloc(sizeof(sensor_controller_data_t));
  ring_buffer_status_t status =
      ring_buffer_peek_last(&sensor_data_buffer, (void**)&last_data);
  if (status != RING_BUFFER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to get last data from the ring buffer");
    return data;
  }
  data.time_us = last_data->single_shot_data.time_us;
  data.load_cell_reading = last_data->single_shot_data.load_cell_reading;
  data.tmp1075_temperature = last_data->single_shot_data.tmp1075_temperature;
  data.pressure_sensor_1 = last_data->single_shot_data.pressure_sensor_1;
  data.pressure_sensor_2 = last_data->single_shot_data.pressure_sensor_2;
  data.distance = last_data->single_shot_data.distance;
  data.lis2dw12_acc_x =
      last_data->continuous_data.accelerometer_data.samples[0].x;
  data.lis2dw12_acc_y =
      last_data->continuous_data.accelerometer_data.samples[0].y;
  data.lis2dw12_acc_z =
      last_data->continuous_data.accelerometer_data.samples[0].z;
  // TODO(Glibus): remove this in the future when implementing saving data
  clear_buffer();

  return data;
}

void single_shot_data_header_to_string(char buffer[256]) {
  sprintf(buffer,
          "Time;Load cell reading [N];Temperature [C];Pressure sensor 1 "
          "[bar];Pressure sensor 2 [bar];Distance [mm];");
}

void continuous_data_header_to_string(char buffer[256]) {
  sprintf(buffer, "Time;acc_x;acc_y;acc_z;");
}

void single_shot_data_to_string(sensor_controller_single_shot_data_t data,
                                char buffer[256]) {
  sprintf(buffer, "%lld;%f;%f;%f;%f;%d;", data.time_us, data.load_cell_reading,
          data.tmp1075_temperature, data.pressure_sensor_1,
          data.pressure_sensor_2, data.distance);
}

void continuous_data_to_string(sensor_controller_continuous_data_t data,
                               char buffer[256]) {
  sprintf(buffer, "%lld;%d;%d;%d;", data.time_us,
          data.accelerometer_data.samples[0].x,
          data.accelerometer_data.samples[0].y,
          data.accelerometer_data.samples[0].z);
}

void read_and_buffer_sensor_data() {
  sensor_controller_data_t* data =
      (sensor_controller_data_t*)malloc(sizeof(sensor_controller_data_t));

  data->continuous_data = read_continuous_data();
  data->single_shot_data = read_single_shot_data();

  ring_buffer_status_t status =
      ring_buffer_push(&sensor_data_buffer, (void*)data);
  if (status != RING_BUFFER_OK) {
    // MACKI_LOG_ERROR(TAG, "Failed to push data to the ring buffer");
    free(data);
  }
}

sensor_controller_continuous_data_t read_continuous_data() {
  sensor_controller_continuous_data_t data;
  data.time_us = rtc_wrapper_get_time_ms();
  lis2dw12_driver_read_fifo_data(sensor_controller_drivers.accelerometer,
                                 &data.accelerometer_data);

  char buffer[256];
  continuous_data_to_string(data, buffer);
  // MACKI_LOG_INFO(TAG, "%s", buffer);
  return data;
}

sensor_controller_single_shot_data_t read_single_shot_data() {
  sensor_controller_single_shot_data_t data;
  data.time_us = rtc_wrapper_get_time_ms();

  // Load cell
  int16_t raw_load_cell_reading = 0;
  adc_wrapper_get_reading(ADC_STRAIN_GAUGE, &raw_load_cell_reading);

  data.load_cell_reading =
      load_cell_millivolt_to_newtons(raw_load_cell_reading);

  // Temperature
  int16_t raw_temperature = 0;
  tmp1075_driver_read_raw_temperature(
      sensor_controller_drivers.temperature_sensor, &raw_temperature);
  data.tmp1075_temperature =
      tmp1075_driver_convert_raw_temperature_to_celsius(raw_temperature);

  // Pressure sensors
  float raw_mv_data;

  // Pressure sensor 1
  ads1115_driver_select_pin(sensor_controller_drivers.adc_expander,
                            MUX_AIN0_GND);
  vTaskDelay(pdMS_TO_TICKS(20));
  ads1115_driver_get_conversion_data_millivolts(
      sensor_controller_drivers.adc_expander, &raw_mv_data);
  data.pressure_sensor_1 = pressure_sensor_volt_to_bar(raw_mv_data);

  // Pressure sensor 2
  ads1115_driver_select_pin(sensor_controller_drivers.adc_expander,
                            MUX_AIN1_GND);
  vTaskDelay(pdMS_TO_TICKS(20));
  ads1115_driver_get_conversion_data_millivolts(
      sensor_controller_drivers.adc_expander, &raw_mv_data);
  data.pressure_sensor_2 = pressure_sensor_volt_to_bar(raw_mv_data);

  // Distance sensor
  stat_info_t stat;
  data.distance = read_range_single_millimeters(
      sensor_controller_drivers.distance_sensor, &stat);

  char buffer[256];
  single_shot_data_to_string(data, buffer);
  // MACKI_LOG_INFO(TAG, "%s", buffer);
  return data;
}
