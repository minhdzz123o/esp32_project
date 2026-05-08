/**
 * @file wifi_manager.h
 * @brief Quản lý kết nối WiFi cho ESP32
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "config.h"

/**
 * @brief Khởi tạo và kết nối WiFi
 * @return true nếu kết nối thành công, false nếu timeout
 */
bool wifi_init();

/**
 * @brief Kiểm tra trạng thái kết nối WiFi
 * @return true nếu đang kết nối
 */
bool wifi_is_connected();

/**
 * @brief Tự động reconnect nếu mất kết nối
 * Gọi hàm này trong loop() để duy trì kết nối
 */
void wifi_check_connection();

/**
 * @brief Lấy địa chỉ IP hiện tại
 * @return String chứa IP address
 */
String wifi_get_ip();

#endif // WIFI_MANAGER_H
