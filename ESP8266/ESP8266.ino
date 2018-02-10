//power usage calculator calibrated on esp8266
//ASC712 5v -----> 5.5 to 3.3 logic converter -----> A(0) ESP8266
#include <Filters.h>


//==================currrent calculator
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

void setup() {
  inputStats.setWindowSecs( windowLength );
  Serial.begin( 57600 );    // start the serial port
}

void loop() {
    sensorValue = analogRead(A0);  // read the analog in value:
    inputStats.input(sensorValue);  // push to Stats function
    
    calculatePowerConsumption();        

    yield();
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



