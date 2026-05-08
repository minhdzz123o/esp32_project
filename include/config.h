/**
 * @file config.h
 * @brief Cấu hình tập trung cho toàn bộ dự án
 *
 * Tất cả hằng số, thông số cấu hình được đặt ở đây
 * để dễ quản lý và thay đổi.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// WiFi Configuration
// ============================================
#define WIFI_SSID         "YOUR_WIFI_SSID"
#define WIFI_PASSWORD     "YOUR_WIFI_PASSWORD"
#define WIFI_TIMEOUT_MS   10000    // Timeout kết nối WiFi (ms)
#define WIFI_RETRY_DELAY  5000     // Delay giữa các lần retry (ms)

// ============================================
// Pin Definitions
// ============================================
#define LED_BUILTIN_PIN   2        // LED onboard ESP32
#define SENSOR_PIN        34       // Chân đọc cảm biến (ADC)
#define BUTTON_PIN        0        // Nút BOOT trên ESP32

// Ví dụ thêm pin:
// #define RELAY_PIN      26
// #define BUZZER_PIN     27
// #define SDA_PIN        21
// #define SCL_PIN        22

// ============================================
// Timing Configuration
// ============================================
#define SENSOR_READ_INTERVAL   2000   // Đọc sensor mỗi 2 giây
#define LED_BLINK_INTERVAL     1000   // Nhấp nháy LED mỗi 1 giây
#define SERIAL_BAUD_RATE       115200

// ============================================
// Debug Configuration
// ============================================
#define DEBUG_ENABLED     true

#if DEBUG_ENABLED
  #define DEBUG_PRINT(x)      Serial.print(x)
  #define DEBUG_PRINTLN(x)    Serial.println(x)
  #define DEBUG_PRINTF(...)   Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// ============================================
// Project Info
// ============================================
#define PROJECT_NAME      "ESP32 Project"
#define PROJECT_VERSION   "1.0.0"

#endif // CONFIG_H
