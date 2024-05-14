// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int adcPin = 27;

// variable for storing the potentiometer value
float voltage,ecValue,temperature = 25;

#define RES2 820.0
#define ECREF 200.0

float readEC(float voltage, float temperature)
{
    float value = 0;
    value = 1000*voltage/RES2/ECREF;
    value = value / (1.0+0.0185*(temperature-25.0));  //temperature compensation
    return value;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  voltage = analogRead(adcPin);
  ecValue = readEC(voltage, temperature);
  Serial.print(ecValue,2);
  Serial.println("ms/cm");
  delay(500);
}