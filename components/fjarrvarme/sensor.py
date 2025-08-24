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

fjarrvarme_ns = cg.esphome_ns.namespace("fjarrvarme")
FVSensor = fjarrvarme_ns.class_("FVSensor", cg.PollingComponent, uart.UARTDevice)


CONFIG_SCHEMA = (
    sensor.sensor_schema(
        FVSensor,
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        device_class=DEVICE_CLASS_ENERGY,
    ).extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
