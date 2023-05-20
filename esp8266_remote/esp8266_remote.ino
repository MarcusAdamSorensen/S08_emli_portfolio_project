// Embedded Linux (EMLI)
// University of Southern Denmark
// ESP8266 Wifi client - Webserver - User interface 
// Kjeld Jensen <kjen@sdu.dk> <kj@kjen.dk>
// 2023-04-18, KJ, First version
// inspired by https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-hosting-a-webserver

// LED
#define PIN_LED_RED     14
#define PIN_LED_YELLOW  13
#define PIN_LED_GREEN   12

// button
//#define PIN_BUTTON      4
//#define DEBOUNCE_TIME 200 // milliseconds
//volatile int button_a_count;
//volatile bool button_pressed = false;
//volatile unsigned long count_prev_time;

// button
#define PIN_BUTTON 4
const long buttonDelay = 2000;
unsigned long currentTime = 0; 
bool buttonState = HIGH;
bool lastButtonState = HIGH; 


// Wifi
#include <ESP8266WiFi.h>
const char* WIFI_SSID = "EMLI_TEAM_07";
const char* WIFI_PASSWORD = "embeddedlinux";

// Static IP address
IPAddress IPaddress(192, 168, 10, 222);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
//IPAddress primaryDNS(1, 1, 1, 1); 
//IPAddress secondaryDNS(8, 8, 8, 8); 

// Webserver
#define CLIENT_TIMEOUT 2000

WiFiServer server(80);
WiFiClient client = server.available();
unsigned long clientConnectTime = 0;
String currentLine = "";
char response_s[10];
char s[25];

// mqtt
#include <PubSubClient.h>
const char* mqttServer = "192.168.10.1";
const int mqttPort = 1883; 
const char* mqttUsername = "team07";
const char* mqttKey = "tjells123";
const char* mqttTopic = "pico/button";

PubSubClient mqtt(client);

//ICACHE_RAM_ATTR void button_a_isr()
//{
//  if (millis() - count_prev_time > DEBOUNCE_TIME)
//  {
//    count_prev_time = millis();
//    button_a_count++;
//    Serial.println("Button pressed");
    
//    button_pressed = true;
//  }
//}

void setup()
{
  // serial
  Serial.begin(115200);
  delay(10);

  // LEDs
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_RED, LOW);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);

  // button
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), button_a_isr, RISING);

  // set the ESP8266 to be a WiFi-client
  WiFi.mode(WIFI_STA); 
  
  // configure static IP address
  WiFi.config(IPaddress, gateway, subnet);
  //WiFi.config(IPaddress, gateway, subnet, primaryDNS, secondaryDNS);

  // connect to Wifi access point
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // mqtt
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(callback);

  // start webserver
  Serial.println("Starting webserver");
  Serial.println("");
  server.begin();
}

int plant_water_alarm = 0;
int pump_water_alarm = 0;
int moisture = 0;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  String message_buffer;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    message_buffer += (char)message[i] ;
  }
  Serial.println();

  if (String(topic) == "pico/data/plant_water_alarm") {
    plant_water_alarm = message_buffer.toInt();
  } else if (String(topic) == "pico/data/pump_water_alarm") {
    pump_water_alarm = message_buffer.toInt();  
  } else if (String(topic) == "pico/data/moisture") {
    moisture = message_buffer.toInt();
  }
}

void mqtt_connect() {
    while (!mqtt.connected()) {
      Serial.println("Connecting to MQTT...");
  
      if (mqtt.connect("ESP32Client", mqttUsername, mqttKey)) {
        Serial.println("Connected to MQTT");
        mqtt.subscribe("pico/data/plant_water_alarm");
        mqtt.subscribe("pico/data/pump_water_alarm");
        mqtt.subscribe("pico/data/moisture");
        mqtt.subscribe("pico/data/light");
      } else {
        Serial.print("Failed with state: ");
        Serial.println(mqtt.state());
        
        delay(2000);  
      }
    }
}

void loop()
{  
  if (!mqtt.connected()) {
      mqtt_connect();
  }
  mqtt.loop();

  if (plant_water_alarm == 1 || pump_water_alarm == 0) {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);  
  } else if (moisture < 60) {
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
  } else {
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH); 
  };
  
  // Reset debounce timer if button state has changed.
  bool reading = digitalRead(PIN_BUTTON); 
  if (reading != lastButtonState) {
    currentTime = millis();
  }

  // Checks whether 2 seconds have passed since last press.
  if ((millis() - currentTime) > buttonDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (mqtt.publish(mqttTopic, "p")) {
          Serial.println("Message published");
        } else {
          Serial.print("State: ");
          Serial.println(mqtt.state());
        }
      }
    }
  }
  lastButtonState = reading;



//  if (button_pressed) {
    //if (mqtt.publish(mqttTopic, "p")) {
//      Serial.println("Message published");
//    } else {
//      Serial.print("State: ");
//      Serial.println(mqtt.state());
//    }

//    button_pressed = false;
  }
}
