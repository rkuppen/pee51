#include <DHT.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MQUnifiedsensor.h> 

#include "sensors.h"
#include "pee51mqtt.h"
#include "globals.h"

//#define RatioMQ7CleanAir 27.5 
//#define RatioMQ8CleanAir 70   

String createJson();

float temperature, temperature1, temperature2, temperature3, temperature4, humidity, humidity1, humidity2, humidity3, humidity4 = 0.00;
float ppmAl_MQ8, ppmAl_MQ7, ppmLPG_MQ8, ppmLPG_MQ7, ppmCH4_MQ8, ppmCH4_MQ7, ppmH, ppmh2_MQ7, ppmCO_MQ8, ppmCO = 0.00;

/************************MQ7 - Carbon monoxide sensor*********************************/
#define Board ("ESP-32")       
#define Pin (33)               
#define Type ("MQ-7")           
#define Voltage_Resolution (5)  // 3V3 <- IMPORTANT. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define ADC_Bit_Resolution (12) // ESP-32 bit resolution. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define RatioMQ7CleanAir 27.5
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

/************************MQ8 - Hydrogen Sensor************************************/
#define Pin_MQ8 (32)
#define Type_MQ8 ("MQ-8")       
#define Voltage_Resolution (5)  // 3V3 <- IMPORTANT. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define ADC_Bit_Resolution (12) // ESP-32 bit resolution. 
#define RatioMQ8CleanAir 70
MQUnifiedsensor MQ8(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ8, Type_MQ8);

/************************DHT22 - Temperature and humidity*************************/

