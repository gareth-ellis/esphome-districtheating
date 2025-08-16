from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_CELSIUS, ICON_THERMOMETER, DEVICE_CLASS_TEMPERATURE

import esphome.codegen as cg
import esphome.config_validation as cv

DEPENDENCIES = ['sensor']

fjarrvarme_ns = cg.esphome_ns.namespace('fjarrvarme')
FjarrvarmeSensor = fjarrvarme_ns.class_('FjarrvarmeSensor', sensor.Sensor)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FjarrvarmeSensor),
}).extend(sensor.SENSOR_SCHEMA).extend({
    cv.Optional(CONF_ID): cv.use_id(FjarrvarmeSensor),
})

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield sensor.register_sensor(var, config)