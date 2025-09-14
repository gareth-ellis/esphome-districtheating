#include "sensor.h"
#include "esphome/core/log.h"
#include <string>

namespace esphome {
namespace fjarrvarme {

constexpr size_t BUF_SIZE = 100;
constexpr unsigned long WAIT_TIME_MIN = 1;

static const char *const TAG = "fjarrvarme.sensor";

uint8_t data_cmd[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2F, 0x3F, 0x21, 0x0D, 0x0A
};

unsigned long last_run = 0;
char buffer[BUF_SIZE];

void FVSensor::set_uart_rx(uart::UARTComponent *uart_rx) { uart_rx_ = uart_rx; }
void FVSensor::set_uart_tx(uart::UARTComponent *uart_tx) { uart_tx_ = uart_tx; }

void FVSensor::setup() {
  ESP_LOGI(TAG, "Sensor initialization complete.");
}

void FVSensor::update() {
  if (millis() - last_run > WAIT_TIME_MIN * 60000) {
    if (uart_tx_ != nullptr) {
      sendDataCmd();
    } else {
      ESP_LOGW(TAG, "TX UART not set, cannot send command.");
    }
    if (uart_rx_ != nullptr) {
      readTelegram();
    } else {
      ESP_LOGW(TAG, "RX UART not set, cannot read telegram.");
    }
    last_run = millis();
    ESP_LOGI(TAG, "Cycle complete at %lu ms", last_run);
  }
}

void FVSensor::loop() {
  // No periodic background work needed here.
}


void FVSensor::set_cumulative_active_import(sensor::Sensor *cumulative_active_import) {
  this->cumulative_active_import = cumulative_active_import;
}

void FVSensor::set_cumulative_volume(sensor::Sensor *cumulative_volume) {
  this->cumulative_volume = cumulative_volume;
}

void FVSensor::publishSensors(ParsedMessage* parsed) {
  if (parsed->cumulativeActiveImport) 
    this->cumulative_active_import->publish_state(parsed->cumulativeActiveImport);
  if (parsed->cumulativeVolume)
    this->cumulative_volume->publish_state(parsed->cumulativeVolume);
  ESP_LOGI(TAG, "Published: Energy=%.2f, Volume=%.6f", parsed->cumulativeActiveImport, parsed->cumulativeVolume);
}

void FVSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Fjärrvärmesensor configuration loaded.");
}

void FVSensor::sendDataCmd() {
  if (!uart_tx_) {
    ESP_LOGW(TAG, "TX UART not available for sending command.");
    return;
  }
  uart_tx_->set_baud_rate(300);
  for (size_t i = 0; i < sizeof(data_cmd); i++) {
    uart_tx_->write_byte(data_cmd[i]);
  }
  ESP_LOGI(TAG, "Command sent to meter.");
}

char* FVSensor::strtok_single(char *str, char const *delims) {
  static char *src = nullptr;
  char *p, *ret = nullptr;
  if (str != nullptr)
    src = str;
  if (src == nullptr)
    return nullptr;
  p = strpbrk(src, delims);
  if (p != nullptr) {
    *p = 0;
    ret = src;
    src = ++p;
  } else if (*src) {
    ret = src;
    src = nullptr;
  }
  return ret;
}

void FVSensor::parseRow(ParsedMessage* parsed, char* obis_code, char* value) {
  if (strncmp(obis_code, "6.8", 6) == 0) {
    parsed->cumulativeActiveImport = atof(value) * 1000;
  } else if (strncmp(obis_code, "6.26", 6) == 0) {
    parsed->cumulativeVolume = atof(value);
  }
}

std::string FVSensor::readLine() {
  if (!uart_rx_) {
    ESP_LOGW(TAG, "RX UART not available for reading.");
    return "";
  }

  std::string line;
  uint8_t byte = 0x00;
  int i = 0;
  // Read until newline character
  while (uart_rx_->available()) {
    uart_rx_->read_byte(&byte);
    i++;
    if (byte == '\n') {
      ESP_LOGD(TAG, "Newline character received.");
      break;
    }
    line += static_cast<char>(byte);
  }
  ESP_LOGD(TAG, "Read %d bytes from UART before newline or break.", i);
  return line;
}

void FVSensor::readTelegram() {
  if (!uart_rx_) {
    ESP_LOGW(TAG, "RX UART not available for reading.");
    return;
  }

  ParsedMessage parsed{};
  bool should_publish = false;
  uint8_t byte = 0x00;
  int preamble = 0;

  // Skip until start-of-text (STX)
  std::string line = this->readLine();
  ESP_LOGD(TAG, "Preamble: %s", line.c_str());
  uart_rx_->set_baud_rate(2400);

  while (int len = uart_rx_->available()) {
    ESP_LOGD(TAG, "Reading %d bytes from UART.", len);
    if (!read_array(reinterpret_cast<uint8_t *>(buffer), len)) {
      ESP_LOGW(TAG, "Failed to read %d bytes from UART.", len);
      break;
    }
    if (len > 0) {
      if (buffer[0] == '!') {
        publishSensors(&parsed);
        return;
      }
      char* obis_code = strtok_single(buffer, "(");
      while (obis_code != nullptr) {
        char* value = strtok_single(nullptr, "*)");
        strtok_single(nullptr, "*)"); // skip unit
        if (value != nullptr) {
          parseRow(&parsed, obis_code, value);
          should_publish = true;
        }
        obis_code = strtok_single(nullptr, "(");
      }
    } else {
      ESP_LOGW(TAG, "No data received from UART.");
    }
    memset(buffer, 0, BUF_SIZE);
  }

  if (should_publish) {
    ESP_LOGD(TAG, "Publishing parsed sensor data.");
    publishSensors(&parsed);
  } else {
    ESP_LOGW(TAG, "No valid sensor data to publish.");
  }
}

bool FVSensor::read_array(uint8_t *buf, int len) {
  if (!uart_rx_) return false;
  for (int i = 0; i < len; i++) {
    if (!uart_rx_->read_byte(&buf[i])) {
      return false;
    }
  }
  return true;
}

}  // namespace fjarrvarme
}  // namespace esphome