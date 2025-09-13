#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace fjarrvarme {

struct ParsedMessage {
  double cumulativeActiveImport = 0;
  double cumulativeVolume = 0;
};

class FVSensor : public PollingComponent {
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
  void parseRow(ParsedMessage* parsed, char* obis_code, char* value);
  char* strtok_single(char *str, char const *delims);
  bool read_array(uint8_t *buf, int len);

  sensor::Sensor *cumulative_active_import = nullptr;
  sensor::Sensor *cumulative_volume = nullptr;

 protected:
  uart::UARTComponent *uart_rx_{nullptr};
  uart::UARTComponent *uart_tx_{nullptr};
};

}  // namespace fjarrvarme
}  // namespace esphome