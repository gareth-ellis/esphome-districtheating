from esphome.components import uart, sensor
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    UNIT_KILOWATT_HOURS,
    UNIT_CUBIC_METER,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_TOTAL_INCREASING,
    CONF_ID,
)

DEPENDENCIES = ["uart"]

fjarrvarme_ns = cg.esphome_ns.namespace("fjarrvarme")
FVSensor = fjarrvarme_ns.class_("FVSensor", cg.PollingComponent)

CONF_CUMULATIVE_ACTIVE_IMPORT = "cumulative_active_import"
CONF_CUMULATIVE_VOLUME = "cumulative_volume"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FVSensor),
    cv.Required("uart_in"): cv.use_id(uart.UARTComponent),
    cv.Required("uart_out"): cv.use_id(uart.UARTComponent),
    cv.Required(CONF_CUMULATIVE_ACTIVE_IMPORT): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ),
    cv.Required(CONF_CUMULATIVE_VOLUME): sensor.sensor_schema(
        unit_of_measurement=UNIT_CUBIC_METER,
        accuracy_decimals=6,
    ),
}).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    rx = await cg.get_variable(config["uart_in"])
    tx = await cg.get_variable(config["uart_out"])
    cg.add(var.set_uart_rx(rx))
    cg.add(var.set_uart_tx(tx))
    sens1 = await sensor.new_sensor(config[CONF_CUMULATIVE_ACTIVE_IMPORT])
    cg.add(var.set_cumulative_active_import(sens1))
    sens2 = await sensor.new_sensor(config[CONF_CUMULATIVE_VOLUME])
    cg.add(var.set_cumulative_volume(sens2))
