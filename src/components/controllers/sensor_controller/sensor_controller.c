// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "sensor_controller.h"

#include <string.h>

#include "adc_wrapper.h"
#include "gpio_wrapper.h"
#include "macki_log.h"
#include "macus_helper.h"
#include "mechanical_controller.h"
#include "sd_card_wrapper.h"
#include "sensor_converters.h"
#include "sensor_driver_definitions.h"

#define TAG "SENSOR_CONTROLLER"

#define SAMPLES_TO_KEEP_IN_BUFFER 3

static ring_buffer_t sensor_data_buffer;

static int64_t procedure_start_time = 0;

static struct {
  ads1115_driver_t* adc_expander;
  lis2dw12_driver_t* accelerometer;
  tmp1075_driver_t* temperature_sensor;
  vl53l0x_driver_t* distance_sensor;
  uint8_t load_cell_tare_gpio_num;
} sensor_controller_drivers = {
    .adc_expander = &adc_expander,
    .accelerometer = &accelerometer,
    .temperature_sensor = &temperature_sensor,
    .distance_sensor = &distance_sensor,
    .load_cell_tare_gpio_num = GPIO_PIN_TARE_LOAD_CELL,
};

static sensor_controller_single_shot_data_t read_single_shot_data();
static sensor_controller_continuous_data_t read_continuous_data();

bool sensor_controller_init() {
  rtc_wrapper_init();

  bool adc_ret = adc_wrapper_init();
  if (!adc_ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize ADC wrapper");
    return false;
  }

  ring_buffer_status_t rb_ret =
      ring_buffer_init(&sensor_data_buffer, SENSOR_DATA_RING_BUFFER_SIZE,
                       sizeof(sensor_controller_data_t));
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
    MACKI_LOG_ERROR(TAG,
                    "Failed to start continuous conversion ADS1115 driver");
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

  // At this point the gpio wrapper should have been initiated in mechanical
  // controllerbut just to be safe, let's do it again
  ret = gpio_wrapper_init();
  if (!ret) {
    MACKI_LOG_WARN(
        TAG, "Failed to initialize GPIO wrapper, probably already initialized");
  }
  tare_load_cell();

  MACKI_LOG_INFO(TAG, "Sensor controller initialized");
  return true;
}

bool sensor_controller_get_last_data(char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  sensor_controller_data_transmission_t data = {0};
  sensor_controller_data_t last_data = {0};
  ring_buffer_status_t status =
      ring_buffer_peek_last(&sensor_data_buffer, (void**)&last_data);
  if (status != RING_BUFFER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to get last data from the ring buffer");
    return false;
  }
  data.time_us = last_data.single_shot_data.time_us;
  data.load_cell_reading = last_data.single_shot_data.load_cell_reading;
  data.tmp1075_temperature = last_data.single_shot_data.tmp1075_temperature;
  data.pressure_sensor_1 = last_data.single_shot_data.pressure_sensor_1;
  data.pressure_sensor_2 = last_data.single_shot_data.pressure_sensor_2;
  data.distance = last_data.single_shot_data.distance;
  data.lis2dw12_acc_x =
      last_data.continuous_data.accelerometer_data.samples[0].x;
  data.lis2dw12_acc_y =
      last_data.continuous_data.accelerometer_data.samples[0].y;
  data.lis2dw12_acc_z =
      last_data.continuous_data.accelerometer_data.samples[0].z;
  data.left_motor_speed = get_motor_speed(STEPPER_MOTOR_0);
  data.right_motor_speed = get_motor_speed(STEPPER_MOTOR_1);
  data.procedure_time_ms = rtc_wrapper_get_time_ms() - procedure_start_time;
  data.is_mechanical_controller_blocked =
      (int16_t)is_mechanical_controller_blocked();

  transmission_data_to_string(data, buffer);

  return true;
}

void read_and_buffer_sensor_data() {
  sensor_controller_data_t data = {0};

  data.continuous_data = read_continuous_data();
  data.single_shot_data = read_single_shot_data();

  ring_buffer_status_t status =
      ring_buffer_push(&sensor_data_buffer, (void*)&data);
  if (status != RING_BUFFER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to push data to the ring buffer");
  }
}

void read_and_save_macus_data() {
  macus_status_t ret = MACUS_STATUS_OK;

  sensor_controller_macus_data_t macus_data = {0};
  char buffer[MACUS_DATA_STRING_SIZE];
  uint8_t frames_buffered;
  macus_get_buffered_frames(&frames_buffered);

  if (frames_buffered == 0) {
    MACKI_LOG_ERROR(TAG, "No MACUS data available");
    return;
  }

  for (uint8_t i = 0; i < frames_buffered; i++) {
    ret = macus_get_data(&macus_data);
    if (ret != MACUS_STATUS_OK) {
      MACKI_LOG_ERROR(TAG, "Failed to get MACUS data, reason: %s",
                      macus_status_to_string(ret));
      break;
    }
    macus_data_to_string(macus_data, buffer);
    sd_card_on_macus_data_received(buffer, MACUS_DATA_STRING_SIZE);
  }
}

