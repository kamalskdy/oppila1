#include "contiki.h"
#include "sys/etimer.h"
#include "dev/adc-owinos.h"
#include "dev/adxl345.h"
#include "dev/relay.h"
#include "dev/i2c.h"
#include "dev/bmp180.h"
#include "mosquitto_demo.h"
#include "mqtt-res.h"
#include "dev/leds.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
/*---------------------------------------------------------------------------*/
#if DEBUG_APP
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
sensor_values_t mosquitto_demo_sensors;
command_values_t mosquitto_demo_commands;
/*---------------------------------------------------------------------------*/
process_event_t mosquitto_demo_sensors_data_event;
process_event_t mosquitto_demo_sensors_alarm_event;
/*---------------------------------------------------------------------------*/
PROCESS(mosquitto_demo_sensors_process, "Agriculture sensor process");
/*---------------------------------------------------------------------------*/
static int
activate_actuator(int arg)
{
  if(!arg) {
    relay.value(RELAY_ON);
  } else {
    relay.value(RELAY_OFF);
  }
  process_poll(&mosquitto_demo_sensors_process);
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
poll_sensors(void)
{
  /* Poll the temperature and humidity sensor */
  
  mosquitto_demo_sensors.sensor[ANALOG_SENSOR_LDR].value = adc_owinos.value(OWINOS_SENSORS_ADC1);
//  PRINTF("%d\n", adc_owinos.value(OWINOS_SENSORS_ADC1));
  if ((adc_owinos.value(OWINOS_SENSORS_ADC1))<=100){
                  leds_on(LEDS_GREEN);
  PRINTF("THERE IS NO LIGHT\n");
  }
  else
  {
          leds_off(LEDS_GREEN1);
  }

  SENSORS_ACTIVATE(adxl345);
 // PRINTF("%d \n", adxl345.value(X_AXIS));
  mosquitto_demo_sensors.sensor[DIGITAL_SENSOR_X_AXIS].value = adxl345.value(X_AXIS);
  if (adxl345.value(X_AXIS)<=-120||adxl345.value(X_AXIS)>180){
          PRINTF("GETS DAMAGED\n");
          leds_toggle(LEDS_GREEN1);
  }
  else {
          leds_off(LEDS_GREEN1);
  }

  mosquitto_demo_sensors.sensor[DIGITAL_SENSOR_Y_AXIS].value = adxl345.value(Y_AXIS);
  mosquitto_demo_sensors.sensor[DIGITAL_SENSOR_Z_AXIS].value = adxl345.value(Z_AXIS);
  SENSORS_DEACTIVATE(adxl345);

  SENSORS_ACTIVATE(bmp180);
  mosquitto_demo_sensors.sensor[DIGITAL_SENSOR_PRES].value = bmp180.value(BMP180_READ_PRESSURE);
  mosquitto_demo_sensors.sensor[DIGITAL_SENSOR_TEMP].value = bmp180.value(BMP180_READ_TEMP);
  SENSORS_DEACTIVATE(bmp180);

  /* Check the sensor values and publish alarms if required, else send the data
   * to any subscriber
   */
  mqtt_sensor_check(&mosquitto_demo_sensors, mosquitto_demo_sensors_alarm_event,
                    mosquitto_demo_sensors_data_event);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mosquitto_demo_sensors_process, ev, data)
{
  static struct etimer et;

  /* This is where our process start */
  PROCESS_BEGIN();

  /* Load sensor defaults */
  mosquitto_demo_sensors.num = 0;

  adc_owinos.configure(SENSORS_HW_INIT, OWINOS_SENSORS_ADC1);

  mqtt_sensor_register(&mosquitto_demo_sensors, ANALOG_SENSOR_LDR,
                       DEFAULT_WRONG_VALUE, DEFAULT_PUBLISH_EVENT_LDR,
                       NULL, DEFAULT_SUBSCRIBE_CFG_LDRTHR, DEFAULT_CC2538_LDR_MIN,
		       DEFAULT_CC2538_LDR_MAX, DEFAULT_LDR_THRESH,
                       DEFAULT_LDR_THRESL, 100);
 
  mqtt_sensor_register(&mosquitto_demo_sensors, DIGITAL_SENSOR_X_AXIS,
                       DEFAULT_WRONG_VALUE, DEFAULT_PUBLISH_EVENT_X_AXIS,
                       NULL, DEFAULT_SUBSCRIBE_CFG_X_AXISHR,
                       DEFAULT_ADXL345_X_AXIS_MIN, DEFAULT_ADXL345_X_AXIS_MAX,
		       DEFAULT_ADXL345_X_AXIS_THRESH,DEFAULT_ADXL345_X_AXIS_THRESL,0);

  mqtt_sensor_register(&mosquitto_demo_sensors, DIGITAL_SENSOR_Y_AXIS,
                       DEFAULT_WRONG_VALUE, DEFAULT_PUBLISH_EVENT_Y_AXIS,
                       NULL, DEFAULT_SUBSCRIBE_CFG_Y_AXISHR,
                       DEFAULT_ADXL345_Y_AXIS_MIN, DEFAULT_ADXL345_Y_AXIS_MAX,
                       DEFAULT_ADXL345_Y_AXIS_THRESH, DEFAULT_ADXL345_Y_AXIS_THRESL, 0);

  mqtt_sensor_register(&mosquitto_demo_sensors, DIGITAL_SENSOR_Z_AXIS,
                       DEFAULT_WRONG_VALUE, DEFAULT_PUBLISH_EVENT_Z_AXIS,
                       NULL, DEFAULT_SUBSCRIBE_CFG_Z_AXISHR,
                       DEFAULT_ADXL345_Z_AXIS_MIN, DEFAULT_ADXL345_Z_AXIS_MAX,
                       DEFAULT_ADXL345_Z_AXIS_THRESH, DEFAULT_ADXL345_Z_AXIS_THRESL, 0);

  mqtt_sensor_register(&mosquitto_demo_sensors, DIGITAL_SENSOR_PRES,
                       DEFAULT_BMP180_PRES_MIN, DEFAULT_PUBLISH_EVENT_PRES,
                       DEFAULT_PUBLISH_ALARM_PRES, DEFAULT_SUBSCRIBE_CFG_PRESTHR,
                       DEFAULT_BMP180_PRES_MIN, DEFAULT_BMP180_PRES_MAX,
                       DEFAULT_PRES_THRESH, DEFAULT_PRES_THRESL, 10);

  mqtt_sensor_register(&mosquitto_demo_sensors, DIGITAL_SENSOR_TEMP,
                       DEFAULT_BMP180_TEMP_MIN, DEFAULT_PUBLISH_EVENT_TEMP,
                       DEFAULT_PUBLISH_ALARM_TEMP, DEFAULT_SUBSCRIBE_CFG_TEMPTHR,
                       DEFAULT_BMP180_TEMP_MIN, DEFAULT_BMP180_TEMP_MAX,
                       DEFAULT_TEMP_THRESH, DEFAULT_TEMP_THRESL, 10);

  /* Sanity check */
  if(mosquitto_demo_sensors.num != DEFAULT_SENSORS_NUM) {
    printf("Mosquitto sensors: error! number of sensors mismatch\n");
    PROCESS_EXIT();
  }

  /* Load commands default */
  mosquitto_demo_commands.num = 1;
  memcpy(mosquitto_demo_commands.command[MOSQUITTO_DEMO_COMMAND].command_name,
         DEFAULT_COMMAND_EVENT_RELAY, strlen(DEFAULT_COMMAND_EVENT_RELAY));
  mosquitto_demo_commands.command[MOSQUITTO_DEMO_COMMAND].cmd = activate_actuator;

  if(mosquitto_demo_commands.num != DEFAULT_COMMANDS_NUM) {
    printf("Mosquitto sensors: error! number of commands mismatch\n");
    PROCESS_EXIT();
  }

  /* Get an event ID for our events */
  mosquitto_demo_sensors_data_event = process_alloc_event();
  mosquitto_demo_sensors_alarm_event = process_alloc_event();

  /* Activate Relay for GPIO activation */
  SENSORS_ACTIVATE(relay);
  activate_actuator(0);

  /* Start the periodic process */
  etimer_set(&et, DEFAULT_SAMPLING_INTERVAL);

  while(1) {

   PROCESS_YIELD();

    if(ev == PROCESS_EVENT_TIMER && data == &et) {
      poll_sensors();
      etimer_reset(&et);
    } else if(ev == sensors_stop_event) {
      PRINTF("Mosquitto: sensor readings paused\n");
      etimer_stop(&et);
    } else if(ev == sensors_restart_event) {
      PRINTF("Mosquitto: sensor readings enabled\n");
      etimer_reset(&et);
    }
  }

  PROCESS_END();
}

