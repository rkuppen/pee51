#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Wire.h>

// To call use: ATD+31614504283;
bool upperCaseStatus = false; // keep tracks of whether incoming string needs to be converted to upper case
String inputString = "";
String lastCommand = ""; 
String number = "+31614504283;";
/*
const char apn[] = "data.lycamobile.nl";
const char apn_User[] = "lmnl";
const char apn_Pass[] = "plus";
*/
String apn = "portalmmm.nl";
String apn_User = " ";
String apn_Pass = " ";

char httpapi[] = "http://jrbubuntu.ddns.net:5000/api/telemetry";

String jsonPayload = "{\"ts\": 500,\"values\": {\"Temperatuur_gas\": 25.9,\"Temperatuur_vloeistof\": 20.2,\"Stroom\": 2.5,\"Ph\": 7.0,\"Spanning\": 120.0,\"Geleidbaarheid\": 1000.0,\"Flowsensor\": 50.0}}";
int dataSize = jsonPayload.length();
String dataSizeStr = String(dataSize);
String httpDataCommand = "AT+HTTPDATA=" + dataSizeStr + ",5000";

#define GSM_RX_PIN 13
#define GSM_TX_PIN 12
#define GSM_RST_PIN 14

char mqttBroker[] = "5.104.118.202"; //"jrbubuntu.ddns.net"; //"test.mosquitto.org"; 
char mqttPort[] = "1883";
int port = 1883;
char mqttTopic[] = "pee51/gsmesp32"; 
char clientId[] = "esp32_ga6";
char message[300]; 
char test[] = "This is a test.";
EspSoftwareSerial::UART gsmSerial;

void codeForTask1(void *parameter)
{
  Serial.println("Inside Task1.");
  for (;;)
  {            
    // String json = createJson();
    // char jsonStr[300];
    // json.toCharArray(jsonStr, 300);
    // Serial.println("Serial print created Json.");
    // Serial.println(json);
    // Serial.println("Sending Mqtt message.");

    //sendMQTTMessage(clientId, mqttBroker, mqttPort, mqttTopic, jsonStr); 

    //Serial.println("Mqtt message send.");
  //sprintf(message, "{\"ID\":%s, \"Alcohol1\":%.2f,\"Alcohol2\":%.2f,\"LPG1\":%.2f,\"LPG2\":%.2f,\"CH4_1\":%.2f,\"CH4_2\":%.2f,\"H2_1\":%.2f,\"H2_2\":%.2f,\"CO1\":%.2f,\"CO2\":%.2f}", clientId, ppmAl_MQ8, ppmAl_MQ7, ppmLPG_MQ8, ppmLPG_MQ7, ppmCH4_MQ8, ppmCH4_MQ7, ppmH, ppmh2_MQ7, ppmCO_MQ8, ppmCO);
    
   vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
TaskHandle_t Task1;
void listenToGsm(bool toUpperCase)
{
    String inputString;
    while (gsmSerial.available())
    {
        inputString = gsmSerial.readString();
        Serial.println();
        Serial.print("-- Response (");
        Serial.print(inputString.length());
        Serial.println(") --");
        if (toUpperCase)
        {
            inputString.toUpperCase();
        }
        Serial.print(inputString);
    }
}
void listenToGsm()
{
    listenToGsm(false);
}

void initialize_gsm(){
    Serial.println("Configure APN settings.");

    gsmSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\", IP"); //Sets the mode to GPRS
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"APN\"," + apn);   //Set APN parameters
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"USER\"," + apn_User); //Set APN username
    listenToGsm();  
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"PWD\","+ apn_Pass); //Set APN password
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.println("APN settings configured.");

    Serial.println("Configure GPRS settings.");
    gsmSerial.println("AT+SAPBR=1,1"); //Open the carrier with previosuly defined parameters "start command"
    listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=2,1"); //Query the status of previously opened GPRS carrier 
    listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    Serial.println("GPRS settings configured.");
}

