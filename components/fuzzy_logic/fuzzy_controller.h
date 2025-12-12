#pragma once

#include "esphome/core/component.h"
#include "eFLL/Fuzzy.h"

namespace esphome {
namespace fuzzy_logic_controller {

class FuzzyLogicController : public Component {
 private:
  Fuzzy *fuzzy_{nullptr};

 public:
  Fuzzy * get(void);
  // --- ESPHome Component Overrides ---
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; }
};

} // namespace fuzzy_logic_controller
} // namespace esphome
