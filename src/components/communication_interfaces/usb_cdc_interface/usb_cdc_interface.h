// Copyright 2024 MACKI

#pragma once

#include "usb_cdc_driver.h"

#include "macki_log.h"

/*!
 * @brief Initialize USB CDC interface
 * Default struct values are set in usb_cdc_interface.c file
 */
void usb_cdc_interface_init();

/*!
 * @brief Register callback for received data
 *
 * @param callback Callback function
 */
void usb_cdc_register_rx_callback(tusb_cdcacm_callback_t callback);

/*!
 * @brief Send data over USB CDC
 *
 * @param data Data to send
 * @param length Data length
 */
void usb_cdc_interface_send_data(uint8_t *data, size_t length);

/*!
 * @brief To be implemented on log implementation
 */
void usb_cdc_on_log_received(char *data, size_t length);

/*!
 * @brief Callback for received data
 *
 * @param itf Interface number (unused - only for compatibility)
 * @param event Event (TBD)
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);

/*!
 * @brief Callback for line state changed
 *
 * @param itf Interface number (unused - only for compatibility)
 * @param event Event (TBD)
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event);
