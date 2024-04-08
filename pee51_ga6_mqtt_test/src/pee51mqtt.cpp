#include <DHT.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MQUnifiedsensor.h>

#include "pee51mqtt.h"
#include "sensors.h"
#include "globals.h"
extern EspSoftwareSerial::UART gsmSerial;
uint8_t mqttMessage[512]={ 0 };
int mqttMessageLength = 0;
char ctrlz[] = "\x1a";
extern String inputString;

extern float temperature, humidity;
extern float ppmAl_MQ7, ppmLPG_MQ7, ppmCH4_MQ7, ppmH, ppmh2_MQ7, ppmCO;
extern float ppmAl_MQ8, ppmLPG_MQ8, ppmCH4_MQ8, ppmH, ppmCO_MQ8;


void InitGsmSerial(int GSM_RX_PIN, int GSM_TX_PIN)
{
    Serial.print("Received RX pin: ");
    Serial.println(GSM_RX_PIN);
    Serial.print("Received TX pin: ");
    Serial.println(GSM_TX_PIN);

    gsmSerial.begin(9600, SWSERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN, false); // GSM module's port
    if (!gsmSerial)
    { // If the object did not initialize, then its configuration is invalid
        Serial.println("Invalid EspSoftwareSerial pin configuration, check config");
        while (1)
        { // Don't continue with invalid configuration
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}
/*          Werkt niet
void connectAndPublish(char *clientId, char *mqttTopic, char *message, char *mqttBroker, int port)
{
    gsmSerial.println("AT");
    if (waitForGsmResponse("OK"))    {            }
    else
    {
           gsmSerial.println("AT");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CSQ");
    if (waitForGsmResponse("+CSQ"))
    {
        // Success!
    }
    else
    {
        gsmSerial.println("AT+CSQ");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGREG?");
    if (waitForGsmResponse("+CGREG"))
    {
        // Success!
    }
    else
    {
        gsmSerial.println("AT+CGREG?");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGATT?");
    if (waitForGsmResponse("+CGATT"))
    {
        // Success!
    }
    else
    {
        gsmSerial.println("AT+CGATT?");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSHUT");
    if (waitForGsmResponse("OK"))
    {
        // Success!
    }
    else
    {
        gsmSerial.println("AT+CIPSHUT");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPMUX=0");
    if (waitForGsmResponse("OK"))
    {
        // Success!
    }
    else
    {
        gsmSerial.println("AT+CIPMUX=0");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTART=\"TCP\",\"5.104.118.202\",1883");
    if (waitForGsmResponse("+CME"))
    {
        // Success!
    }
    else
    {
         
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIICR");
    if (waitForGsmResponse("OK"))
    {
        // Success!
    }
    else
    {
       gsmSerial.println("AT+CIICR");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIFSR");
    // Establish TCP connection to broker

    // gsmSerial.println("AT+CIPSTART=\"TCP\",\"" + String(mqttBroker) + "\"," + port);
    
    // if (gsmSerial.find("OK") || gsmSerial.find("CONNECT OK"))
    // {
    //     Serial.println("TCP connected");
    // }
    // else
    // {
    //     Serial.println("TCP connection failed");
    //     return;
    // }
       
    // CONNECT Packet
    byte connectPacket[100];

    connectPacket[0] = 0x10;                  // Control Packet Type
    connectPacket[1] = 12 + strlen(clientId); // Remaining Length
    connectPacket[2] = 0;                     // Protocol Name Length MSB
    connectPacket[3] = 4;                     // Protocol Name Length LSB
    connectPacket[4] = 'M';                   // Protocol Name
    connectPacket[5] = 'Q';
    connectPacket[6] = 'T';
    connectPacket[7] = 'T';
    connectPacket[8] = 4;                 // Protocol Version
    connectPacket[9] = 0x02;              // Connect Flags
    connectPacket[10] = 0;                // Keep Alive MSB
    connectPacket[11] = 60;               // Keep Alive LSB
    connectPacket[12] = 0;                // Client ID Length MSB
    connectPacket[13] = strlen(clientId); // Client ID Length LSB
    memcpy(&connectPacket[14], clientId, strlen(clientId));
    // QoS 1
    connectPacket[15] = 0x00; // Will QoS
    connectPacket[16] = 0x01; // QoS value

    Serial.print("Size of Connect packet: ");
    Serial.println(sizeof(connectPacket));
    Serial.println("");
    Serial.print("Connect packet in HEX: ");
    for (int i = 0; i < sizeof(connectPacket); i++)
    {
        Serial.print(connectPacket[i], HEX);
    };
    Serial.println("");
    Serial.println("Sending MQTT Connect now.");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSEND"); // Length
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.write(connectPacket, sizeof(connectPacket));
    if (gsmSerial.find("OK") || gsmSerial.find("SEND OK"))
    {
        Serial.println("Sent CONNECT succesfull");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); // (signals end of message)

    // Add CONNACK check

    // Send MQTT PUBLISH packet
    Serial.println("Sending MQTT Publish now.");

    byte publishPacket[100];
    publishPacket[0] = 0x30; // Control Packet Type
    int mqttBrokerLen = strlen(mqttTopic);
    publishPacket[1] = 2 + mqttBrokerLen + strlen(message); // Remaining Length
    publishPacket[2] = 0;
    publishPacket[3] = mqttBrokerLen;
    memcpy(&publishPacket[4], mqttTopic, mqttBrokerLen);
    memcpy(&publishPacket[4 + mqttBrokerLen], message, strlen(message));

    Serial.print("Size of MQTT Publish packet: ");
    Serial.println(sizeof(publishPacket));
    Serial.println("");
    Serial.print("MQTT Publish packet: ");
    for (int i = 0; i < sizeof(publishPacket); i++)
    {
        Serial.print(publishPacket[i], HEX);
    };
    Serial.println("");
    Serial.println("Sending MQTT Publish now.");
    Serial.println("");

    gsmSerial.println("AT+CIPSEND"); // Length
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.write(publishPacket, sizeof(publishPacket));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); // (signals end of message)
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    if (gsmSerial.find("OK") || gsmSerial.find("SEND OK"))
    {
        Serial.println("Sent Publish succesfull.");
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
    
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // Close TCP connection
    gsmSerial.println("AT+CIPCLOSE");
}
*/
void sendMQTTMessage(char *clientId, char *mqttBroker, char *mqttPort, char *mqttTopic, char *message)
{
    Serial.println("Inside the fisrt, longer, sendData function.");
    Serial.print("Received clientID: ");
    Serial.println(clientId);
    Serial.print("Received Broker: ");
    Serial.println(mqttBroker);
    Serial.print("Received Port: ");
    Serial.println(mqttBroker);
    Serial.print("Received Topic: ");
    Serial.println(mqttBroker);
    Serial.print("Received Message: ");
    Serial.println(message);

    Serial.println("Running send_mqtt.");
    Serial.println("Send AT. Return OK = ready."); // is it ready? good return is: OK
    gsmSerial.println("AT");

    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CSQ. Return Higer then 0,0."); // is it ready? good return is: OK
    gsmSerial.println("AT+CSQ");
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CGREG?. Return +CREG:0.1 = ready."); // is it registered on GPRS provider network?
    gsmSerial.println("AT+CGREG?");
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CGATT?. Return +CGATT: 1 = ready."); // is GPRS attached or not?
    gsmSerial.println("AT+CGATT?");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPSHUT. Return SHUT OK"); // resets previous IP session if any. good return is : SHUT OK
    gsmSerial.println("AT+CIPSHUT");
    listenToGsm();
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPSTATUS?. Return STATE: IP INITIAL."); // Check if IP stack is intitalized.
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPMUX=0. Return OK"); // Sets single connection mode.
    gsmSerial.println("AT+CIPMUX=0");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\" Return OK."); // Starts task
    gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPSTATUS?. Return STATE: IP START."); // Check if IP stack is intitalized.
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIICR. Return OK."); // Brings up wireless connection.
    gsmSerial.println("AT+CIICR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPSTATUS?. Return STATE: IP GPRSACT."); // Check if IP stack is intitalized.
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIFSR. Return other then 0.0.0.0"); // Gets local IP adress.
    gsmSerial.println("AT+CIFSR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Send AT+CIPSTART."); // Starts connection with TCP protocol, TCP, domain name, port.
    gsmSerial.println("AT+CIPSTART=\"TCP\", \"5.104.118.202\", 1883");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    /*                  MQTT Connect                */
    Serial.println("AT+CIPSEND");
    gsmSerial.println("AT+CIPSEND");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    mqttMessageLength = 16 + strlen(clientId);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    mqtt_connect(mqttMessage, clientId);
    for (int j = 0; j < mqttMessageLength; j++)
    {
        gsmSerial.write(mqttMessage[j]); // Message contents
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); // (signals end of message)
    Serial.println("First ctrl-z send.");
    Serial.print("mqttMessageLength to connect: ");
    Serial.println(mqttMessageLength);
    Serial.print("MQTT connect message: ");
    for (int j = 0; j < mqttMessageLength; j++)
    {
        Serial.write((byte)mqttMessage[j]); // Message contents
    }
    Serial.println("");

    /*                  MQTT Publish                */
    mqttMessageLength = 4 + strlen(mqttBroker) + strlen(message);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    Serial.println("AT+CIPSEND");
    gsmSerial.println("AT+CIPSEND");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    mqtt_publish_message(mqttMessage, mqttBroker, message);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    for (int k = 0; k < mqttMessageLength; k++)
    {
        gsmSerial.write(mqttMessage[k]);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); // (signals end of message)
    Serial.println("Second ctrl-z send.");

    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("");
    Serial.println("-------------Sent-------------"); // Message contents
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.print("MQTT published message: ");
    for (int k = 0; k < mqttMessageLength; k++)
    {
        Serial.write((byte)mqttMessage[k]);
    }
    Serial.println("");
    Serial.print("mqttMessageLength to publish: ");
    Serial.println(mqttMessageLength);
}

void pleaseSendData(const char *clientId, const char *mqttBroker, const char *mqttPort, const char *mqttTopic, const char *message, uint16_t packet_id){
    Serial.println("Running pleaseSendData.");    
    Serial.println("Setting up TCP connection.");
    gsmSerial.println("AT");
    if (waitForGsmResponse("OK")){}
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);        
        gsmSerial.println("AT");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CSQ");
    if (waitForGsmResponse("+CSQ:")||waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CSQ");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CREG?");
    if (waitForGsmResponse("+CREG: ")||waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CREG?");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGREG?");
    if (waitForGsmResponse("+CGREG: ")||waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CGREG?");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGATT?");
    if (waitForGsmResponse("+CGATT: ")||waitForGsmResponse("OK"))    {    }
    else
    {
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CGATT?");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CGATT=1");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CGATT=1");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSHUT");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPSHUT");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    if (waitForGsmResponse("+IPSTATUS: IP INITIAL"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPSTATUS");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);    
    gsmSerial.println("AT+CIPMUX=0");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPMUX=0");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    gsmSerial.println("AT+CIPSTATUS");
    if (waitForGsmResponse("+CIPSTATUS")||waitForGsmResponse("OK")||waitForGsmResponse("+CIPSTATUS"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPSTATUS");        
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);  
    gsmSerial.println("AT+CIICR");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIICR");        
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTATUS");
    if (waitForGsmResponse("IPSTATUS: IP GPRSACT"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPSTATUS");        
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);  
    gsmSerial.println("AT+CIFSR");
    if (waitForGsmResponse("OK"))    {    }
    else
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIFSR");        
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSTART=\"TCP\",\"5.104.118.202\",1883");
    if (waitForGsmResponse("CONNECT OK"))    {    }
    else
    {
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CIPSTART=\"TCP\",\"5.104.118.202\",1883");    
        //gsmSerial.println("AT+CIPSTART=\"TCP\",\"" + String(mqttBroker) + "\"," + port);    
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    mqttMessageLength = 16 + strlen(clientId);
    Serial.print("MQTT Connect length: ");    
    Serial.println(mqttMessageLength); 
    mqtt_connect(mqttMessage, clientId);
    Serial.println("MQTT Connect Message: ");
    for (int j = 0; j < mqttMessageLength; j++) {        
        Serial.print(mqttMessage[j], HEX); // Message contents
        Serial.print("");
    }
        Serial.println();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    for (int j = 0; j < mqttMessageLength; j++) {
        gsmSerial.write(mqttMessage[j]);        
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); // (signals end of message)
    listenToGsm();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSEND");
    listenToGsm();
    Serial.println("Send first AT+CIPSEND");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    mqttMessageLength = 4 + strlen(mqttTopic) + strlen(message);
    Serial.print("MQTT Pubblish length: ");    
    Serial.println(mqttMessageLength);    
    Serial.println("MQTT Publish Message: ");
    mqtt_publish(mqttMessage, mqttTopic, message, packet_id);
    for (int j = 0; j < mqttMessageLength; j++) {
        Serial.print(mqttMessage[j], HEX);
        Serial.print(" ");
    }
    Serial.println();
    for (int k = 0; k < mqttMessageLength; k++) {
        gsmSerial.write(mqttMessage[k]);
    } //Misschien listenToGsm(); In de for loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gsmSerial.write(ctrlz); 
    listenToGsm();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPSEND");
    listenToGsm();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("Send second AT+CIPSEND");
    Serial.println("-------------Sent-------------"); 
    vTaskDelay(15000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+CIPCLOSE");
    listenToGsm();
    Serial.println("Send AT+CIPCLOSE");       
}
void mqtt_connect_message(uint8_t *mqtt_message, char *client_id)
{
    uint8_t i = 0;
    uint8_t client_id_length = strlen(client_id);

    mqtt_message[0] = 16;                    // MQTT Message Type CONNECT
    mqtt_message[1] = 0;   // Protocol Name Length MSB   
    mqtt_message[2] = 14 + client_id_length; // Remaining length of the message 
    mqtt_message[3] = 6;   // Protocol Name Length LSB
    mqtt_message[4] = 77;  // ASCII Code for M
    mqtt_message[5] = 81;  // ASCII Code for Q
    mqtt_message[6] = 73;  // ASCII Code for I
    mqtt_message[7] = 115; // ASCII Code for s
    mqtt_message[8] = 100; // ASCII Code for d
    mqtt_message[9] = 112; // ASCII Code for p
    mqtt_message[10] = 3;  // MQTT Protocol version = 3
    mqtt_message[11] = 2;  // conn flags
    mqtt_message[12] = 0;  // Keep-alive Time Length MSB
    mqtt_message[13] = 15; // Keep-alive Time Length LSB

    mqtt_message[14] = 0;                // Client ID length MSB
    mqtt_message[15] = client_id_length; // Client ID length LSB

    // Client ID
    for (i = 0; i < client_id_length + 16; i++)
    {
        mqtt_message[16 + i] = client_id[i];
    }
}

void mqtt_publish_message(uint8_t *mqtt_message, char *mqttBroker, char *message)
{
    uint8_t i = 0;
    uint8_t mqttBroker_length = strlen(mqttBroker);
    uint8_t message_length = strlen(message);

    mqtt_message[0] = 48;                                // MQTT Message Type CONNECT
    mqtt_message[1] = 2 + mqttBroker_length + message_length; // Remaining length
    mqtt_message[2] = 0;                                 // MQTT Message Type CONNECT
    mqtt_message[3] = mqttBroker_length;                      // MQTT Message Type CONNECT

    // Topic
    for (i = 0; i < mqttBroker_length; i++)
    {
        mqtt_message[4 + i] = mqttBroker[i];
    }

    // Message
    for (i = 0; i < message_length; i++)
    {
        mqtt_message[4 + mqttBroker_length + i] = message[i];
    }
}

void mqtt_disconnect_message(uint8_t *mqtt_message)
{
    mqtt_message[0] = 0xE0; // msgtype = connect
    mqtt_message[1] = 0x00; // length of message (?)
}

void mqtt_connect(uint8_t *mqtt_message, const char *client_id) {
    // MQTT Message Type: CONNECT
    mqtt_message[0] = 16;

    // Placeholder for Remaining Length (to be updated later)
    mqtt_message[1] = 0;
    mqtt_message[2] = 0;

    // Protocol Name: MQTT, Protocol Level: 4
    mqtt_message[3] = 0;   // Protocol Name Length MSB
    mqtt_message[4] = 4;   // Protocol Name Length LSB
    mqtt_message[5] = 'M';
    mqtt_message[6] = 'Q';
    mqtt_message[7] = 'T';
    mqtt_message[8] = 'T';

    // Connect Flags: QoS = 1, Clean Session = 1
    mqtt_message[9] = 0x02;

    // Keep Alive: 30 seconds
    mqtt_message[10] = 0;  // Keep Alive MSB
    mqtt_message[11] = 30; // Keep Alive LSB

    // Client ID
    uint16_t client_id_length = strlen(client_id);
    mqtt_message[12] = (client_id_length >> 8) & 0xFF; // Client ID Length MSB
    mqtt_message[13] = client_id_length & 0xFF;       // Client ID Length LSB
    memcpy(&mqtt_message[14], client_id, client_id_length); // Copy Client ID string
     mqtt_message[14 + client_id_length] = '\0'; // Null terminate the Client ID string
    // Calculate Remaining Length and update mqtt_message[1] and mqtt_message[2]
    uint16_t total_length = 14 + client_id_length; // Total length of Variable Header and Payload
    mqtt_message[1] = (total_length >> 8) & 0xFF;  // Remaining Length MSB
    mqtt_message[2] = total_length & 0xFF;        // Remaining Length LSB
}

void mqtt_publish(uint8_t *mqtt_message, const char *mqttTopic, const char *payload, uint16_t packet_id) {
    // PUBLISH Message Type
    mqtt_message[0] = 0x30;

    // Placeholder for Remaining Length (to be updated later)
    mqtt_message[1] = 0;
    mqtt_message[2] = 0;

    // Topic Name
    uint16_t topic_length = strlen(mqttTopic);
    mqtt_message[3] = (topic_length >> 8) & 0xFF; // Topic Name Length MSB
    mqtt_message[4] = topic_length & 0xFF;       // Topic Name Length LSB
    memcpy(&mqtt_message[5], mqttTopic, topic_length); // Copy Topic Name string

    // Calculate Payload Length
    uint16_t payload_length = strlen(payload);

    // Copy Payload
    memcpy(&mqtt_message[5 + topic_length], payload, payload_length);
    mqtt_message[5 + topic_length + 1 + payload_length] = '\0';

    // Remaining Length 
    uint16_t remaining_length = 5 + topic_length + payload_length;
    mqtt_message[1] = (remaining_length >> 8) & 0xFF; // Remaining Length MSB
    mqtt_message[2] = remaining_length & 0xFF;       // Remaining Length LSB

    // Set Packet Identifier for QoS levels 1 and 2
    if (packet_id != 0) {
        mqtt_message[3] |= 0x02; // Set QoS bits
        mqtt_message[5 + topic_length + payload_length] = (packet_id >> 8) & 0xFF; // Packet Identifier MSB
        mqtt_message[6 + topic_length + payload_length] = packet_id & 0xFF;        // Packet Identifier LSB
    }
}


bool connectGSMNetwork()
{
    Serial.println("connectGSMNetwork.");
    Serial.println("Checking network registration...");
    unsigned long startMillis = millis();
    // Wait up to 10 seconds
    if (gsmSerial.println("AT+CREG?"))
    {
        listenToGsm();
        Serial.println("Inside the connectGSMNetwork bool: ");
        Serial.print("gsmSerial.available(): ");
        Serial.println(gsmSerial.available());
        Serial.print("inputString before: ");
        Serial.println(inputString);
        if (inputString.indexOf("+CREG: 0,1") != -1 || inputString.indexOf("+CREG: 0,5") != -1)
        {
            Serial.println(" Registered");
            return true; // Network registered successfully
        }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second before retrying

    Serial.println(" Not registered");
    return false; // Failed to register on the network
}

void reinitialize_sim800l()
{
    Serial.println("reinitialize_sim800l.");
    Serial.println("Sending same commands as in setup again.");
    Serial.println("Sending AT+CFUN=1 to enable full functionallity mode.");
    gsmSerial.println("AT+CFUN=1");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();

    Serial.println("Sending AT+CREG command to check network registration status");
    gsmSerial.println("AT+CREG");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();

    Serial.println("Sending AT+CGATT command to attach to GPRS network.");
    gsmSerial.println("AT+CGATT=1");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();

    Serial.println("Sending AT+CIICR command to bring up wireless connection.");
    gsmSerial.println("AT+CIICR");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();

    Serial.println("Sending AT+CIFSR command to get local IP address");
    gsmSerial.println("AT+CIFSR");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();

    Serial.println("Sending AT+CIPSTATUS command to get connection status");
    gsmSerial.println("AT+CIPSTATUS");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    listenToGsm();
}

void GA6_init()
{
    Serial.println("Running GA6_init.");
    Serial.println("Initializing IoT-GA6.");
    Serial.println("AT+CGDCONT=1,\"IP\",\"data.lycamobile.nl\"");

    // Set APN (Access Point Name) details
    gsmSerial.println("AT+CGDCONT=1,\"IP\",\"data.lycamobile.nl\"");
    listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    Serial.println("Activating GPRS (PDP) context.");
    gsmSerial.println("AT+CGACT=1,1");
    listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    Serial.println("Return current stat of PDD context.");
    gsmSerial.println("AT+CGDCONT?");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Bringing up wireless connection.");
    gsmSerial.println("AT+CIICR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Check if there's an IP.");
    gsmSerial.println("AT+CIFSR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Check the registration status.");
    gsmSerial.println("AT+CREG?");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Check attach status.");
    gsmSerial.println("AT+CGACT?");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Attach to network.");
    gsmSerial.println("AT+CGATT=1");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Wait for attach.");
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    Serial.println("Start task and set the APN.");
    gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Send AT+CIPSTATUS?. Return STATE: IP START."); // Check if IP stack is intitalized.
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Bring up the wireless connection.");
    gsmSerial.println("AT+CIICR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Wait for bringup.");
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    Serial.println("Get the local IP address.");
    gsmSerial.println("AT+CIFSR");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Get the status of the IP connection.");
    gsmSerial.println("AT+CIPSTATUS");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("APN settings configured.");
}

String listenToGsm(bool) {
  String response = "";
  uint32_t startTime = millis();
  while (gsmSerial.available() && (millis() - startTime < 500)) { // Timeout after 500ms
    response += gsmSerial.readString();
  }

  if (response.length() > 0) {
    Serial.println();
    Serial.print("-- Response (");
    Serial.print(response.length());
    Serial.println(") --");
    if (toUpperCase) {
      response.toUpperCase();
    }
    Serial.print(response);
  } else {
    // Handle timeout
    Serial.println("-- Timeout waiting for response --");
  }

  // Optionally, parse response for error codes or success indicators
  // ...

  return response;
}


boolean waitForGsmResponse(String expectedResponse)
{       
    String response;
    unsigned long startTime = millis();
    if (gsmSerial.available()){
        response = gsmSerial.readString();
    }   
    while (millis() - startTime < 10000)
    {
        // Read available data from GSM module
        while (gsmSerial.available())
        {
            response = gsmSerial.readString();

            // Check if response contains expected string
            if (response.indexOf(expectedResponse) >= 0)
            {
                Serial.print("Response: ");
                Serial.println(response);
                return true;
            }
        }
        // Wait a bit before checking again
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    // Timeout reached
    Serial.println("Timeout reached, retrying command.");
    Serial.println("Excpected response: " + expectedResponse + "Received response: " + response);
    return false;
}
