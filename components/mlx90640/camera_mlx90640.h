#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

#include "MLX90640_API.h"

namespace esphome {
namespace mlx90640_app {

// Keep as PollingComponent only (camera functionality via web server)
class MLX90640 : public PollingComponent, public i2c::I2CDevice {
 public:
  static MLX90640 *instance;
  int rx(uint16_t addr, uint16_t len, uint16_t *data);
  int tx(uint16_t addr, uint16_t data);
  
  MLX90640() = default;

  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Sensor setters
  void set_min_temperature_sensor(sensor::Sensor *min_temperature_sensor) {
    this->min_temperature_sensor_ = min_temperature_sensor;
  }
  void set_max_temperature_sensor(sensor::Sensor *max_temperature_sensor) {
    this->max_temperature_sensor_ = max_temperature_sensor;
  }
  void set_mean_temperature_sensor(sensor::Sensor *mean_temperature_sensor) {
    this->mean_temperature_sensor_ = mean_temperature_sensor;
  }
  void set_median_temperature_sensor(sensor::Sensor *median_temperature_sensor) {
    this->median_temperature_sensor_ = median_temperature_sensor;
  }

  // Configuration setters
  void set_refresh_rate(uint8_t refresh_rate) { this->refresh_rate_ = refresh_rate; }

  bool get_raw_pixels(float **data) {
    *data = this->mlx90640To_;
    return this->data_lock_.try_lock();
  }

  void put_raw_pixels(void) {
    this->data_lock_.unlock();
  }


 protected:
  // Sensor outputs
  sensor::Sensor *min_temperature_sensor_{nullptr};
  sensor::Sensor *max_temperature_sensor_{nullptr};
  sensor::Sensor *mean_temperature_sensor_{nullptr};
  sensor::Sensor *median_temperature_sensor_{nullptr};
  esphome::Mutex data_lock_;

  // Configuration
  uint8_t refresh_rate_;

  // MLX90640 specific
  paramsMLX90640 mlx90640_;
  uint16_t eeMLX90640_[832];
  float mlx90640To_[768];  // 32x24 thermal array

  // Helper methods
  void read_thermal_data_();
  void publish_sensors_();
};

}  // namespace mlx90640_app
}  // namespace esphome
