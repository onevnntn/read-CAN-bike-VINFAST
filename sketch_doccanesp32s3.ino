#include "driver/twai.h"

// Định nghĩa chân CAN
#define CAN_TX_PIN GPIO_NUM_17
#define CAN_RX_PIN GPIO_NUM_18

void setup() {
    Serial.begin(115200);
    
    // 1. Cấu hình cấu trúc bộ điều khiển CAN (TWAI)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_LISTEN_ONLY); // Chế độ Listen Only để an toàn cho xe
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); // Thử 250Kbits trước, nếu không được đổi sang 500KBITS
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // 2. Cài đặt và khởi động trình điều khiển
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        Serial.println("CAN Driver installed successfully.");
    }
    if (twai_start() == ESP_OK) {
        Serial.println("CAN Driver started.");
    }
}

void loop() {
    twai_message_t message;
    // 3. Chờ và nhận gói tin từ mạng CAN của xe
    if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        // In ID của gói tin (Hex)
        Serial.print("ID: 0x");
        Serial.print(message.identifier, HEX);
        Serial.print("  Data: ");
        
        // In các byte dữ liệu đi kèm (thường là 8 byte)
        for (int i = 0; i < message.data_length_code; i++) {
            Serial.print(message.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}