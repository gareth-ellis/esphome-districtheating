#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace fjarrvarme {

class ParsedMessage {
 public:
  double cumulativeActiveImport;
  double cumulativeVolume;
};

class FVSensor : public PollingComponent, public sensor::Sensor {
 public:
  void set_uart_rx(uart::UARTComponent *uart_rx);
  void set_uart_tx(uart::UARTComponent *uart_tx);

  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;
  void sendDataCmd();
  void readTelegram();
  void publishSensors(ParsedMessage* parsed);

 protected:
  bool read_array(uint8_t *buffer, int len);
  void parseRow(ParsedMessage* parsed, char* obis_code, char* value);
  char* strtok_single(char * str, char const * delims);

  uart::UARTComponent *uart_tx_{nullptr};
  uart::UARTComponent *uart_rx_{nullptr};
};

}  // namespace fjarrvarme
}  // namespace esphome