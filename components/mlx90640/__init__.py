import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, i2c
from esphome.const import (
    CONF_ID,
    CONF_ADDRESS,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)


DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor"]

# Component namespace
mlx90640_ns = cg.esphome_ns.namespace("mlx90640_app")
MLX90640 = mlx90640_ns.class_(
    "MLX90640",
    cg.PollingComponent,
    i2c.I2CDevice
)

# Configuration keys
CONF_REFRESH_RATE = "refresh_rate"
CONF_MIN_TEMPERATURE = "min_temperature"
CONF_MAX_TEMPERATURE = "max_temperature"
CONF_MEAN_TEMPERATURE = "mean_temperature"
CONF_MEDIAN_TEMPERATURE = "median_temperature"

# Base configuration schema
BASE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MLX90640),
        cv.Optional(CONF_ADDRESS, default=0x33): cv.i2c_address,
        cv.Optional(CONF_REFRESH_RATE, default=0x04): cv.hex_uint8_t,
        cv.Optional(CONF_MIN_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MEAN_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MEDIAN_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.polling_component_schema("60s")).extend(i2c.i2c_device_schema(default_address=0x33))

# Final schema wrapped in cv.All
CONFIG_SCHEMA = cv.All(BASE_SCHEMA)


async def to_code(config):
    # Create the MLX90640 component
    var = cg.new_Pvariable(config[CONF_ID])
    
    # Register as polling component
    await cg.register_component(var, config)
    
    # Register as I2C device
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_refresh_rate(config[CONF_REFRESH_RATE]))

    # Configure temperature sensors if present
    if CONF_MIN_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MIN_TEMPERATURE])
        cg.add(var.set_min_temperature_sensor(sens))

    if CONF_MAX_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MAX_TEMPERATURE])
        cg.add(var.set_max_temperature_sensor(sens))

    if CONF_MEAN_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MEAN_TEMPERATURE])
        cg.add(var.set_mean_temperature_sensor(sens))

    if CONF_MEDIAN_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MEDIAN_TEMPERATURE])
        cg.add(var.set_median_temperature_sensor(sens))

