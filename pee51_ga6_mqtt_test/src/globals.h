#ifndef GLOBALS_H
#define GLOBALS_H

#include <SoftwareSerial.h>

extern EspSoftwareSerial::UART gsmSerial;

extern float temperature, temperature1, temperature2, temperature3, temperature4, humidity, humidity1, humidity2, humidity3, humidity4;
extern float ppmAl_MQ8, ppmAl_MQ7, ppmLPG_MQ8, ppmLPG_MQ7, ppmCH4_MQ8, ppmCH4_MQ7, ppmH, ppmh2_MQ7, ppmCO_MQ8, ppmCO;
extern String jsonString;

extern uint16_t packet_id_counter;
extern float a, b;
extern int c;
extern char ctrlz[];
extern uint8_t mqttMessage[];
extern int mqttMessageLength;
#endif