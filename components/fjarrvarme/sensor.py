import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
    UNIT_KILOWATT_HOURS,
    UNIT_CUBIC_METER,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_TOTAL_INCREASING,
)

DEPENDENCIES = ["uart"]

empty_uart_sensor_ns = cg.esphome_ns.namespace("fv_sensor")
FVSensor = empty_uart_sensor_ns.class_("FVSensor", cg.PollingComponent, uart.UARTDevice)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FVSensor),
        cv.Optional("heating_cumulative_active_import"): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOWATT_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
        cv.Optional("heating_cumulative_volume"): sensor.sensor_schema(
            unit_of_measurement=UNIT_CUBIC_METER, accuracy_decimals=6
        ),
    }
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
