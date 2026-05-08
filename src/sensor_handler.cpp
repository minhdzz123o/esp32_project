/**
 * @file sensor_handler.cpp
 * @brief Implementation đọc dữ liệu cảm biến
 */

#include "sensor_handler.h"

// Thời điểm đọc sensor lần cuối
static unsigned long lastReadTime = 0;

void sensor_init() {
    pinMode(SENSOR_PIN, INPUT);

    // Cấu hình ADC (tùy chọn)
    analogSetAttenuation(ADC_11db);    // Full range: 0-3.3V
    analogSetWidth(12);                 // 12-bit resolution (0-4095)

    DEBUG_PRINTLN("[OK] Sensor initialized");
    DEBUG_PRINTF("     Pin: GPIO%d\n", SENSOR_PIN);
    DEBUG_PRINTF("     Read interval: %d ms\n", SENSOR_READ_INTERVAL);
}

int sensor_read_raw() {
    int value = analogRead(SENSOR_PIN);
    DEBUG_PRINTF("[SENSOR] Raw value: %d\n", value);
    return value;
}

float sensor_read_voltage() {
    int raw = analogRead(SENSOR_PIN);
    float voltage = (raw / 4095.0) * 3.3;
    DEBUG_PRINTF("[SENSOR] Voltage: %.2f V\n", voltage);
    return voltage;
}

bool sensor_update(float* value) {
    unsigned long now = millis();

    if (now - lastReadTime >= SENSOR_READ_INTERVAL) {
        lastReadTime = now;
        *value = sensor_read_voltage();
        return true;  // Có dữ liệu mới
    }

    return false;  // Chưa đến thời điểm đọc
}
