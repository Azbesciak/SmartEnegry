//power usage calculator calibrated on esp8266
//ASC712 5v -----> 5.5 to 3.3 logic converter -----> A(0) ESP8266
#include <Filters.h>
#include <ESP8266WiFi.h>


//==================currrent calculator==================
RunningStatistics inputStats;                 // create statistics to look at the raw test signal
float testFrequency = 60;                     // test signal frequency (Hz)
float windowLength = 20.0/testFrequency;     // how long to average the signal, for statistist
int sensorValue = 0;
const float intercept = -.1261; // to be adjusted based on calibration testing
const float slope = .0198; // to be adjusted based on calibration testing

const int voltage = 230;
unsigned long samplingPeriod = 1000; // in milliseconds
// Track time in milliseconds since last reading 
unsigned long previousMillis = 0;

//==================switch==================
const int switchPin = 0;
const int switchStartValue = 0;


//==================wifi==================
const char * SSID = "";
const char * PASSWORD = "";



//==================led diodes==================
const int blueLED = 4;
const int greenLED = 3;
const int redLED = 2;


void setup() {
  Serial.begin( 57600 );

  setLED();

  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, switchStartValue);
  inputStats.setWindowSecs( windowLength );
  
  connectWiFi();  
}

void loop() {
    sensorValue = analogRead(A0);  // read the analog in value:
    inputStats.input(sensorValue);  // push to Stats function
    
    calculatePowerConsumption();        

    yield();
}


//---------------------------functions------------------------

void setLED()
{
  pinMode(blueLED, OUTPUT);
  digitalWrite(blueLED, LOW);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
}

void connectWiFi()
{
  
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(50);
    yield();
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void calculatePowerConsumption()
{
      if((unsigned long)(millis() - previousMillis) < samplingPeriod)
      return;
      
      previousMillis = millis();   // update time

      
      // convert signal sigma value to current in amps
      double sigma = inputStats.sigma();
      double current_amps = intercept + slope * sigma;
      current_amps = max(0.0, current_amps);  //discard negative readings

      //Serial.println(inputStats.mean());
      // output sigma or variation values associated with the inputValue itsel
      Serial.print( "sigma: " ); 
      Serial.print( sigma );
      Serial.print( "\tamps: " ); 
      Serial.print( current_amps );
      Serial.print("\t watts: ");
      Serial.print( current_amps * voltage);
      Serial.print( "\n" );      
}



