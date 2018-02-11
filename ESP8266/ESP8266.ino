//power usage calculator calibrated on esp8266
//ASC712 5v -----> 5.5 to 3.3 logic converter -----> A(0) ESP8266
#include <Filters.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DEBUG 1

//==================currrent calculator==================
RunningStatistics inputStats;                 // create statistics to look at the raw test signal
float testFrequency = 60;                     // test signal frequency (Hz)
float windowLength = 20.0/testFrequency;     // how long to average the signal, for statistist
int sensorValue = 0;
const float intercept = -.1261; // to be adjusted based on calibration testing
const float slope = .0198; // to be adjusted based on calibration testing

const int voltage = 230;
unsigned long sampling_period = 1000; // in milliseconds
// Track time in milliseconds since last reading 
unsigned long previousMillis = 0;

//==================switch==================
const int switch_pin = 0;
const int switch_start_value = 0;


//==================wifi==================
WiFiClient wifiClient;
const char * SSID = "";
const char * PASSWORD = "";
const int wait_for_connection_millis = 100;


//==================mqtt==================
PubSubClient mqtt_client(wifiClient);
const char* mqtt_server = "192.168.1.23";  //rpi address
const char* switch_topic = "/switches/switch1";
const char* power_consumption_topic = "/power/switch1";
const int publish_period = 100;
const int mqtt_buffer_size = 100;
const int my_ID = 1;

//==================led diodes==================
const int blueLED = 4;
const int greenLED = 3;
const int redLED = 2;


void setup() {
  Serial.begin( 57600 );

  setLED();
  
  //init switch
  pinMode(switch_pin, OUTPUT);
  digitalWrite(switch_pin, switch_start_value);
  //init statistics
  inputStats.setWindowSecs( windowLength );
  
  init_WiFi();  
  init_mqtt();
}

void loop() {
    sensorValue = analogRead(A0);  // read the analog in value:
    inputStats.input(sensorValue);  // push to Stats function

    if(check_mqtt_connection())
    {
        mqtt_client.loop();
        double power = calculate_power_consumption();       
        //double power = rand() % 10;
        if(power > -1)
        {  
           mqtt_publish(power_consumption_topic, power); 
        }
    }
    delay(200);
    yield();  //give esp8266 some time
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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void init_WiFi()
{
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
      long mils = millis();
      while(millis() - mils < wait_for_connection_millis)
      {
        digitalWrite(~digitalRead(LED_BUILTIN), LOW);
        yield(); //leave thread
      }
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW);
}

void init_mqtt()
{
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message in topic [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println("arrived. Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(strcmp(topic, switch_topic) == 0)
  {
      if(char(payload[0]) == '1')
      {
        Serial.println("Switching on switch");
      }else
      {
        Serial.println("Switching off switch");
      }
  } else if(strcmp(topic, power_consumption_topic) == 0)
  {
      Serial.println("Give power consumption value.");
  }else
  {
      Serial.print("Not my topic");
  }
}
bool check_mqtt_connection()
{
  if (!mqtt_client.connected()) {
    return mqtt_reconnect();
  }
  return 1;
}

void mqtt_publish(const char* topic, int value)
{
  value = value % 10;
  mqtt_publish(topic, (char)(value + 48));
}
void mqtt_publish(const char* topic, double value)
{
  char buf[7];
  snprintf(buf, 7, "%7.2f", value);
  mqtt_publish(topic, buf);
}

int value = 0;
char msg[75];
void mqtt_publish(const char* topic, char * message)
{
    //mqtt - reconnect if needed and publish power consumption value

    char buffer[mqtt_buffer_size];
    memset(buffer, 0, mqtt_buffer_size);
    Serial.println(my_ID);
    Serial.print( "\t>");
    Serial.print(message);
    Serial.print( "<\n");
    snprintf (buffer, mqtt_buffer_size, "%d:>%s<", my_ID, message);
    Serial.print("Publish message: ");
    Serial.print(buffer);
    Serial.print("\nto topic ");
    Serial.println(topic);
    int retr = mqtt_client.publish(topic, buffer);
    Serial.print("\t publish result ");
    Serial.println(retr);
   
}
const int timeout = 2000;
bool mqtt_reconnect() {
  int tries = 3;
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client.connect("ESP8266Client")) {
      Serial.println("...\tmqtt connected");
      mqtt_client.publish("outTopic", "hello world");
      // ... and resubscribe
      mqtt_client.subscribe("/sth/example");
      return 1;
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.print(" try again in ");
      Serial.print(timeout / 1000);
      Serial.println(" seconds.");
      long mils = millis();
      while(millis() - mils < timeout)
      {
        delay(100);
        yield();
      }
      Serial.println();
    }
  }
  return 0;
}
double calculate_power_consumption()
{
      //calculate power consumption only when sampling_period passed
      if((unsigned long)(millis() - previousMillis) < sampling_period)
        return -1;
      
      previousMillis = millis();   // update time

      // convert signal sigma value to current in amps
      double sigma = inputStats.sigma();
      double current_amps = intercept + slope * sigma;
      current_amps = max(0.0, current_amps);  //discard negative readings

      double power = current_amps / voltage;
      #ifdef DEBUG
      Serial.print( "sigma: " ); 
      Serial.print( sigma );
      Serial.print( "\tamps: " ); 
      Serial.print( current_amps );
      Serial.print("\t watts: ");
      Serial.print( current_amps * voltage);
      Serial.print( "\n" );
      
      Serial.print("in power consumption function ");
      Serial.print(current_amps);
      Serial.println(power);
      #endif
      return power;
}




