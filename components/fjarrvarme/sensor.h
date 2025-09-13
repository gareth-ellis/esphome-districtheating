#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace fjarrvarme {

class FVSensor : public PollingComponent, public sensor::Sensor {
 public:
  void set_uart_rx(uart::UARTComponent *uart_rx) { uart_rx_ = uart_rx; }
  void set_uart_tx(uart::UARTComponent *uart_tx) { uart_tx_ = uart_tx; }

 protected:
  uart::UARTComponent *uart_rx_{nullptr};
  uart::UARTComponent *uart_tx_{nullptr};

  void update() override;
};

}  // namespace fjarrvarme
}  // namespace esphome