#define DHT_SENSOR_PIN 14 
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void dht22_init(){
    dht_sensor.begin();
}
void mq7_init()
{
     vTaskDelay(500 / portTICK_PERIOD_MS);
    // CO
    MQ7.setRegressionMethod(1); //_PPM =  a*ratio^b
    // MQ7.setA(521853); MQ7.setB(-3.821); // Configurate the ecuation values to get Benzene concentration
    MQ7.setA(99.042);
    MQ7.setB(-1.518); // Configure the equation to calculate CO concentration value
    MQ7.init();
    delay(5);
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
        calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
        Serial.print(".");
    }
    MQ7.setR0(calcR0 / 10);
    Serial.println("Done calculating R0 for MQ7!.");
    /*
      //If the RL value is different from 10K please assign your RL value with the following method:
      MQ7.setRL(9.87);
    */
    if (isinf(calcR0))
    {
        Serial.println("MQ7 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    } // while(1);
    if (calcR0 == 0)
    {
        Serial.println("MQ7 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    } // while(1);
    /*****************************  MQ CAlibration ********************************************/
    MQ7.serialDebug(true);
}
void mq8_init()
{
     vTaskDelay(500 / portTICK_PERIOD_MS);
    // Hydrogen
    MQ8.setRegressionMethod(1); //_PPM =  a*ratio^b
    MQ8.setA(976.97);
    MQ8.setB(-0.688); // Configure the equation to to calculate H2 concentration
    MQ8.init();
    Serial.print("Calibrating MQ8 please wait.");
    float calcR0_MQ8 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ8.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0_MQ8 += MQ8.calibrate(RatioMQ8CleanAir);
        Serial.print(".");
    }
    MQ8.setR0(calcR0_MQ8 / 10);
    Serial.println("R0 for MQ8 calculation done!.");

    if (isinf(calcR0_MQ8))
    {
        Serial.println("MQ8 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    }
    if (calcR0_MQ8 == 0)
    {
        Serial.println("MQ8 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    }
    /*****************************  MQ CAlibration ********************************************/
    MQ8.serialDebug(true);
}

void measure()
{
    Serial.println("Inside measure task, inside sensors.cpp.");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    temperature1 = dht_sensor.readTemperature();
    humidity1 = dht_sensor.readHumidity();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    temperature2 = dht_sensor.readTemperature();
    humidity2 = dht_sensor.readHumidity();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    temperature3 = dht_sensor.readTemperature();
    humidity3 = dht_sensor.readHumidity();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    temperature4 = dht_sensor.readTemperature();
    humidity4 = dht_sensor.readHumidity();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    temperature = (temperature1 + temperature2 + temperature3 + temperature4) / 4;
    humidity = (humidity1 + humidity2 + humidity3 + humidity4) / 4;

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    MQ7.update();
    ppmCO = MQ7.readSensor();
    Serial.print("CO value: ");
    Serial.println(ppmCO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    MQ8.update();
    ppmH = MQ8.readSensor();
    Serial.print("H value: ");
    Serial.println(ppmH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    // Alcohol
    MQ8.setA(76101);
    MQ8.setB(-1.86);
    MQ7.setA(40000000000000000);
    MQ7.setB(-12.35);
    delay(50);
    MQ7.update();
    MQ8.update();
    ppmAl_MQ8 = MQ8.readSensor();
    ppmAl_MQ7 = MQ7.readSensor();
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // LPG
    MQ8.setA(10000000);
    MQ8.setB(-3.123);
    MQ7.setA(700000000);
    MQ7.setB(-7.703);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    MQ7.update();
    MQ8.update();
    ppmLPG_MQ8 = MQ8.readSensor();
    ppmLPG_MQ7 = MQ7.readSensor();
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // CH4
    MQ8.setA(80000000000000);
    MQ8.setB(-6.666);
    MQ7.setA(60000000000000);
    MQ7.setB(-10.54);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    MQ7.update();
    MQ8.update();
    ppmCH4_MQ8 = MQ8.readSensor();
    ppmCH4_MQ7 = MQ7.readSensor();
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // H2 MQ7
    MQ7.setA(69.014);
    MQ7.setB(-1.374);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    MQ7.update();
    ppmh2_MQ7 = MQ7.readSensor();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    // CO MQ8
    MQ8.setA(2000000000000000000);
    MQ8.setB(-8.074);
   vTaskDelay(50 / portTICK_PERIOD_MS);
    MQ8.update();
    ppmCO_MQ8 = MQ8.readSensor();
    vTaskDelay(50 / portTICK_PERIOD_MS);

    MQ8.setA(76101);
    MQ8.setB(-1.86); // Terug naar H2
    MQ7.setA(99.042);
    MQ7.setB(-1.518); // Terug naar CO
    vTaskDelay(50 / portTICK_PERIOD_MS);
    MQ7.update();
    MQ8.update();
    vTaskDelay(50 / portTICK_PERIOD_MS);
}
/*
String createJson() {
  // Construct JSON string
  String jsonString = "{";
  jsonString += "\"temperature\":" + String(temperature) + ",";
  jsonString += "\"humidity\":" + String(humidity) + ",";
  jsonString += "\"ppmCO\":" + String(ppmCO) + ",";
  jsonString += "\"ppmH\":" + String(ppmH) + ",";
  jsonString += "\"ppmAl_MQ8\":" + String(ppmAl_MQ8) + ",";
  jsonString += "\"ppmAl_MQ7\":" + String(ppmAl_MQ7) + ",";
  jsonString += "\"ppmLPG_MQ8\":" + String(ppmLPG_MQ8) + ",";
  jsonString += "\"ppmLPG_MQ7\":" + String(ppmLPG_MQ7) + ",";
  jsonString += "\"ppmCH4_MQ8\":" + String(ppmCH4_MQ8) + ",";
  jsonString += "\"ppmCH4_MQ7\":" + String(ppmCH4_MQ7) + ",";
  jsonString += "\"ppmh2_MQ7\":" + String(ppmh2_MQ7) + ",";
  jsonString += "\"ppmCO_MQ8\":" + String(ppmCO_MQ8);
  jsonString += "}";

  return jsonString;
}*/
String createJson() {
  // Construct JSON string
  String jsonString = "{";
  jsonString += "\"temperature\":" + String(a) + ",";
  jsonString += "\"humidity\":" + String(b);  + ",";
  jsonString += "\"ppmCO\":" + String(c) + ",";
  jsonString += "}";
  return jsonString;
}
