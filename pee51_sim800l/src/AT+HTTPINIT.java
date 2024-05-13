
void send_http_quicker(){
    Serial.println("Checking network registration...");
    unsigned long startMillis = millis();
    // Wait up to 10 seconds
    if (gsmSerial.println("AT+CREG?"))
    {
        listenToGsm();
        Serial.print("gsmSerial.available(): ");
        Serial.println(gsmSerial.available());
        Serial.print("inputString before: ");
        Serial.println(inputString);
        if (inputString.indexOf("+CREG: 0,1") != -1 || inputString.indexOf("+CREG: 0,5") != -1)
            {
                Serial.println("Registered");
                gsmSerial.println("AT+HTTPSTATUS?")
                listenToGsm();
                Serial.print("gsmSerial.available(): ");
                Serial.println(gsmSerial.available());
                Serial.print("inputString before: ");
                Serial.println(inputString);
                if (inputString.indexOf("+HTTPSTATUS: GET,0,0,0") != -1 || inputString.indexOf("+CREG: 0,5") != -1)
                    {
                        post_http();
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second before retrying
        }
    else
    {
        Serial.println("GSM not registered.");
        Serial.println("Set APN settings.");
        gsmSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\" IP"); 
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+SAPBR=3,1,\"APN\",\"data.lycamobile.nl\"");  
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+SAPBR=3,1,\"USER\",\"lmnl\""); 
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+SAPBR=3,1,\"PWD\",\"plus\"");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.println("APN settings configured.");
        Serial.println("Settings for http connection.");
        gsmSerial.println("AT+SAPBR=1,1");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+SAPBR=2,1");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+CGATT=1");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+HTTPINIT");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+HTTPPARA=\"URL\", \"http://jrbubuntu.ddns.net:5000/api/telemetry\"");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+HTTPPARA=\"CID\",1"); //Niet zeker
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gsmSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
        listenToGsm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }   
  
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

void initialize_gsm(){
    Serial.println("Configure APN settings.");

    gsmSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\" IP"); //Sets the mode to GPRS
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
}
