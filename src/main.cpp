/**
 * @file main.cpp
 * @brief Entry point - ESP32 Project
 *
 * File chính điều phối toàn bộ hệ thống.
 * Chỉ gọi các hàm từ module khác, KHÔNG viết logic phức tạp ở đây.
 */

#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "sensor_handler.h"

// ============================================
// Biến toàn cục
// ============================================
unsigned long lastLedToggle = 0;
bool ledState = false;

// ============================================
// Setup - Chạy 1 lần khi khởi động
// ============================================
void setup() {
    // --- Serial Monitor ---
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);  // Chờ Serial ổn định

    Serial.println();
    Serial.println("╔══════════════════════════════════╗");
    Serial.printf( "║  %s v%s      ║\n", PROJECT_NAME, PROJECT_VERSION);
    Serial.println("║  Board: ESP32 DevKit             ║");
    Serial.println("╚══════════════════════════════════╝");
    Serial.println();

    // --- GPIO Setup ---
    pinMode(LED_BUILTIN_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    DEBUG_PRINTLN("[OK] GPIO initialized");

    // --- Sensor Setup ---
    sensor_init();

    // --- WiFi Setup ---
    if (wifi_init()) {
        DEBUG_PRINTLN("[OK] System ready!");
    } else {
        DEBUG_PRINTLN("[WARN] Running in OFFLINE mode");
    }

    Serial.println();
    Serial.println("========== SYSTEM RUNNING ==========");
    Serial.println();
}

// ============================================
// Loop - Chạy liên tục
// ============================================
void loop() {
    // --- 1. Kiểm tra WiFi ---
    wifi_check_connection();

    // --- 2. Đọc sensor (non-blocking) ---
    float sensorValue;
    if (sensor_update(&sensorValue)) {
        // Có dữ liệu mới từ sensor
        DEBUG_PRINTF("[DATA] Sensor = %.2f V\n", sensorValue);

        // TODO: Gửi dữ liệu lên server / MQTT
        // TODO: Hiển thị lên LCD
        // TODO: Kiểm tra ngưỡng cảnh báo
    }

    // --- 3. Blink LED (non-blocking) ---
    unsigned long now = millis();
    if (now - lastLedToggle >= LED_BLINK_INTERVAL) {
        lastLedToggle = now;
        ledState = !ledState;
        digitalWrite(LED_BUILTIN_PIN, ledState);
    }

    // --- 4. Kiểm tra nút nhấn ---
    if (digitalRead(BUTTON_PIN) == LOW) {
        DEBUG_PRINTLN("[BTN] Button pressed!");
        // TODO: Xử lý sự kiện nút nhấn

        // Debounce đơn giản
        delay(200);
    }
}
