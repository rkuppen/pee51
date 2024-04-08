#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <WiFi.h> 
//#include <MQUnifiedsensor.h> 
//#include <DHT.h>
#include <AsyncMqtt_Generic.h>
#include <HardwareSerial.h>
#include "pee51mqtt.h"
#include "sensors.h"
#include "globals.h"
//#include <Modem.h>

// To call use: ATD+31614504283;
bool upperCaseStatus = false; // keep tracks of whether incoming string needs to be converted to upper case
String inputString = "";
String lastCommand = ""; 
String number = "+31614504283;";

const char apn[] = "data.lycamobile.nl";
const char gprsUser[] = "lmnl";
const char gprsPass[] = "plus";

EspSoftwareSerial::UART gsmSerial;
#define GSM_RX_PIN 13
#define GSM_TX_PIN 12
PubSubClient client(gsmSerial);  // MQTT client
IPAddress server(5, 104, 118, 202);

const char mqttBroker[] = "5.104.118.202"; //"jrbubuntu.ddns.net"; //"test.mosquitto.org"; 
const char mqttPort[] = "1883";
int port = 1883;
const char mqttTopic[] = "pee51/gsmesp32"; 
const char clientId[] = "esp32_ga6";
char message[300]={ 0 }; 
uint16_t packet_id_counter, packet_id = 0; // Initialize packet_id counter
// Function to generate a unique packet_id
uint16_t generate_packet_id() {
    packet_id_counter++; // Increment counter
    if (packet_id_counter == 0) {
        // Handle wraparound, reset to 1
        packet_id_counter = 1;
    }
    return packet_id_counter;
}

/*For Testing*/
float a = 20.2;
float b = 12.122;
int c = 44;



void codeForTask1(void *parameter)
{
  for (;;)
  {          
    Serial.println("Inside Task1.");
    measure();   

    //sendMQTTMessage(clientId, mqttBroker, mqttPort, mqttTopic, jsonStr); 

    //Serial.println("Mqtt message send.");
    //sprintf(message, "{\"ID\":%s, \"Alcohol1\":%.2f,\"Alcohol2\":%.2f,\"LPG1\":%.2f,\"LPG2\":%.2f,\"CH4_1\":%.2f,\"CH4_2\":%.2f,\"H2_1\":%.2f,\"H2_2\":%.2f,\"CO1\":%.2f,\"CO2\":%.2f}", clientId, ppmAl_MQ8, ppmAl_MQ7, ppmLPG_MQ8, ppmLPG_MQ7, ppmCH4_MQ8, ppmCH4_MQ7, ppmH, ppmh2_MQ7, ppmCO_MQ8, ppmCO);
        
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Starting 2nd pleaseSendData.");

    // String json = createJson();
    // char jsonStr[300];
    // json.toCharArray(jsonStr, 300);
    // Serial.println("Serial print created Json.");
    // Serial.println(json);
    // pleaseSendData(clientId, mqttBroker, mqttPort, mqttTopic, jsonStr);


  }
}
TaskHandle_t Task1;

void pubsub(){  

    gsmSerial.println("AT+CSQ");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGREG?");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGATT?");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSHUT");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPMUX=0");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIICR");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();    vTaskDelay(300 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIFSR");
    gsmSerial.println("AT+CIPSTART=\"TCP\", \"5.104.118.202\", 1883");
  String response = listenToGsm(); // Get response without conversion

  // Check response for success (refer to SIM800L manual for expected response)
  if (response.indexOf("OK") > -1) {
    Serial.println("TCP connection established");

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    Serial.print("client state:...");
    Serial.println(client.state());
    // Attempt to connect
    if (client.connect("pee51esp32")) {
        Serial.println("connected");
        client.publish("pee51/gsmesp32","hello world");
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        vTaskDelay(5000/ portTICK_PERIOD_MS);
    }
  }
  }
}
// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(38400);
  vTaskDelay(500 / portTICK_PERIOD_MS);   
  gsmSerial.begin(38400, SWSERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN, false);
  //InitGsmSerial(GSM_RX_PIN, GSM_TX_PIN);
  Serial.println("GSM serial initialized.");
  dht22_init(); 
  mq7_init();
  mq8_init();
  Serial.println("Sensors initialized.");
  vTaskDelay(10000 / portTICK_PERIOD_MS);  
  gsmSerial.println("AT");
  listenToGsm();
  gsmSerial.println("AT+IPR=38400");
  listenToGsm();
  //GA6_init();
  client.setServer(server, 1883);  

  //xTaskCreatePinnedToCore(codeForTask1, "Sensors", 15000,       NULL,         0,            &Task1,         1); // Core: 1
                                       //pcName,  usStackDepth, pvParameters, uxPriority,   pvCreatedTask,  xCoreID
  vTaskDelay(500 / portTICK_PERIOD_MS);                                                                 // needed to start-up task1
}
void PleaseSendDataJson() 
{ //Enter "/" in serial monitor to run
    //measure();

    String json = createJson();
    char jsonStr[300];
    json.toCharArray(jsonStr, 300);
    Serial.println("Serial print created Json.");
    Serial.println(json);  
    vTaskDelay(500 / portTICK_PERIOD_MS);   
    pleaseSendData(clientId, mqttBroker, mqttPort, mqttTopic, jsonStr, packet_id);
}

