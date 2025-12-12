#include "camera_mlx90640.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mlx90640_app {

static const char *const TAG = "MLX90640";

MLX90640 *MLX90640::instance = nullptr;

// ============================================================================
// Setup
// ============================================================================

void MLX90640::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MLX90640...");
  instance = this; // Capture this instance during setup
  int status;

  status = MLX90640_I2CRead(this->address_, 0x2400, 1, this->eeMLX90640_);
  // Check if sensor is connected
  if (0 != status) {
    ESP_LOGE(TAG, "Failed to read from MLX90640 - is it connected %#x ?", this->address_);
    this->mark_failed();
    return;
  }

  status = MLX90640_DumpEE(this->address_, this->eeMLX90640_);
  if (status != 0)
    ESP_LOGE(TAG,"Failed to load system parameters");
  // Extract parameters
  status = MLX90640_ExtractParameters(this->eeMLX90640_, &this->mlx90640_);
  if (status != 0) {
    ESP_LOGE(TAG, "Failed to extract MLX90640 parameters");
    this->mark_failed();
    return;
  }

  // Set refresh rate
  MLX90640_SetRefreshRate(this->address_, this->refresh_rate_);

  ESP_LOGCONFIG(TAG, "MLX90640 setup complete");
}

// ============================================================================
// Update - Called periodically
// ============================================================================

void MLX90640::update() {
  ESP_LOGV(TAG, "Updating MLX90640 data...");
  this->data_ready_ = false;
  // Read thermal data from sensor
  this->read_thermal_data_();
  this->data_ready_ = true;
  // Publish temperature sensors to Home Assistant
  this->publish_sensors_();

  // Mark image as ready
  this->last_frame_time_ = millis();
}

// ============================================================================
// Loop - Called continuously
// ============================================================================

void MLX90640::loop() {
  // Nothing needed in loop
}

// ============================================================================
// Dump Config
// ============================================================================

void MLX90640::dump_config() {
  ESP_LOGCONFIG(TAG, "MLX90640:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Refresh Rate: 0x%02X", this->refresh_rate_);

  LOG_SENSOR("  ", "Min Temperature", this->min_temperature_sensor_);
  LOG_SENSOR("  ", "Max Temperature", this->max_temperature_sensor_);
  LOG_SENSOR("  ", "Mean Temperature", this->mean_temperature_sensor_);
  LOG_SENSOR("  ", "Median Temperature", this->median_temperature_sensor_);
}
// ============================================================================
// Read Thermal Data from MLX90640
// ============================================================================

void MLX90640::read_thermal_data_() {
  uint16_t frame[834];
  int status;

  // Get frame data
  for (int retry = 0; retry < 3; retry++) {
    status = MLX90640_GetFrameData(this->address_, frame);
    if (status >= 0) {
      break;
    }
    ESP_LOGW(TAG, "Failed to get frame data, retry %d/3", retry + 1);
    delay(10);
  }

  if (status < 0) {
    ESP_LOGE(TAG, "Failed to read frame data from MLX90640");
    return;
  }

  // Calculate temperatures
  float vdd = MLX90640_GetVdd(frame, &this->mlx90640_);
  float ta = MLX90640_GetTa(frame, &this->mlx90640_);
  float tr = ta - 8.0f;
  float emissivity = 0.95f;

  MLX90640_CalculateTo(frame, &this->mlx90640_, emissivity, tr, this->mlx90640To_);

  ESP_LOGV(TAG, "Frame read complete - Ta: %.1f°C, Vdd: %.2fV", ta, vdd);
}

// ============================================================================
// Publish Temperature Sensors
// ============================================================================

void MLX90640::publish_sensors_() {
  std::vector<float> temps(this->mlx90640To_, this->mlx90640To_ + 768);
  std::sort(temps.begin(), temps.end());

  if (this->min_temperature_sensor_ != nullptr) {
    this->min_temperature_sensor_->publish_state(temps[0]);
  }

  if (this->max_temperature_sensor_ != nullptr) {
    this->max_temperature_sensor_->publish_state(temps[767]);
  }

  if (this->mean_temperature_sensor_ != nullptr) {
    float sum = 0;
    for (auto i : temps) {
      sum += i;
    }
    this->mean_temperature_sensor_->publish_state(sum / 768.0f);
  }

  if (this->median_temperature_sensor_ != nullptr) {
    std::vector<float> temps(this->mlx90640To_, this->mlx90640To_ + 768);
    std::sort(temps.begin(), temps.end());
    this->median_temperature_sensor_->publish_state(temps[384]);
  }

  ESP_LOGV(TAG, "Published sensors - Min: %.1f°C, Max: %.1f°C",
           temps[0], temps[767]);
}

int MLX90640::rx(uint16_t addr, uint16_t len, uint16_t *data) {
  auto err = this->read_register16(addr, reinterpret_cast<uint8_t *>(data), len * 2);
  if (err != i2c::ERROR_OK) return -1;

  for (int i = 0; i < len; i++) {
    data[i] = esphome::i2c::i2ctohs(data[i]); // Fix endianness for ESP32
  }
  return 0;
}

int MLX90640::tx(uint16_t addr, uint16_t data) {
  uint16_t be_data = esphome::i2c::htoi2cs(data);
  auto err = this->write_register16(addr, reinterpret_cast<const uint8_t *>(&be_data), 2);
  return (err == i2c::ERROR_OK) ? 0 : -1;
}

}  // namespace mlx90640_app
}  // namespace esphome

extern "C" {
  int MLX90640_I2CRead(uint8_t slaveAddr, unsigned int startAddress, unsigned int nWordsRead, uint16_t *data) {
    if (esphome::mlx90640_app::MLX90640::instance == nullptr) return -1;
    return esphome::mlx90640_app::MLX90640::instance->rx(startAddress, nWordsRead, data);
  }

  int MLX90640_I2CWrite(uint8_t slaveAddr, unsigned int writeAddress, uint16_t data) {
    if (esphome::mlx90640_app::MLX90640::instance == nullptr) return -1;
    return esphome::mlx90640_app::MLX90640::instance->tx(writeAddress, data);
  }
}
