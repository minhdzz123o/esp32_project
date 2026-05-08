# ESP32 Project

## Mô tả
Template dự án ESP32 với cấu trúc bài bản, sử dụng PlatformIO + Arduino Framework.

## Cấu trúc thư mục

```
esp32_project/
├── src/
│   ├── main.cpp              # Entry point chính
│   ├── wifi_manager.cpp      # Quản lý kết nối WiFi
│   └── sensor_handler.cpp    # Đọc dữ liệu cảm biến
├── include/
│   ├── config.h              # Cấu hình tập trung
│   ├── wifi_manager.h        # Header WiFi
│   └── sensor_handler.h      # Header Sensor
├── docs/
│   └── pinout.md             # Sơ đồ chân
├── lib/                      # Thư viện riêng
├── test/                     # Unit test
├── platformio.ini            # Cấu hình PlatformIO
└── README.md
```

## Cách sử dụng

### 1. Cài đặt
- Cài [VS Code](https://code.visualstudio.com/)
- Cài Extension [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)

### 2. Cấu hình
Sửa file `include/config.h`:
- Thay `WIFI_SSID` và `WIFI_PASSWORD` bằng thông tin WiFi của bạn
- Cập nhật pin definitions theo mạch thực tế

### 3. Build & Upload
```bash
# Build project
pio run

# Upload lên ESP32
pio run --target upload

# Mở Serial Monitor
pio device monitor
```

### 4. Thêm thư viện
Sửa `platformio.ini`, thêm vào `lib_deps`:
```ini
lib_deps =
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.0
```

## Nguyên tắc code

1. **Non-blocking**: Dùng `millis()` thay vì `delay()`
2. **Modular**: Mỗi file = 1 chức năng
3. **Config tập trung**: Mọi hằng số đặt trong `config.h`
4. **Debug có cấp độ**: Dùng `DEBUG_PRINTLN()` macro
