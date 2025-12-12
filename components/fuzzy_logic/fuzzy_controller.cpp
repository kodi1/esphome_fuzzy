#include "fuzzy_controller.h"

namespace esphome {
namespace fuzzy_logic_controller {

void FuzzyLogicController::setup() {
    this->fuzzy_ = new Fuzzy();
    ESP_LOGV("fuzzy_logic", "%s %#p", __func__, this->fuzzy_);
    ESP_LOGCONFIG("fuzzy_logic", "Fuzzy Logic Controller setup");
}

Fuzzy * FuzzyLogicController::get(void) {
  ESP_LOGV("fuzzy_logic", "%s %#p", __func__, this->fuzzy_);
  return this->fuzzy_;
}

void FuzzyLogicController::loop() {
}

// Boilerplate to make the C++ class accessible from YAML
// This is not necessary if you use the 'components' block as I did above,
// but it's good practice for creating a reusable component.
}
}
