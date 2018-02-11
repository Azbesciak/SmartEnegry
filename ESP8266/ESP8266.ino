//power usage calculator calibrated on esp8266
//ASC712 5v -----> 5.5 to 3.3 logic converter -----> A(0) ESP8266

#include <Filters.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>

#define DEBUG 1
#define PUB_DEBUG 0
#define POWER_DEBUG 1


//==================pins mapping==================
/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
 */

//==================currrent calculator==================
RunningStatistics inputStats;                 // create statistics to look at the raw test signal
float testFrequency = 60;                     // test signal frequency (Hz)
float windowLength = 20.0 / testFrequency;   // how long to average the signal, for statistist
int sensorValue = 0;

const int sigma_first_threshold = 20.0;
const float intercept_first = -.0245; // to be adjusted based on calibration testing
const float slope_first = .0185; // to be adjusted based on calibration testing


const int sigma_second_threshold = 50.0;
const float intercept_second = -.2942; // to be adjusted based on calibration testing
const float slope_second = .0192; // to be adjusted based on calibration testing

//sigma > 50
const float intercept = -.0781; // to be adjusted based on calibration testing
const float slope = .0179; // to be adjusted based on calibration testing

const int voltage = 230;
unsigned long sampling_period = 1000; // in milliseconds
// Track time in milliseconds since last reading
unsigned long previousMillis = 0;

//==================switch==================
const int switch_pin = D1;
const int switch_start_value = 0;
const int min_switching_period = 500;


//==================wifi==================
WiFiClient wifiClient;
const char * SSID = "";
const char * PASSWORD = "";
const int wait_for_connection_millis = 100;


//==================mqtt==================
PubSubClient mqtt_client(wifiClient);
const int mqtt_reconnect_timeout = 2000;
const char* mqtt_server = "192.168.1.10";  //rpi address
const int mqtt_buffer_size = 100;
const int my_ID = 1;
//channels
const char* switch_topic_sub = "/switches/request";
const char* switch_topic_pub = "/switches/response";

const char* power_consumption_topic_pub = "/power/response";

const char* switch_state_topic_sub = "/switches/state/request";
const char* switch_state_topic_pub = "/switches/state/response";

//==================led diodes==================
const int BLUE_LED = D4;
const int GREEN_LED = D3;
const int RED_LED = D2;

const int main_loop_delay = 10;
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

  double power = calculate_power_consumption(); //if time between two invokes is too short returns -1
  if (power > -1)
  {
    mqtt_publish(power_consumption_topic_pub, power);
  }

  mqtt_client.loop();
  delay(main_loop_delay);
  //non-blocking delay
  /*
  long cur_millis = millis();
  while(millis() - cur_millis < main_loop_delay)
  {
    yield();//give esp8266 some time
  }
  */
}


//---------------------------functions------------------------

void setLED()
{
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);

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
    while (millis() - mils < wait_for_connection_millis)
    {
      digitalWrite(~digitalRead(LED_BUILTIN), LOW);
      yield(); //leave thread
    }
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(GREEN_LED, LOW);
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
  if (strcmp(topic, switch_topic_sub) == 0)
  {
    if (char(payload[0]) == '1')
    {
      Serial.println("Switching on device");
      digitalWrite(switch_pin, LOW);
      mqtt_publish(switch_topic_pub, "1;OK");
    } else if (char(payload[0]) == '0')
    {
      Serial.println("Switching off device");
      digitalWrite(switch_pin, HIGH);
      mqtt_publish(switch_topic_pub, "0;OK");
    }
  } else if (strcmp(topic, switch_state_topic_sub) == 0)
  {
    Serial.println("Server wants to know switch state.");
    mqtt_publish(switch_state_topic_pub, digitalRead(switch_pin));
  } else
  {
    Serial.println("Not my topic");
  }
}
//invoked by mqtt_publish before publish
bool check_mqtt_connection()
{
  if (!mqtt_client.connected()) {
    return mqtt_reconnect();
  }
  return 1;
}