void post_http(){
    Serial.println("Post http data...");
    gsmSerial.println("AT+HTTPINIT");       //Initialize HTTP service
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);    
    gsmSerial.println("AT+HTTPPARA=\"CID\",1"); //Define carrier profile zeker
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+HTTPPARA=\"URL\", \"http://jrbubuntu.ddns.net:5000/api/telemetry\""); //Pass URL to be called
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\""); //Define content type
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    gsmSerial.println(httpDataCommand);
    listenToGsm(); 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.println(String(jsonPayload));
    listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    gsmSerial.println("AT+HTTPACTION=1");       //Post HTTP data
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+HTTPREAD");          //Read HTTP response
    listenToGsm();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
void setup() {
 Serial.begin(115200);
  vTaskDelay(500 / portTICK_PERIOD_MS);   
  gsmSerial.begin(9600, SWSERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN, false);
  Serial.println("GSM serial initialized.");

  xTaskCreatePinnedToCore(codeForTask1, "Sensors", 15000,       NULL,         0,            &Task1,         1); // Core: 1
                                       //pcName,  usStackDepth, pvParameters, uxPriority,   pvCreatedTask,  xCoreID
  vTaskDelay(500 / portTICK_PERIOD_MS);            
}

void loop() {
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
      Serial.println("Geen functie ingesteld.");
    }
    if (inputString.startsWith("1"))
    { 
      Serial.println(inputString);
      Serial.println("Running initialize_gsm()");
      initialize_gsm();        
    } 
    if (inputString.startsWith("2"))
    { 
      Serial.println(inputString);
      Serial.println("Running post_http()");
      post_http();        
    }      
    if (inputString.startsWith("3"))
    { 
      Serial.println(inputString);
      Serial.println("Geen functie ingesteld.");
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
  listenToGsm(upperCaseStatus);  
  vTaskDelay(500 / portTICK_PERIOD_MS); 
}




/*


void GA6_init()
{
    Serial.println("GA6_init.");
    Serial.println("Initializing IoT-GA6.");
    // Set APN (Access Point Name) details
    //To set TCP function
    //gsmSerial.println("AT+CGDCONT=1,\"IP\",\"data.lycamobile.nl\"");
    //To set http or FTP
    gsmSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\", IP"); 
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"APN\",\"data.lycamobile.nl\"");  
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"USER\",\"lmnl\""); 
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"PWD\",\"plus\"");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.println("APN settings configured.");

    Serial.println("Activating GPRS (PDP) context.");
    gsmSerial.println("AT+CGACT=1,1");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
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
    gsmSerial.println(" b");
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("Wait for attach.");
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    Serial.println("Start task and set the APN.");
    //gsmSerial.println("AT+CSTT=\"data.lycamobile.nl\", \"lmnl\", \"plus\"");
    //listenToGsm();
    //vTaskDelay(5000 / portTICK_PERIOD_MS);
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

void connectGSMNetwork()
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
            return; // Network registered successfully
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second before retrying
    }    
  else
  {
    Serial.println(" Not registered");
    }
}


void send_http(){
  Serial.println("Set APN settings.");
  gsmSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\", IP"); 
    listenToGsm();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"APN\",\"data.lycamobile.nl\"");  
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"USER\",\"lmnl\""); 
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gsmSerial.println("AT+SAPBR=3,1,\"PWD\",\"plus\"");
    listenToGsm();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.println("APN settings configured.");
  Serial.println("send_http");
  gsmSerial.println("AT+SAPBR=1,1");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+SAPBR=2,1");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+CGATT=1");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPINIT");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPPARA=\"URL\", \"http://jrbubuntu.ddns.net:5000/api/telemetry\"");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPPARA=\"CID\",1"); //Niet zeker
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPACTION=1");
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
//gsmSerial.println("AT+HTTPDATA=55,5000");
gsmSerial.println(httpDataCommand);
 listenToGsm(); 
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println(String(jsonPayload));
 listenToGsm();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
gsmSerial.println("AT+HTTPACTION=1");
  Serial.println("End of send_http");
}


*/