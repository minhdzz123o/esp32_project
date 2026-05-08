/**
 * @file sensor_handler.h
 * @brief Quản lý đọc dữ liệu từ cảm biến
 */

#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>
#include "config.h"

/**
 * @brief Khởi tạo các chân cảm biến
 */
void sensor_init();

/**
 * @brief Đọc giá trị analog từ cảm biến
 * @return Giá trị ADC (0-4095 cho ESP32 12-bit)
 */
int sensor_read_raw();

/**
 * @brief Đọc giá trị đã convert sang voltage
 * @return Điện áp (0.0 - 3.3V)
 */
float sensor_read_voltage();

/**
 * @brief Kiểm tra và đọc sensor theo interval (non-blocking)
 * Gọi trong loop(), chỉ đọc khi đến thời điểm
 * @param value Con trỏ nhận giá trị đọc được
 * @return true nếu có giá trị mới
 */
bool sensor_update(float* value);

#endif // SENSOR_HANDLER_H
