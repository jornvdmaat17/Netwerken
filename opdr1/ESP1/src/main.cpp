#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define BUTTON D2
#define LED D3

#define SSID "<NETWORK SSID>"
#define PASS "<NETWORK PASSWORD>"

#define AIO "io.adafruit.com"
#define AIO_PORT 1883
#define AIO_USERNAME "<ADAFRUIT.IO USERNAME>"
#define AIO_KEY "<SECRET ADAFRUIT.IO KEY>"

#define AIO_LED1 "<ADAFRUIT.IO USERNAME>/feeds/LED1"
#define AIO_LED2 "<ADAFRUIT.IO USERNAME>/feeds/LED2"

bool LED2_state = false;

// Create MQTT and WiFi client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO, AIO_PORT, AIO_USERNAME, AIO_KEY);

// Setup publisher and subcriber
Adafruit_MQTT_Publish LED2 = Adafruit_MQTT_Publish(&mqtt, AIO_LED2);
Adafruit_MQTT_Subscribe LED1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_LED1);

// Function declartion
void MQTT_connect();

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Connect to WiFi
  WiFi.begin(SSID, PASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");    
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  // Subscribe to the topic
  mqtt.subscribe(&LED1);
}

void loop() {
  // Connect to the MQTT server
  MQTT_connect();

  // Read from the subscribed topic and react accordingly by either turning the LED ON or OFF
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10))) {
    if (subscription == &LED1) {
      Serial.print("\nReceived: "); Serial.print((char*)LED1.lastread);
      if(strcmp((char*)LED1.lastread, "ON") == 0){     
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
    }
  }

  // If the button is pressed send ON or OFF to the topic of the other led
  if(!digitalRead(BUTTON)){
    Serial.println("Button pressed");
    LED2_state = !LED2_state;
    !LED2.publish(LED2_state ? "ON" : "OFF");
  }

  // Ping the MQTT server to keep the connection alive
  if(!mqtt.ping()) {
    mqtt.disconnect();
  }
}

void MQTT_connect(){
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  // connect will return 0 for connected
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    retries--;
    if (retries == 0) {
      // After n times retries stop retrying
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