sensor_controller_continuous_data_t read_continuous_data() {
  sensor_controller_continuous_data_t data;
  data.time_us = rtc_wrapper_get_time_ms();
  lis2dw12_driver_read_fifo_data(sensor_controller_drivers.accelerometer,
                                 &data.accelerometer_data);

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
                            MUX_AIN2_GND);
  vTaskDelay(pdMS_TO_TICKS(20));
  ads1115_driver_get_conversion_data_millivolts(
      sensor_controller_drivers.adc_expander, &raw_mv_data);
  data.pressure_sensor_1 = pressure_sensor_volt_to_bar(raw_mv_data);

  // Pressure sensor 2
  ads1115_driver_select_pin(sensor_controller_drivers.adc_expander,
                            MUX_AIN3_GND);
  vTaskDelay(pdMS_TO_TICKS(20));
  ads1115_driver_get_conversion_data_millivolts(
      sensor_controller_drivers.adc_expander, &raw_mv_data);
  data.pressure_sensor_2 = pressure_sensor_volt_to_bar(raw_mv_data);

  // Distance sensor
  stat_info_t stat;
  data.distance = read_range_single_millimeters(
      sensor_controller_drivers.distance_sensor, &stat);

  return data;
}

void single_shot_data_header_to_string(
    char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  sprintf(buffer,
          "Time;Load cell reading [N];Temperature [C];Pressure sensor 1 "
          "[bar];Pressure sensor 2 [bar];Distance [mm];\n");
}

void continuous_data_header_to_string(char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  sprintf(buffer, "Time;acc_x;acc_y;acc_z;\n");
}

void single_shot_data_to_string(sensor_controller_single_shot_data_t data,
                                char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  sprintf(buffer, "%lld;%f;%f;%f;%f;%d;\n", data.time_us,
          data.load_cell_reading, data.tmp1075_temperature,
          data.pressure_sensor_1, data.pressure_sensor_2, data.distance);
}

void continuous_data_to_string(sensor_controller_continuous_data_t data,
                               char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  sprintf(buffer, "%lld;%f;%f;%f;", data.time_us,
          data.accelerometer_data.samples[0].x,
          data.accelerometer_data.samples[0].y,
          data.accelerometer_data.samples[0].z);
}

void continuous_data_to_string_all_data(
    sensor_controller_continuous_data_t data,
    char buffer[SENSOR_DATA_SD_BUFFER_SIZE * 2]) {
  for (size_t i = 0; i < data.accelerometer_data.current_samples_number; i++) {
    sprintf(buffer, "%lld;%f;%f;%f;", data.time_us,
            data.accelerometer_data.samples[i].x,
            data.accelerometer_data.samples[i].y,
            data.accelerometer_data.samples[i].z);
  }
}

void transmission_data_to_string(sensor_controller_data_transmission_t data,
                                 char buffer[SENSOR_DATA_SD_BUFFER_SIZE]) {
  snprintf(buffer, SENSOR_DATA_SD_BUFFER_SIZE,
           "%lld;%f;%f;%f;%f;%d;%f;%f;%f;%ld;%ld;%lld;%d", data.time_us,
           data.load_cell_reading, data.tmp1075_temperature,
           data.pressure_sensor_1, data.pressure_sensor_2, data.distance,
           data.lis2dw12_acc_x, data.lis2dw12_acc_y, data.lis2dw12_acc_z,
           data.left_motor_speed, data.right_motor_speed,
           data.procedure_time_ms, data.is_mechanical_controller_blocked);
}

void sensor_controller_save_data_to_sd() {
  while (ring_buffer_get_count(&sensor_data_buffer) >
         SAMPLES_TO_KEEP_IN_BUFFER) {
    sensor_controller_data_t data;
    ring_buffer_pop(&sensor_data_buffer, (void**)&data);

    char buffer[SENSOR_DATA_SD_BUFFER_SIZE];
    single_shot_data_to_string(data.single_shot_data, buffer);
    sd_card_on_sensor_single_shot_data_received(buffer, strlen(buffer));

    char buffer2[SENSOR_DATA_SD_BUFFER_SIZE * 2];
    continuous_data_to_string_all_data(data.continuous_data, buffer2);
    sd_card_on_sensor_continuous_data_received(buffer2, strlen(buffer2));
  }
}

void sensor_controller_print_header_on_sd() {
  char buffer[SENSOR_DATA_SD_BUFFER_SIZE];
  single_shot_data_header_to_string(buffer);
  sd_card_on_sensor_single_shot_data_received(buffer, strlen(buffer));

  continuous_data_header_to_string(buffer);
  sd_card_on_sensor_continuous_data_received(buffer, strlen(buffer));
}

size_t sensor_controller_get_ring_buffer_count() {
  return ring_buffer_get_count(&sensor_data_buffer);
}

void update_procedure_start_time(int64_t time_ms) {
  procedure_start_time = time_ms;
}

void tare_load_cell() {
  if (sensor_controller_drivers.load_cell_tare_gpio_num != 0) {
    gpio_pin_set_level(sensor_controller_drivers.load_cell_tare_gpio_num,
                       GPIO_LEVEL_HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_pin_set_level(sensor_controller_drivers.load_cell_tare_gpio_num,
                       GPIO_LEVEL_LOW);
  }
}
