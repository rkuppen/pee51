// sensors.h
#ifndef SENSORS_H
#define SENSORS_H

#include <DHT.h>
#include <Arduino.h>
#include <MQUnifiedsensor.h>

#include "pee51mqtt.h"
#include "globals.h"

extern MQUnifiedsensor MQ7;
extern MQUnifiedsensor MQ8;

void dht22_init(void);
void mq8_init(void);
void mq7_init(void);

extern DHT dht_sensor;

void measure(void);
String createJson();

#endif