void mqtt_publish(const char* topic, int value)
{
  char buffer[10];
  snprintf(buffer, 10, "%d", value);
  mqtt_publish(topic, buffer);
}
void mqtt_publish(const char* topic, double value)
{
  char buf[10];
  snprintf(buf, 10, "%.2f", value);
  mqtt_publish(topic, buf);
}


void mqtt_publish(const char* topic, char * message)
{
  if (!check_mqtt_connection())
  {
    return;
  }

  //mqtt - reconnect if needed and publish power consumption value
  char buffer[mqtt_buffer_size];
  memset(buffer, 0, mqtt_buffer_size);
  snprintf (buffer, mqtt_buffer_size, "%d:%s", my_ID, message);
  int retr = mqtt_client.publish(topic, buffer);
  //retr == 1 success, otherwise failed to send
#if PUB_DEBUG
  Serial.println(my_ID);
  Serial.print( "\t>");
  Serial.print(message);
  Serial.print( "<\n");

  Serial.print("Publish message: ");
  Serial.print(buffer);
  Serial.print("\nto topic ");
  Serial.println(topic);
  Serial.print("\t publish result ");
  Serial.println(retr);
#endif
  digitalWrite(BLUE_LED, HIGH);
  delay(40);
  digitalWrite(BLUE_LED, LOW);
}

bool mqtt_reconnect() {
  int max_connecting_attemts = 3;
  while (!mqtt_client.connected()) {
    if (!max_connecting_attemts)
    {
      break;
    }

    Serial.print("Attempting MQTT connection");
    if (mqtt_client.connect("ESP8266Client")) {
      mqtt_client.subscribe(switch_state_topic_sub);
      mqtt_client.subscribe(switch_topic_sub);
      
      digitalWrite(RED_LED, LOW);

#if DEBUG
      Serial.println("...\tmqtt connected");
      Serial.print("Subscribed to: ");
      Serial.print(switch_state_topic_sub);
      Serial.print(", ");
      Serial.print(switch_topic_sub);
      Serial.println();
#endif

      return 1;
    } else {
      digitalWrite(RED_LED, HIGH);
      max_connecting_attemts--;
#if DEBUG
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.print(" try again in ");
      Serial.print(mqtt_reconnect_timeout / 1000);
      Serial.println(" seconds.");
#endif

      long mils = millis();
      while (millis() - mils < mqtt_reconnect_timeout )
      {
#if DEBUG
        Serial.print(".");
#endif
        yield();
      }
    }
  }
#if DEBUG
  Serial.print("Couldn't connect to broker for ");
  Serial.print(max_connecting_attemts);
  Serial.println(" times.");
#endif
  return 0; //failed to connect for max_connecting_attmept times
}
double calculate_power_consumption()
{
  //calculate power consumption only when sampling_period passed
  if ((unsigned long)(millis() - previousMillis) < sampling_period)
    return -1;

  previousMillis = millis();   // update time

  // convert signal sigma value to current in amps
  double sigma = inputStats.sigma();
  double current_amps = 0;
  
  if(sigma > sigma_second_threshold)
  {
    current_amps = intercept + slope * sigma;
  } else if(sigma > sigma_first_threshold)
  {
    //sigma >20 && <=50
    current_amps = intercept_second + slope_second * sigma;
  }else
  {
    //sigma <= 20
    current_amps = intercept_first + slope_first * sigma;
  }
  
  current_amps = max(0.0, current_amps);  //discard negative readings
  
  double power = current_amps * voltage;
#if POWER_DEBUG
  Serial.print( "sigma: " );
  Serial.print( sigma );
  Serial.print( "\tamps: " );
  Serial.print( current_amps );
  Serial.print("\t watts: ");
  Serial.print( current_amps * voltage);
  Serial.print( "\n" );

  Serial.print("in power consumption function ");
  Serial.println(power);
#endif
  return power;
}




