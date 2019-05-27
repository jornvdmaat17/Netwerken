#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define BUTTON 16
#define LED 5

#define SSID "VRV95175E46E1"
#define PASS "9aYuHkwkGxuw"

#define AIO "io.adafruit.com"
#define AIO_PORT 1883
#define AIO_USERNAME "jvdmaat"
#define AIO_KEY "1d1ecc22ccaa4b61925564a4915c7bf3"

#define AIO_LED1 "jvdmaat/feeds/LED1"
#define AIO_LED2 "jvdmaat/feeds/LED2"

bool LED2_state = false;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO, AIO_PORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish LED2 = Adafruit_MQTT_Publish(&mqtt, AIO_LED2);
Adafruit_MQTT_Subscribe LED1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_LED1);

void MQTT_connect();

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  WiFi.begin(SSID, PASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");    
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  mqtt.subscribe(&LED1);
}

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &LED1) {
      Serial.print("\nReceived: "); Serial.print((char*)LED1.lastread);
      if(strcmp((char*)LED1.lastread, "ON")){        
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
    }
  }


  // if (!LED2.publish("ON")) {
  //   Serial.println(F("Failed"));
  // } else {
  //   Serial.println(F("OK!"));
  // }

  if(! mqtt.ping()) {
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
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}