#include "esphome/core/log.h"
#include "sensor.h"
#include "obis.h"
#include <cstddef>

#define BUF_SIZE 2500
#define WAIT_TIME 1

uint8_t data_cmd[] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  '/',  '#' , '!', 0x0D, 0x0A
};
const char* DELIMITERS = "(*";
unsigned long timeLastRun;
char buffer[BUF_SIZE];
namespace esphome {
namespace fjarrvarme {

static const char *TAG = "fjarrvarme.sensor";

void FVSensor::setup() {
  // Serial.begin(115200);
  // pinMode(RX_PIN, INPUT);
  // pinMode(TX_PIN, OUTPUT);
  ESP_LOGI(TAG, "Starting sensor...");

}

void FVSensor::update() {
  if (millis() - timeLastRun > WAIT_TIME * 60000) {
        sendDataCmd();
        readTelegram();
        timeLastRun = millis();
    }

}

void FVSensor::loop() {

}

void FVSensor::publishSensors(const OBISData *data, int count) {
  for (int i = 0; i < count; i++) {
    // Publish each sensor reading
    ESP_LOGI(TAG, "Publishing sensor %d: %s", i, data[i].value);
  }
}

void FVSensor::dump_config(){
    ESP_LOGCONFIG(TAG, "Fjärrvärmesensor");
}

    void FVSensor::sendDataCmd() {
      for (int i = 0; i < sizeof(data_cmd); i++) {
        this->write_byte(data_cmd[i]);
      }
      ESP_LOGI("cmd", "data cmd sent");
    }

    void FVSensor::readTelegram() {

      OBISData obisdata[MAX_OBIS_CODES];
      
      bool publish=false;
      // fast forward until we find the STX byte (start-of-text)
      uint8_t b = 0x00;
      while (this->available() && b != 0x02) {
        b = this->read();
      }

      while (int len = this->available()) {
        ESP_LOGD("readTelegram", "Got %d bytes available to read", len);
        if (!this->read_array((uint8_t *) buffer, len))
               ESP_LOGW("readTelegram", "read_array() returned false, meter reading may be incomplete");
        ESP_LOGD("readTelegram", "Read %s", buffer);

        int count;
        parse_obis(buffer, obisdata, &count);
        print_parsed_data(obisdata, count);
        publishSensors(obisdata, count);

        // clean buffer
        memset(buffer, 0, BUF_SIZE - 1);

      }
    }

}  // namespace fjarrvarme
}  // namespace esphome