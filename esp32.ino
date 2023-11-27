#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_AHTX0.h>


//libraries to integrate functionality
#include <WiFi.h> //wifi connection
#include <PubSubClient.h> //MQTT messaging
#include <Arduino.h> //Input and output
String formattedDate;
String dayStamp;
String timeStamp;
// Wi-Fi credentials: replace with those of your network
const char* ssid = "Rethymno";  // The name of the WiFi network
const char* password = "7xynz123"; // The WiFi network passkey
String nite = "0";

// MQTT broker details: replace with your own
const char* mqtt_server = "lunania.local"; // The MQTT broker's hostname or IP address
const int mqtt_port = 1883;  // MQTT broker port (1883 is default)
const char* mqtt_topic = "terra/hum";  // MQTT topic to publish messages
// MQTT client name prefix (will add MAC address)
String name = "ESP32Client_";
unsigned long previousMillis = 0;
const long interval = 5000;
// Create an instance of the WiFiClient class
WiFiClient espClient;
// Create an instance of the PubSubClient class
PubSubClient client(espClient);
Adafruit_AHTX0 aht;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Timer for publishing every 5 seconds
void setup() {
 
  // Start Serial communication

  Serial.begin(115200);
  pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
      pinMode(19, OUTPUT);
        pinMode(32, OUTPUT);
          pinMode(33, OUTPUT);
  // Read the MAC address
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  // Convert MAC address to a string
  char macStr[18]; // MAC address is 12 characters long without separators, plus null terminator
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  // Concatenate the name prefix with the MAC address 
  name = name + macStr;

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set MQTT server and port
  client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
 timeClient.begin();
  timeClient.setTimeOffset(-18000);
 
}

void loop() {
 sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
 while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedTime();
   int splitT = formattedDate.indexOf("T");
  // You may need to map the lightValue to the desired lighting range based on calibration
  // For example, if your photocell resistance values are known for full daylight and darkness, use map() function here.
  // lightValue = map(lightValue, minVal, maxVal, 0, 100); // Map to a 0-100 lighting scale
 unsigned long currentMillis = millis();
 client.loop();
  // Publish a message every 5 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  //Publish the lighting data to the MQTT topic
 String tempMessage = String(int(temp.temperature));
 String humMessage = String(int(humidity.relative_humidity));
client.publish("terra/temp", tempMessage.c_str());
  client.publish(mqtt_topic, humMessage.c_str());
 timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  String timeMessage = timeStamp;
  timeMessage.remove(5,3);
  Serial.println(timeMessage);
   client.publish("terra/time", timeMessage.c_str());
    client.publish("terra/bool", nite.c_str());
  }
  // Connect to MQTT if necessary
  if (!client.connected()) {
    reconnect();
  }
  // Allow the PubSubClient to process incoming messages
 
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
      String messageTemp;
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "terra/mist") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(5, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(5, LOW);
    }
  }

    if (String(topic) == "terra/lite") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("lite on");
      digitalWrite(18, HIGH);
     nite = "0";
     
    }
    else if(messageTemp == "off"){
      Serial.println("lite off");
      digitalWrite(18, LOW);
      nite = "1";
    }
  }

   if (String(topic) == "terra/heat") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("heat on");
      digitalWrite(19, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("heat off");
      digitalWrite(19, LOW);
    }
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("terra/mist");
       client.subscribe("terra/heat");
        client.subscribe("terra/lite");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

