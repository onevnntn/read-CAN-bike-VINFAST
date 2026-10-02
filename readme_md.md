# Đọc Dữ Liệu CAN Bus bằng ESP32-S3 & TJA1050 (Arduino IDE)

Tài liệu chi tiết và mã nguồn đơn giản để đọc dữ liệu từ mạng **CAN Bus** bằng vi điều khiển **ESP32-S3** kết hợp với module chuyển đổi **TJA1050** trên môi trường **Arduino IDE**.

---

## 🛠️ 1. Sơ đồ đấu nối (Hardware Connections)

Module **TJA1050** hoạt động ở mức nguồn 5V. ESP32-S3 sử dụng bộ điều khiển CAN nội bộ (gọi là TWAI - Two-Wire Automotive Interface) nên **không cần dùng thư viện ngoài**.

| Chân TJA1050 | Chân ESP32-S3 | Ghi chú |
| :--- | :--- | :--- |
| **VCC** | **5V** (VIN / 5V) | Cấp nguồn 5V cho TJA1050 |
| **GND** | **GND** | Nối đất chung |
| **TXD** | **GPIO 17** | Nối vào chân CAN TX của ESP32-S3 |
| **RXD** | **GPIO 18** | Nối vào chân CAN RX của ESP32-S3 |
| **CANH** | **CAN_H** | Đường CAN High của xe/thiết bị |
| **CANL** | **CAN_L** | Đường CAN Low của xe/thiết bị |

> ⚠️ **Lưu ý:**
> - Nếu module TJA1050 của bạn có chân **S (Silent Mode)**, hãy nối chân **S xuống GND** để cho phép mạch gửi/nhận dữ liệu bình thường.
> - Đảm bảo giữa 2 dây CAN_H và CAN_L có điện trở xả bus **120Ω** (trên mạch TJA1050 thường đã gắn sẵn).

---

## 💻 2. Mã nguồn Arduino (ESP32-S3 TWAI)

Mã nguồn sử dụng trực tiếp thư viện `driver/twai.h` sẵn có trong ESP32 Board Package (không cần cài thêm library bên ngoài).

```cpp
#include "driver/twai.h"

// Định nghĩa chân GPIO nối với TJA1050
#define CAN_TX_PIN GPIO_NUM_17
#define CAN_RX_PIN GPIO_NUM_18

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    // 1. Cấu hình chế độ hoạt động (LISTEN ONLY để an toàn khi cắm vào xe)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_LISTEN_ONLY);
    
    // 2. Cấu hình tốc độ Baud (Thường dùng 250Kbps hoặc 500Kbps)
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); 
    
    // 3. Nhận tất cả các gói tin (không lọc ID)
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // 4. Khởi tạo driver CAN
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        Serial.println("Cai dat CAN Driver thanh cong.");
    } else {
        Serial.println("Loi cai dat CAN Driver!");
        return;
    }

    // 5. Bắt đầu đọc dữ liệu
    if (twai_start() == ESP_OK) {
        Serial.println("Da bat dau doc dữ liệu CAN...");
    } else {
        Serial.println("Loi khoi chay CAN!");
    }
}

void loop() {
    twai_message_t message;
    
    // Chờ nhận gói tin (thời gian chờ tối đa 1000ms)
    if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        // In CAN ID ở dạng HEX
        Serial.print("ID: 0x");
        Serial.print(message.identifier, HEX);
        Serial.print(" | Data: ");
        
        // In các Byte dữ liệu thu được
        for (int i = 0; i < message.data_length_code; i++) {
            if (message.data[i] < 0x10) Serial.print("0");
            Serial.print(message.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}
```

---

## 🚀 3. Hướng dẫn sử dụng

1. Mở **Arduino IDE** và chọn board: `ESP32S3 Dev Module`.
2. Sao chép đoạn code trên và nạp vào ESP32-S3.
3. Mở **Serial Monitor** thiết lập baudrate `115200`.
4. Nếu chưa thấy dữ liệu xuất ra, bạn hãy thử thay đổi cấu hình tốc độ Baud trong code:
   - Thay `TWAI_TIMING_CONFIG_250KBITS()` thành `TWAI_TIMING_CONFIG_500KBITS()` hoặc `TWAI_TIMING_CONFIG_125KBITS()`.

---

## ☕ Ủng hộ dự án (Donate) & Liên hệ

Nếu hướng dẫn này hữu ích đối với dự án của bạn, bạn có thể ủng hộ mình một ly cà phê! ❤️

* **Zalo:** `0844491666`
* **MoMo / Ngân hàng:** Quét mã QR bên dưới để chuyển khoản ủng hộ.

<div align="center">
  <img src="https://img.vietqr.io/image/970422-0844491666-compact2.png" alt="Mã QR Donate" width="280"/>
  <p><i>Cảm ơn sự hỗ trợ và đồng hành của bạn!</i></p>
</div>