void loop()
{ 
  if (Serial.available())
  {                                    // monitor the serial 0 interface
    String inputString = Serial.readString(); // read the contents of serial buffer as string
    Serial.println();
    Serial.print("-- Input (");
    Serial.print(inputString.length());
    Serial.println(") --");
    String lastCommand = inputString; // just a backup of the original string
    // custom commands
    if (inputString.startsWith("/"))
    { // allows you to send non-AT commands
      Serial.println(inputString);
      Serial.println("Running Json Please send data.");
      PleaseSendDataJson();
    }
    if (inputString.startsWith("1"))
    { // send mqtt message
      Serial.println(inputString);
      connectGSMNetwork();
    }
     if (inputString.startsWith("2"))
    { // send mqtt message
      Serial.println(inputString);
      GA6_init();
    }      
    if (inputString.startsWith("3"))
    { // send mqtt message
      Serial.println(inputString);
      reinitialize_sim800l();        
    } 
    if (inputString.startsWith("4"))  
    { // send mqtt message
      Serial.println(inputString);
      Serial.println("Please send data.");
      sprintf(message, "{\"temperature\":%.2f,\"humidity\":%.2f,\"ppmCO\":%d}", a, b, c);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      generate_packet_id();
      vTaskDelay(500 / portTICK_PERIOD_MS);
      pleaseSendData(clientId, mqttBroker, mqttPort, mqttTopic, message, packet_id);    
    } 
    if (inputString.startsWith("5"))  
    { // send mqtt message
      Serial.println(inputString);
      Serial.println("Manual mqtt 1.");
      sprintf(message, "{\"temperature\":%.2f,\"humidity\":%.2f,\"ppmCO\":%d}", a, b, c);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      generate_packet_id();
      vTaskDelay(500 / portTICK_PERIOD_MS);
      //manualMQTT();    
    } 
    if (inputString.startsWith("6"))  
    { // send mqtt message
      Serial.println(inputString);
      Serial.println("PubSub publish");
      pubsub();
    } 
    if (inputString.startsWith("?"))
    { // send mqtt message
      Serial.println(inputString);
      //quick_send;
    }    
    // send a number as hex
    else if (inputString.startsWith("#H") || inputString.startsWith("#h"))
    { // allows you to send hex value
      Serial.println(inputString);
      String subString = inputString.substring(2);     // remove "#h" part
      const char *hexString = subString.c_str();       // convert String object to C-style string
      uint8_t intNumber = strtol(hexString, NULL, 16); // convert hex formatted C-style string to int value
      gsmSerial.write(intNumber);                      // can only write values between 0-255
    }
    // send a number as dec
    else if (inputString.startsWith("#D") || inputString.startsWith("#d"))
    { // allows you to send dec value
      Serial.println(inputString);
      String subString = inputString.substring(2); // remove "#h" part
      uint8_t intNumber = subString.toInt();
      gsmSerial.write(intNumber); // can only write values between 0-255
    }
    // send the CR character
    else if (inputString.startsWith("<cr>") || inputString.startsWith("<CR>"))
    {                       // allows you to send dec value
      gsmSerial.write(0xD); // can only write values between 0-255
    }
    // send the line feed or newline charcater
    else if (inputString.startsWith("<lf>") || inputString.startsWith("<LF>"))
    {                       // allows you to send dec value
      gsmSerial.write(0xA); // can only write values between 0-255
    }
    // sends out a string without '$' or CR, LF characters
    else if (inputString.startsWith("$"))
    { // allows you to send text
      Serial.println(inputString);
      String subString = inputString.substring(1); // remove the $ char
      gsmSerial.print(subString);
      upperCaseStatus = false;
    }
    // at command
    else if (inputString.startsWith("at") || inputString.startsWith("AT"))
    {
      inputString.toUpperCase();
      Serial.println(inputString);
      gsmSerial.print(inputString);
      gsmSerial.write(0xD); // carriage return : important
      // gsmSerial.write(0xA); //newline
      upperCaseStatus = true;
    } //AT command with %
    else if (inputString.startsWith("%"))
    {
      inputString = "AT%" + inputString.substring(1);
      inputString.toUpperCase();
      Serial.println(inputString);
      gsmSerial.print(inputString);
      gsmSerial.write(0xD); // carriage return : important
      // gsmSerial.write(0xA); //newline
      upperCaseStatus = true;
    } //AT command with @
    else if (inputString.startsWith("@"))
    {
      inputString = "AT@" + inputString.substring(1);
      inputString.toUpperCase();
      Serial.println(inputString);
      gsmSerial.print(inputString);
      gsmSerial.write(0xD); // carriage return : important
      // gsmSerial.write(0xA); //newline
      upperCaseStatus = true;
    }
    // at command
    else if (inputString.startsWith(">"))
    {
      inputString = "AT+" + inputString.substring(1);
      inputString.toUpperCase();
      Serial.println(inputString);
      gsmSerial.print(inputString);
      gsmSerial.write(0xD); // carriage return : important
      // gsmSerial.write(0xA); //newline
      upperCaseStatus = true;
    }
  }  

  listenToGsm();  
  vTaskDelay(500 / portTICK_PERIOD_MS);                                                               

}

    