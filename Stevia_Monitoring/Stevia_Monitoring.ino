#include <WiFi.h>
#include <PubSubClient.h>

#define moisture_1 34
#define moisture_2 35
#define moisture_3 32

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883; 
const char* mqtt_user = "panpan";
const char* mqtt_password = "Panpan13";
const char* mqtt_publish_topic = "topic/panjipanjipanji";
const char* mqtt_subscribe_topic = "topic/panjisprinklerpanji";
const float voltageToPhScale = -4.34; 
const float voltageOffset = 17.741;
const float thresholdHumidity = 60;
const int AirValue = 2550;
const int WaterValue = 950;
const int pumpPin = 23; 
const int phPin = 33;
int counterPublish = 0;
int countSprinkler = 0;
float sensorHumidityValue;
bool relayState = false;
bool msgMqtt = false;

String messagePublish = "";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String incomingMessage;
  for (unsigned int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(incomingMessage);
  if (String(incomingMessage) == "1") {
    msgMqtt = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); 
  pinMode(phPin, INPUT);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.print("Connecting to MQTT broker");
  while (!client.connected()) {
    Serial.print(".");
    if (client.connect("ESP32ClientBSIPTAS", mqtt_user, mqtt_password)) {
      Serial.println("\nConnected to MQTT broker");
      client.subscribe(mqtt_subscribe_topic); 
    } else {
      Serial.print(" failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    Serial.print("Reconnecting to MQTT broker");
    while (!client.connected()) {
      if (client.connect("ESP32ClientBSIPTAS", mqtt_user, mqtt_password)) {
        Serial.println("\nReconnected to MQTT broker");
        client.subscribe(mqtt_subscribe_topic); 
      } else {
        Serial.print(" failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }
  client.loop();

  int moisture1 = analogRead(moisture_1);
  int moisture2 = analogRead(moisture_2);
  int moisture3 = analogRead(moisture_3);

  float avgMoisture = (moisture1 + moisture2 + moisture3) / 3;
  float humidityPercent = map(avgMoisture, WaterValue, AirValue, 100, 0);
  humidityPercent = constrain(humidityPercent, 0, 100);

  int analogValue = analogRead(phPin);
  float voltage = analogValue * (3.3 / 4095.0); 
  float phValue = (voltage * voltageToPhScale) + voltageOffset;

  if (humidityPercent < thresholdHumidity || msgMqtt) {
    publishMessage(humidityPercent, phValue);
    digitalWrite(pumpPin, HIGH); 
    relayState = true;
    Serial.println("Relay is ON and Pump is ON");
    
    for (int i = 0; i < 10; i++) {
      Serial.println(i);
      delay(1000);
    }
    digitalWrite(pumpPin, LOW); 
    relayState = false;
    msgMqtt = false;
    Serial.println("Relay is OFF and Pump is OFF after 10 s");
  }

  if (counterPublish > 9) {
    publishMessage(humidityPercent, phValue);
    counterPublish = 0;
  }
  delay(1000);
  counterPublish++;
  Serial.print("Count Second: ");
  Serial.println(counterPublish);
}

void publishMessage(float humidity, float ph) {
  messagePublish = "Humidity = " + String(humidity) + "% \nPH = " + String(ph);

  Serial.print("Publishing message: ");
  Serial.println(messagePublish);
  if (client.publish(mqtt_publish_topic, messagePublish.c_str())) {
    Serial.println("Message published");
  } else {
    Serial.println("Message failed to publish");
  }
}
