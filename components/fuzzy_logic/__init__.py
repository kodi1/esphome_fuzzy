import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# 1. Define the C++ namespace for your component
fuzzy_logic_controller_ns = cg.esphome_ns.namespace('fuzzy_logic_controller')
FuzzyLogicController = fuzzy_logic_controller_ns.class_('FuzzyLogicController', cg.Component)

# 2. Map the C++ class name
# Assuming your main class in fuzzy_controller.h is 'FuzzyLogicController'
FuzzyLogicController = fuzzy_logic_controller_ns.class_('FuzzyLogicController', cg.Component)

# In fuzzy_controller/fuzzy_logic_controller/__init__.py

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(FuzzyLogicController),
})
# 4. Define the to_code function (How to generate C++ code)
# In fuzzy_controller/fuzzy_logic_controller/__init__.py

async def to_code(config):
    cg.add_library(
        name="eFLL",
        repository="https://github.com/benvoliobenji/eFLL-Reborn",
        version=None,
    )

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
