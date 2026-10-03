# Quy trình dự án & Tư duy hệ thống

> Tài liệu này giải thích dự án **bằng sơ đồ**: hệ thống được chia thế nào, chạy ra sao,
> và nhóm làm việc theo quy trình nào. Đọc theo thứ tự từ trên xuống — mỗi phần là một
> "góc nhìn" khác nhau về cùng một hệ thống.
>
> Lộ trình và bảng tiến độ để làm theo nằm ở [`roadmap.md`](roadmap.md).

Sơ đồ viết bằng [Mermaid](https://mermaid.js.org/) — GitHub và VS Code (extension
*Markdown Preview Mermaid Support*) tự vẽ ra hình.

## Mục lục

1. [Bản đồ các góc nhìn](#1-bản-đồ-các-góc-nhìn)
2. [Tư duy hệ thống: V-Model](#2-tư-duy-hệ-thống-v-model)
3. [Kiến trúc phân lớp của firmware](#3-kiến-trúc-phân-lớp-của-firmware)
4. [Luồng khởi động `setup()`](#4-luồng-khởi-động-setup)
5. [Luồng chạy liên tục `loop()`](#5-luồng-chạy-liên-tục-loop)
6. [Máy trạng thái WiFi](#6-máy-trạng-thái-wifi)
7. [Định thời non-blocking với `millis()`](#7-định-thời-non-blocking-với-millis)
8. [Pipeline Build → Upload → Chạy](#8-pipeline-build--upload--chạy)
9. [Vòng lặp phát triển hằng ngày](#9-vòng-lặp-phát-triển-hằng-ngày)
10. [Quy trình Git cho nhóm 4 người](#10-quy-trình-git-cho-nhóm-4-người)
11. [Quy trình thêm một tính năng mới](#11-quy-trình-thêm-một-tính-năng-mới)
12. [Cây quyết định debug phần cứng](#12-cây-quyết-định-debug-phần-cứng)

---

## 1. Bản đồ các góc nhìn

Một hệ thống nhúng nên được nhìn từ nhiều góc. Mỗi góc trả lời một câu hỏi khác nhau:

```mermaid
flowchart LR
    SYS(("Hệ thống<br/>ESP32"))
    SYS --> S["<b>Cấu trúc</b><br/>Gồm những khối nào?<br/>→ Mục 3"]
    SYS --> B["<b>Hành vi</b><br/>Chạy theo thứ tự nào?<br/>→ Mục 4, 5"]
    SYS --> ST["<b>Trạng thái</b><br/>Đang ở chế độ nào?<br/>→ Mục 6"]
    SYS --> T["<b>Thời gian</b><br/>Việc gì xảy ra khi nào?<br/>→ Mục 7"]
    SYS --> P["<b>Quy trình</b><br/>Con người làm việc ra sao?<br/>→ Mục 8–12"]
```

| Góc nhìn   | Loại sơ đồ             | Câu hỏi trả lời                                 |
|------------|------------------------|-------------------------------------------------|
| Cấu trúc   | Block diagram          | Module nào phụ thuộc module nào?                |
| Hành vi    | Flowchart              | Code chạy theo nhánh nào?                        |
| Trạng thái | State machine          | Hệ thống phản ứng thế nào khi có sự kiện?       |
| Thời gian  | Sequence / timeline    | Có chỗ nào **chặn** (blocking) CPU không?       |
| Quy trình  | Workflow / Gantt       | Ai làm gì, theo thứ tự nào, khi nào xong?       |

---

## 2. Tư duy hệ thống: V-Model

Ngành ô tô (ASPICE, ISO 26262) phát triển phần mềm theo **V-Model**: nhánh trái đi
xuống là *thiết kế*, nhánh phải đi lên là *kiểm chứng*. Mỗi tầng bên trái có một tầng
kiểm thử tương ứng bên phải.

```mermaid
flowchart TB
    subgraph L["⬇ THIẾT KẾ"]
        direction TB
        R1["1 · Yêu cầu hệ thống<br/><i>Hệ thống phải làm gì?</i>"]
        R2["2 · Kiến trúc<br/><i>Chia thành module nào?</i>"]
        R3["3 · Thiết kế chi tiết<br/><i>API, chân GPIO, timing</i>"]
    end
    C["4 · Viết code<br/>src/ · include/"]
    subgraph R["⬆ KIỂM CHỨNG"]
        direction BT
        T3["5 · Unit test<br/><i>Từng hàm đúng?</i>"]
        T2["6 · Integration test<br/><i>Ghép module chạy đúng?</i>"]
        T1["7 · System test<br/><i>Đáp ứng yêu cầu ban đầu?</i>"]
    end
    R1 --> R2 --> R3 --> C --> T3 --> T2 --> T1
    R1 <-. "kiểm chứng" .-> T1
    R2 <-. "kiểm chứng" .-> T2
    R3 <-. "kiểm chứng" .-> T3
```

**Áp vào dự án này:**

| Tầng | Sản phẩm trong repo | Kiểm chứng bằng |
|------|---------------------|-----------------|
| Yêu cầu | `README.md` — mô tả, `roadmap.md` — mục tiêu từng giai đoạn | System test: chạy board, đối chiếu từng yêu cầu |
| Kiến trúc | Mục 3 của tài liệu này | Integration test: WiFi + sensor + LED chạy cùng lúc |
| Thiết kế chi tiết | `include/*.h`, `include/config.h`, `docs/pinout.md` | Unit test trong `test/` |
| Code | `src/*.cpp` | Build không lỗi, không warning |

> 💡 **Quy tắc vàng:** viết yêu cầu và tiêu chí kiểm thử **trước** khi viết code.
> Nếu không nói được "thế nào là xong", thì chưa nên bắt đầu.

---

## 3. Kiến trúc phân lớp của firmware

*(Vẽ lại từ sơ đồ ASCII "MAIN.CPP điều phối chính" trong README.)*

```mermaid
flowchart TB
    subgraph APP["🟦 Lớp ứng dụng — chỉ điều phối"]
        MAIN["<b>main.cpp</b><br/>setup() chạy 1 lần<br/>loop() chạy liên tục"]
    end

    subgraph MOD["🟩 Lớp module — mỗi file một chức năng"]
        SEN["<b>sensor_handler</b><br/>• sensor_init()<br/>• sensor_update()<br/>• sensor_read_voltage()"]
        WIFI["<b>wifi_manager</b><br/>• wifi_init()<br/>• wifi_check_connection()<br/>• wifi_get_ip()"]
    end

    CFG[["<b>config.h</b><br/>Pin · Timing · Debug macro"]]

    subgraph HAL["🟨 Lớp nền tảng — Arduino core / ESP-IDF"]
        ADC["analogRead()"]
        WL["WiFi.h"]
        GPIO["pinMode()<br/>digitalWrite()<br/>millis()"]
    end

    HW["⬛ <b>Phần cứng ESP32</b><br/>GPIO34 cảm biến · GPIO2 LED · GPIO0 nút BOOT · Radio WiFi"]

    MAIN --> SEN
    MAIN --> WIFI
    MAIN --> GPIO
    SEN --> ADC
    WIFI --> WL
    ADC --> HW
    WL --> HW
    GPIO --> HW
    CFG -.-> MAIN
    CFG -.-> SEN
    CFG -.-> WIFI
```

**Ba luật của kiến trúc phân lớp:**

1. **Mũi tên chỉ đi xuống.** `main.cpp` gọi module, module **không bao giờ** gọi ngược `main.cpp`.
2. **Module không biết nhau.** `sensor_handler` không `#include "wifi_manager.h"`. Nếu cần
   phối hợp (ví dụ gửi dữ liệu cảm biến qua WiFi) thì `main.cpp` làm cầu nối.
3. **Mọi hằng số nằm ở `config.h`.** Đổi chân, đổi chu kỳ — chỉ sửa một chỗ.

> 🚗 Đây chính là ý tưởng của **AUTOSAR**: Application → RTE → Basic Software → MCU.
> Dự án nhỏ này là phiên bản thu gọn của cùng tư duy đó.

---

## 4. Luồng khởi động `setup()`

```mermaid
flowchart TD
    A(["⚡ Cấp nguồn / Reset"]) --> B["Serial.begin(115200)<br/>delay(1000) chờ Serial ổn định"]
    B --> C["In banner<br/>PROJECT_NAME + PROJECT_VERSION"]
    C --> D["GPIO init<br/>LED GPIO2 = OUTPUT<br/>Nút GPIO0 = INPUT_PULLUP"]
    D --> E["sensor_init()<br/>ADC 11 dB → 0–3.3 V, 12-bit"]
    E --> F["wifi_init()<br/>WiFi.begin(SSID, PASS)"]
    F --> G{"Kết nối được<br/>trong 10 s?"}
    G -- Có --> H["[OK] System ready!<br/>In IP + RSSI"]
    G -- "Không (timeout)" --> I["[WARN] Chạy OFFLINE<br/>loop() sẽ tự thử lại"]
    H --> J(["▶ Chuyển sang loop()"])
    I --> J

    classDef block fill:#fde2e1,stroke:#c0392b,color:#000
    class F,G block
```

> 🟥 Ô đỏ = đoạn **chặn** CPU (blocking). Trong `setup()` chấp nhận được vì chỉ chạy
> một lần, nhưng thời gian khởi động có thể kéo dài tới ~11 giây khi không có WiFi.

---

## 5. Luồng chạy liên tục `loop()`

`loop()` là một **super-loop**: 4 tác vụ được kiểm tra lần lượt, mỗi tác vụ tự hỏi
"đã đến lúc làm chưa?" rồi trả quyền cho tác vụ tiếp theo.

```mermaid
flowchart TD
    START(["🔁 Đầu vòng loop()"]) --> W1

    subgraph T1["① WiFi — wifi_check_connection()"]
        W1{"WiFi đang<br/>kết nối?"}
        W2{"Đã qua 5 s từ<br/>lần thử trước?"}
        W3["⚠ Reconnect<br/>while chờ tối đa 5 s"]
        W1 -- Không --> W2
        W2 -- Có --> W3
    end

    subgraph T2["② Cảm biến — sensor_update()"]
        S1{"now − lastReadTime<br/>≥ 2000 ms?"}
        S2["Đọc ADC → đổi ra Volt<br/>lastReadTime = now"]
        S3["In [DATA] Sensor = x.xx V"]
        S1 -- Có --> S2 --> S3
    end

    subgraph T3["③ LED — nháy không chặn"]
        L1{"now − lastLedToggle<br/>≥ 1000 ms?"}
        L2["Đảo trạng thái LED"]
        L1 -- Có --> L2
    end

    subgraph T4["④ Nút nhấn BOOT"]
        B1{"GPIO0 == LOW?"}
        B2["In [BTN]<br/>⚠ delay(200) chống dội"]
        B1 -- Có --> B2
    end

    W1 -- Có --> S1
    W2 -- Chưa --> S1
    W3 --> S1
    S1 -- Chưa --> L1
    S3 --> L1
    L1 -- Chưa --> B1
    L2 --> B1
    B1 -- Không --> END(["↩ Hết vòng — Arduino gọi lại loop() ngay"])
    B2 --> END

    classDef block fill:#fde2e1,stroke:#c0392b,color:#000
    class W3,B2 block
```

### 🔍 Tư duy hệ thống: tìm "điểm nghẽn"

Nguyên tắc số 1 của dự án là **non-blocking**, nhưng sơ đồ trên lộ ra 2 chỗ vi phạm (ô đỏ):

| Vị trí | Chặn bao lâu | Hậu quả |
|--------|--------------|---------|
| `wifi_check_connection()` — vòng `while` chờ reconnect | tới **5 s**, lặp lại mỗi ~5 s khi mất WiFi | Cảm biến lỡ nhịp đọc 2 s, LED đứng hình, nút nhấn không phản hồi |
| `delay(200)` sau khi nhấn nút | 200 ms mỗi lần, **lặp liên tục khi giữ nút** | Giữ nút sẽ in `[BTN]` liên tục ~5 lần/giây |

Đây là ví dụ điển hình vì sao phải vẽ sơ đồ: đọc từng file thì thấy ổn, nhưng nhìn
**toàn hệ thống** mới thấy một module làm "đói" các module khác. Cách sửa nằm trong
**Giai đoạn P1** của [`roadmap.md`](roadmap.md).

---

## 6. Máy trạng thái WiFi

Hành vi hiện tại của `wifi_manager.cpp`, vẽ dưới dạng máy trạng thái:

```mermaid
stateDiagram-v2
    [*] --> Connecting : wifi_init()
    Connecting --> Connected : WL_CONNECTED
    Connecting --> Disconnected : quá 10 s (WIFI_TIMEOUT_MS)

    Connected --> Disconnected : mất sóng / router tắt
    Disconnected --> Disconnected : chưa đủ 5 s (WIFI_RETRY_DELAY)
    Disconnected --> Reconnecting : đủ 5 s từ lần thử trước

    Reconnecting --> Connected : thành công trong 5 s
    Reconnecting --> Disconnected : thất bại → chờ thử lại

    note right of Reconnecting
        ⚠ Trạng thái này đang
        CHẶN loop() tới 5 s
    end note
```

**Mục tiêu P1** — tách `Reconnecting` thành trạng thái *không chặn*: mỗi lần `loop()`
gọi vào chỉ kiểm tra `WiFi.status()` một lần rồi trả về ngay.

```mermaid
stateDiagram-v2
    [*] --> Connecting : wifi_init()
    Connecting --> Connected : WL_CONNECTED
    Connecting --> WaitRetry : timeout

    Connected --> WaitRetry : mất kết nối
    WaitRetry --> Connecting : đủ WIFI_RETRY_DELAY → WiFi.begin()

    note right of Connecting
        Mỗi lần loop() gọi vào:
        chỉ đọc WiFi.status() rồi return
        → không còn vòng while
    end note
```

---

## 7. Định thời non-blocking với `millis()`

*(Vẽ lại từ ví dụ "Lần lặp đầu tiên / 2000 ms sau / 2500 ms sau" trong README.)*

Ý tưởng: thay vì **ngủ** chờ (`delay`), mỗi vòng lặp **nhìn đồng hồ** rồi quyết định.

```mermaid
sequenceDiagram
    autonumber
    participant L as loop()
    participant S as sensor_update()
    participant C as millis()
    participant A as ADC GPIO34

    Note over L,A: lastReadTime = 0 lúc khởi động.<br/>setup() đã tốn ≥ 1000 ms (delay + chờ WiFi)

    L->>S: gọi lần đầu (ví dụ t = 3200 ms)
    S->>C: now = 3200
    Note right of S: 3200 − 0 = 3200 ≥ 2000 ✓
    S->>A: analogRead()
    A-->>S: raw = 2048
    S-->>L: true, 1.65 V → in [DATA]

    L->>S: vòng sau (t = 3201 ms)
    S->>C: now = 3201
    Note right of S: 3201 − 3200 = 1 < 2000 ✗
    S-->>L: false → làm việc khác

    L->>S: ... hàng nghìn vòng sau (t = 5200 ms)
    S->>C: now = 5200
    Note right of S: 5200 − 3200 = 2000 ≥ 2000 ✓
    S->>A: analogRead()
    S-->>L: true → in [DATA]
```

> ⚠ **Đính chính README cũ:** lần gọi đầu tiên **thường đọc ngay** chứ không trả `false`,
> vì `setup()` đã chạy hơn 1 giây (`delay(1000)` + chờ WiFi) nên `millis() − 0` đã vượt 2000 ms
> trong đa số trường hợp.
>
> 💡 Phép trừ `now - lastReadTime` với kiểu `unsigned long` vẫn đúng cả khi `millis()`
> tràn số sau ~49,7 ngày — đây là lý do **không** viết `now >= lastReadTime + 2000`.

---

## 8. Pipeline Build → Upload → Chạy

*(Vẽ lại từ sơ đồ "1️⃣ BUILD / 2️⃣ UPLOAD" trong README.)*

```mermaid
flowchart LR
    subgraph BUILD["1️⃣ BUILD — pio run"]
        direction TB
        SRC["src/*.cpp<br/>include/*.h<br/>lib/"] --> CC["Compiler<br/>xtensa-esp32-elf-g++"]
        CC --> OBJ["main.cpp.o<br/>sensor_handler.cpp.o<br/>wifi_manager.cpp.o"]
        FW["Arduino core<br/>+ lib_deps"] --> LD
        OBJ --> LD["Linker"]
        LD --> ELF["firmware.elf<br/><i>có debug symbol</i>"]
        ELF --> BIN["firmware.bin<br/><i>ảnh nạp flash</i>"]
    end

    subgraph UP["2️⃣ UPLOAD — pio run -t upload"]
        direction TB
        ESPT["esptool.py<br/>921600 baud qua USB"] --> FLASH["Ghi vào<br/>SPI Flash"]
    end

    subgraph RUN["3️⃣ CHẠY — pio device monitor"]
        direction TB
        BOOT["Bootloader<br/>→ setup() → loop()"] --> MON["Serial Monitor<br/>115200 baud<br/>+ exception decoder"]
    end

    BIN --> ESPT
    FLASH --> BOOT
    ELF -. "giải mã<br/>backtrace khi crash" .-> MON
```

Log build tương ứng:

```text
Compiling .pio/build/esp32dev/src/main.cpp.o
Compiling .pio/build/esp32dev/src/sensor_handler.cpp.o
Compiling .pio/build/esp32dev/src/wifi_manager.cpp.o
Linking .pio/build/esp32dev/firmware.elf
Building .pio/build/esp32dev/firmware.bin
```

---

## 9. Vòng lặp phát triển hằng ngày

```mermaid
flowchart LR
    A["✏️ Sửa code"] --> B["🔨 Build<br/>pio run"]
    B -- "Lỗi compile" --> A
    B -- OK --> C["⬆️ Upload<br/>pio run -t upload"]
    C -- "Không thấy cổng COM" --> H["Kiểm tra cáp data,<br/>driver CP210x/CH340,<br/>giữ nút BOOT khi nạp"]
    H --> C
    C --> D["🖥️ Monitor<br/>pio device monitor"]
    D --> E{"Hành vi đúng<br/>như mong đợi?"}
    E -- Không --> F["🐞 Debug<br/>đọc log · thêm DEBUG_PRINTF<br/>· đo chân bằng đồng hồ"]
    F --> A
    E -- Có --> G["✅ git commit<br/>một thay đổi nhỏ, rõ ràng"]
```

> 💡 Vòng lặp càng ngắn càng tốt. Sửa ít → build → test ngay. Đừng viết 300 dòng rồi
> mới nạp lần đầu.

---

## 10. Quy trình Git cho nhóm 4 người

### 10.1 Mô hình nhánh

```mermaid
gitGraph
    commit id: "first commit"
    commit id: "README"
    branch fix/wifi-nonblocking
    checkout fix/wifi-nonblocking
    commit id: "wifi state machine"
    commit id: "test rút router"
    checkout main
    branch feat/mqtt
    checkout feat/mqtt
    commit id: "thêm PubSubClient"
    checkout main
    merge fix/wifi-nonblocking tag: "P1 xong"
    checkout feat/mqtt
    merge main
    commit id: "publish JSON"
    checkout main
    merge feat/mqtt
    commit id: "v1.1.0" tag: "v1.1.0"
```

- `main` **luôn build được và nạp được**. Không ai commit thẳng vào `main`.
- Mỗi việc = 1 nhánh: `feat/...` (tính năng), `fix/...` (sửa lỗi), `docs/...` (tài liệu).
- Trước khi merge, kéo `main` mới nhất vào nhánh của mình để giải quyết xung đột sớm.

### 10.2 Vòng đời một công việc

```mermaid
flowchart LR
    I["📋 Issue<br/>mô tả + tiêu chí xong"] --> BR["🌿 Tạo nhánh<br/>feat/ten-viec"]
    BR --> CM["💾 Commit nhỏ<br/>feat(wifi): ..."]
    CM --> PU["⬆️ git push"]
    PU --> PR["🔀 Pull Request<br/>ghi Closes #số-issue"]
    PR --> RV{"Review<br/>≥ 1 thành viên<br/>+ build OK?"}
    RV -- "Cần sửa" --> CM
    RV -- Duyệt --> MG["✅ Merge vào main"]
    MG --> UP["☑️ Tick checkbox<br/>trong roadmap.md"]
```

### 10.3 Quy ước commit message

| Tiền tố | Dùng khi | Ví dụ |
|---------|----------|-------|
| `feat:` | Thêm tính năng | `feat(mqtt): publish sensor data dạng JSON` |
| `fix:` | Sửa lỗi | `fix(wifi): bỏ vòng while chặn khi reconnect` |
| `refactor:` | Đổi cấu trúc, không đổi hành vi | `refactor(sensor): dùng chung sensor_read_raw()` |
| `docs:` | Tài liệu | `docs: cập nhật pinout cho relay` |
| `test:` | Thêm/sửa test | `test: unit test bộ lọc trung bình trượt` |

---

## 11. Quy trình thêm một tính năng mới

Trước khi gõ code cho bất kỳ tính năng nào, đi qua sơ đồ này:

```mermaid
flowchart TD
    A(["💡 Yêu cầu mới<br/>vd: gửi dữ liệu lên MQTT"]) --> Q1{"Thuộc module<br/>đã có không?"}
    Q1 -- Có --> M1["Thêm hàm vào<br/>module đó (.h + .cpp)"]
    Q1 -- Không --> M2["Tạo module mới<br/>include/xxx.h + src/xxx.cpp"]
    M1 --> K
    M2 --> K["Trả lời 5 câu hỏi hệ thống ⬇"]
    K --> C1["Hằng số mới → config.h<br/>Chân mới → docs/pinout.md"]
    C1 --> C2["Viết theo mẫu:<br/>xxx_init() trong setup()<br/>xxx_update() trong loop() — không chặn"]
    C2 --> C3["Gọi từ main.cpp<br/>main làm cầu nối giữa các module"]
    C3 --> C4["Test: trường hợp bình thường<br/>+ trường hợp lỗi"]
    C4 --> C5(["📝 Cập nhật README / roadmap → PR"])
```

**5 câu hỏi hệ thống** (rút gọn từ tư duy FMEA trong ô tô):

| # | Câu hỏi | Ví dụ với MQTT |
|---|---------|----------------|
| 1 | **Đầu vào** là gì? | Giá trị Volt từ `sensor_update()` |
| 2 | **Đầu ra** là gì? | Gói JSON tới topic `esp32/<id>/sensor` |
| 3 | **Trạng thái** cần nhớ? | Đã kết nối broker chưa, lần gửi cuối |
| 4 | **Thời gian**: bao lâu làm 1 lần, có chặn không? | Mỗi 2 s, `client.loop()` phải trả về ngay |
| 5 | **Khi lỗi** thì sao? | Mất WiFi → bỏ qua gửi, không treo; broker chết → thử lại sau 5 s |

---

## 12. Cây quyết định debug phần cứng

```mermaid
flowchart TD
    P(["🐞 Có vấn đề"]) --> Q1{"Serial Monitor<br/>có chữ không?"}
    Q1 -- "Không / ký tự rác" --> A1["• monitor_speed = 115200?<br/>• Cáp USB có dây data?<br/>• Nhấn nút EN để reset"]
    Q1 -- Có --> Q2{"Board có bị<br/>reset liên tục?"}
    Q2 -- Có --> A2["• Đọc backtrace (exception decoder)<br/>• Brownout → nguồn yếu, đổi cáp/cổng USB<br/>• GPIO12 bị kéo HIGH lúc boot?"]
    Q2 -- Không --> Q3{"WiFi timeout?"}
    Q3 -- Có --> A3["• SSID/PASS trong config.h<br/>• Router phải là 2.4 GHz<br/>• Xem RSSI, đưa board lại gần"]
    Q3 -- Không --> Q4{"Giá trị cảm biến<br/>luôn 0 hoặc 3.3 V?"}
    Q4 -- Có --> A4["• Dây tín hiệu có vào GPIO34?<br/>• Chung GND giữa cảm biến và ESP32?<br/>• Đo điện áp chân bằng đồng hồ<br/>• Không cấp quá 3.3 V vào chân ADC!"]
    Q4 -- Không --> Q5{"Hệ thống<br/>phản hồi chậm?"}
    Q5 -- Có --> A5["• Tìm delay() / while trong loop()<br/>• Đo thời gian mỗi vòng loop()<br/>• Xem lại Mục 5"]
    Q5 -- Không --> A6["Mô tả lỗi + log + ảnh mạch<br/>→ tạo Issue cho nhóm"]
```

> 💡 **Chia đôi để khoanh vùng:** khi không rõ lỗi ở đâu, tắt một nửa hệ thống
> (comment `wifi_check_connection()` chẳng hạn). Lỗi còn hay mất? Lặp lại với nửa còn lại.
