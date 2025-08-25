#include "esphome/core/log.h"
#include "sensor.h"
#define BUF_SIZE 100
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

class ParsedMessage {
  public:
    double cumulativeActiveImport;
    double cumulativeVolume;
};

void FVSensor::setup() {
  // Serial.begin(115200);
  // pinMode(RX_PIN, INPUT);
  // pinMode(TX_PIN, OUTPUT);
  ESP_LOGI(TAG, "Starting sensor...");

}

void FVSensor::update() {
  if (millis() - timeLastRun > WAIT_TIME * 60000) {
        int baud_rate = this->parent_->get_baud_rate();
        this->parent_->set_baud_rate(300);
        ESP_LOGW(TAG, "Changing baud rate from %d to 300", baud_rate);
        sendDataCmd();
        this->parent_->set_baud_rate(baud_rate);
        ESP_LOGW(TAG, "Restoring baud rate to %d", baud_rate);
        readTelegram();
        timeLastRun = millis();
        ESP_LOGI(TAG, "Data sent %lu", timeLastRun);
    }

}

void FVSensor::loop() {

}

void FVSensor::publishSensors(ParsedMessage* parsed) {
  ESP_LOGI(TAG, "Publishing sensor: 6.8 = %f", parsed->cumulativeActiveImport);
  ESP_LOGI(TAG, "Publishing sensor: 6.26 = %f", parsed->cumulativeVolume);
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



 char* strtok_single (char * str, char const * delims) {
      static char  * src = NULL;
      char  *  p,  * ret = 0;
      if (str != NULL)
        src = str;
      if (src == NULL)
        return NULL;
      if ((p = strpbrk (src, delims)) != NULL) {
        *p  = 0;
        ret = src;
        src = ++p;
      } else if (*src) {
        ret = src;
        src = NULL;
      }
      return ret;
    }

    void parseRow(ParsedMessage* parsed, char* obis_code, char* value) {
      if (strncmp(obis_code, "6.8", 6) == 0) {
        parsed->cumulativeActiveImport = atof(value) * 1000;

      } else if (strncmp(obis_code, "6.26", 6) == 0) {
        parsed->cumulativeVolume = atof(value);

      }
    }


void FVSensor::readTelegram() {

  ParsedMessage parsed = ParsedMessage();
      
      bool publish=false;
      // fast forward until we find the STX byte (start-of-text)
      uint8_t b = 0x00;
      int i=0;
      while (available() && b != 0x02) {
        b = read();
        i++;
      }
      ESP_LOGW("readTelegram", "Found STX byte %d", b);
      ESP_LOGW("readTelegram", "Interface status %d",available());
      ESP_LOGW("readTelegram", "Bytes read before STX: %d", i);

      while (int len = available()) {
        ESP_LOGW("readTelegram", "Got %d bytes available to read", len);
        if (!read_array((uint8_t *) buffer, len))
               ESP_LOGW("readTelegram", "read_array() returned false, meter reading may be incomplete");
        ESP_LOGW("readTelegram", "Read %s", buffer);

        if (len > 0) {
          // end character reached
          if (buffer[0] == '!') {
            publishSensors(&parsed);
            return;
          }

          char* obis_code = strtok_single(buffer, "(");
          while (obis_code != NULL) {
            char* value = strtok_single(NULL, "*)");
            char* unit = strtok_single(NULL, "*)");
            
            if (value != NULL) {
              //ESP_LOGI("data", "%s=[%s]", obis_code, value);
              parseRow(&parsed, obis_code, value);
              publish=true;
            }
            obis_code = strtok_single(NULL, "(");
          }
         
        }else{
          ESP_LOGW("readTelegram", "Incomplete data received");
        }

        // clean buffer
        memset(buffer, 0, BUF_SIZE - 1);

      }

      if (publish == true) {
        ESP_LOGD("readTelegram", "Publishing sensor data");
        publishSensors(&parsed);
      }else{
        ESP_LOGW("readTelegram", "No valid sensor data to publish");

      }
    }

}  // namespace fjarrvarme
}  // namespace esphome