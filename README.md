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

1. **Non-blocking**: Dùng `millis()` thay vì `delay()`┌─────────────────────────────────────────┐
│         MAIN.CPP (Điều phối chính)      │
│         - setup() chạy 1 lần            │
│         - loop() chạy liên tục          │
└────────┬───────────────┬────────┬──────┘
         │               │        │
         ▼               ▼        ▼
    ┌─────────┐  ┌────────────┐ ┌──────────────┐
    │ CONFIG  │  │   SENSOR   │ │    WIFI      │
    │  .h     │  │  HANDLER   │ │   MANAGER    │
    │         │  │  .cpp/.h   │ │   .cpp/.h    │
    │ •Pin    │  │ •đọc ADC   │ │ •kết nối WiFi│
    │ •Timing │  │ •timeout   │ │ •reconnect   │
    │ •Tham số│  │ •interval  │ │ •kiểm tra IP │
    └─────────┘  └────────────┘ └──────────────┘
2. **Modular**: Mỗi file = 1 chức năng
3. **Config tập trung**: Mọi hằng số đặt trong `config.h`
4. **Debug có cấp độ**: Dùng `DEBUG_PRINTLN()` macro// Lần lặp đầu tiên (t = 0ms)




1️⃣ BUILD (Compile)
┌──────────────────────────────────────┐
│ PlatformIO quét thư mục src/ & lib/  │
├──────────────────────────────────────┤
│ ✓ Biên dịch main.cpp                 │
│ ✓ Biên dịch sensor_handler.cpp       │
│ ✓ Biên dịch wifi_manager.cpp         │
│ ✓ Biên dịch tất cả file .h/.cpp      │
└────────────┬─────────────────────────┘
             ▼
        ┌──────────────────┐
        │ Linker (Liên kết)│
        │ Kết hợp tất cả   │
        │ → firmware.bin   │
        └────────┬─────────┘
                 ▼
2️⃣ UPLOAD
    ┌─────────────────────────┐
    │ Upload firmware.bin     │
    │ vào ESP32 qua USB       │
    └─────────────────────────┘sensor_update() → lastReadTime = 0, chưa đến 2 giây → return false

// Lần lặp 2000ms sau
sensor_update() → now - lastReadTime = 2000ms ≥ 2000ms?
                  ✓ YES! → Đọc ADC, convert voltage, update lastReadTime
                  ✓ return true → in "[DATA]"

// Lần lặp 2500ms sau  
sensor_update() → now - lastReadTime = 500ms < 2000ms → return false
                → loop() tiếp tục, không cần đọc lại

📊 Kiểm Chứng
Khi bạn chạy Build (esp32dev), log sẽ hiện:

Compiling .pio/build/esp32dev/src/main.cpp.o
Compiling .pio/build/esp32dev/src/sensor_handler.cpp.o
Compiling .pio/build/esp32dev/src/wifi_manager.cpp.o
Linking .pio/build/esp32dev/firmware.elf
Building .pio/build/esp32dev/firmware.bin