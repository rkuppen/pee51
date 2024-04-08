#ifndef PEE51MQTT_H
#define PEE51MQTT_H

#include <string.h>
#include <stdint.h>
#include <DHT.h>
#include <Arduino.h>
#include <MQUnifiedsensor.h> 

#include "sensors.h"
#include "globals.h"


void InitGsmSerial(int, int);

void GA6_init(void);

void reinitialize_sim800l(void);

void connectAndPublish(char *clientId, char *mqttTopic, char *message, char *mqttBroker, int);

bool connectGSMNetwork(void);

String listenToGsm(bool toUpperCase = false);

//void listenToGsm(bool toUpperCase);

boolean waitForGsmResponse(String expectedResponse);

void sendMQTTMessage(char* , char* , char* , char* , char* );

void mqtt_connect_message(uint8_t *, char *);

void mqtt_publish_message(uint8_t *, char *, char *);

void mqtt_disconnect_message(uint8_t *);

void pleaseSendData(const char *clientId, const char *mqttBroker, const char *mqttPort, const char *mqttTopic, const char *, uint16_t packet_id);

void mqtt_connect(uint8_t *mqttMessage, const char *clientId);

void mqtt_publish(uint8_t *mqtt_message, const char *mqttTopic, const char *message, uint16_t packet_id);

#endif
