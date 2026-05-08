# ESP32 Pin Mapping

## Board: ESP32 DevKit V1

### Sơ đồ chân đang sử dụng

| GPIO | Chức năng      | Hướng   | Ghi chú                    |
|------|---------------|---------|----------------------------|
| 2    | LED Onboard   | OUTPUT  | LED xanh tích hợp trên board |
| 0    | Button BOOT   | INPUT   | Nút BOOT, có pull-up nội   |
| 34   | Sensor (ADC)  | INPUT   | Chỉ input, không có pull-up |

### Chân có thể sử dụng thêm

| GPIO | Gợi ý sử dụng      | Lưu ý                          |
|------|--------------------|---------------------------------|
| 21   | I2C SDA            | Mặc định cho I2C                |
| 22   | I2C SCL            | Mặc định cho I2C                |
| 18   | SPI SCK            | VSPI                            |
| 19   | SPI MISO           | VSPI                            |
| 23   | SPI MOSI           | VSPI                            |
| 5    | SPI CS             | VSPI                            |
| 16   | UART2 RX           | Serial2                         |
| 17   | UART2 TX           | Serial2                         |
| 25   | DAC1               | Analog output                   |
| 26   | DAC2 / Relay       | Analog output hoặc digital      |
| 27   | Buzzer / PWM       | Hỗ trợ PWM                     |
| 32   | ADC / Touch        | ADC1_CH4                        |
| 33   | ADC / Touch        | ADC1_CH5                        |

### ⚠️ Chân cần TRÁNH sử dụng

| GPIO   | Lý do                                        |
|--------|----------------------------------------------|
| 6-11   | Kết nối SPI Flash nội - KHÔNG DÙNG           |
| 1      | TX0 - Serial debug                           |
| 3      | RX0 - Serial debug                           |
| 12     | Boot fail nếu pulled HIGH lúc khởi động      |
| 34-39  | Chỉ INPUT, không có pull-up/pull-down nội    |
