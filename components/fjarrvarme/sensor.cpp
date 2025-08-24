#include "esphome/core/log.h"
#include "sensor.h"

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
  char response[] = "12345678910";
  if (sizeof(response) > 10) {
    
    float energy = (float)response[5] * 100.0f; // Dummy conversion
    float volume = (float)response[6] * 10.0f;   // Dummy conversion

    this->publish_state(energy); // Publish energy value
    ESP_LOGD(TAG, "UH50: Energy=%.2f kWh, Volume=%.2f m3", energy, volume);
  } else {
    ESP_LOGW(TAG, "UH50: No valid response received");
  }

}

void FVSensor::loop() {

}

void FVSensor::dump_config(){
    ESP_LOGCONFIG(TAG, "Fjärrvärmesensor");
}

}  // namespace fjarrvarme
}  // namespace esphome