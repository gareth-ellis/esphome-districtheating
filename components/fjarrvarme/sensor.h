#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "obis.h"  // Include the header where OBISData is defined

namespace esphome {
namespace fjarrvarme {

class FVSensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void publishSensors(const OBISData *data, int count);
  void dump_config() override;
  void sendDataCmd();
  void readTelegram();
};

}  // namespace fjarrvarme
}  // namespace esphome