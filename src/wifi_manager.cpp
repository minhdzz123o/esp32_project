/**
 * @file wifi_manager.cpp
 * @brief Implementation quản lý kết nối WiFi
 */

#include "wifi_manager.h"

// Thời điểm lần cuối thử reconnect
static unsigned long lastReconnectAttempt = 0;

bool wifi_init() {
    DEBUG_PRINTLN("=============================");
    DEBUG_PRINTLN("  WiFi Manager - Connecting");
    DEBUG_PRINTLN("=============================");
    DEBUG_PRINTF("SSID: %s\n", WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > WIFI_TIMEOUT_MS) {
            DEBUG_PRINTLN("\n[ERROR] WiFi connection TIMEOUT!");
            return false;
        }
        DEBUG_PRINT(".");
        delay(500);
    }

    DEBUG_PRINTLN();
    DEBUG_PRINTLN("[OK] WiFi connected!");
    DEBUG_PRINTF("[OK] IP Address: %s\n", WiFi.localIP().toString().c_str());
    DEBUG_PRINTF("[OK] Signal Strength (RSSI): %d dBm\n", WiFi.RSSI());

    return true;
}

bool wifi_is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

void wifi_check_connection() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > WIFI_RETRY_DELAY) {
            lastReconnectAttempt = now;
            DEBUG_PRINTLN("[WARN] WiFi disconnected! Attempting reconnect...");

            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

            // Chờ tối đa 5 giây
            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
                delay(100);
            }

            if (WiFi.status() == WL_CONNECTED) {
                DEBUG_PRINTLN("[OK] WiFi reconnected!");
                DEBUG_PRINTF("[OK] IP: %s\n", WiFi.localIP().toString().c_str());
            } else {
                DEBUG_PRINTLN("[ERROR] Reconnect failed. Will retry...");
            }
        }
    }
}

String wifi_get_ip() {
    if (wifi_is_connected()) {
        return WiFi.localIP().toString();
    }
    return "Not connected";
}
