#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace fjarrvarme {

class ParsedMessage;

class FVSensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void set_uart_rx(uart::UARTDevice *uart_rx) { uart_rx_ = uart_rx; }
  void set_uart_tx(uart::UARTDevice *uart_tx) { uart_tx_ = uart_tx; }
  void setup() override;
  void update() override;
  void loop() override;
  void publishSensors(ParsedMessage *parsed);
  void dump_config() override;
  void sendDataCmd();
  void parse_obis(char *str, char const *delimiters);
  void readTelegram();

protected:
  uart::UARTDevice *uart_rx_;
  uart::UARTDevice *uart_tx_;

};

}  // namespace fjarrvarme
}  // namespace esphome