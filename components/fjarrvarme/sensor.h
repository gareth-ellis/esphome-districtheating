#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace fjarrvarme {

class ParsedMessage;

class FVSensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void publishSensors(ParsedMessage *parsed);
  void dump_config() override;
  void sendDataCmd();
  void parse_obis(char *str, char const *delimiters);
  void readTelegram();

};

}  // namespace fjarrvarme
}  // namespace esphome