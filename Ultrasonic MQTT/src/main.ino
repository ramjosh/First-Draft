#include <ESP8266WiFi.h>
#include <Servo.h>

const char *ssid = "JAILAVANYA";
const char *password = " ";

#include <PubSubClient.h>

const char *mqttServer = "192.168.0.108";
const int mqttPort = 1883;
const char *mqttUser = "mqtt2020";
const char *mqttPassword = "mqtt2020";
const char *mqttClientName = "ultrasonic sensor";
#define mqttRadar "home/radarData"

WiFiClient ULTRASONICSENSOR;
PubSubClient client(ULTRASONICSENSOR);

long duration;
int distance;
const int trigPin = 9;
const int echoPin = 10;
int deg, dist;
String radarData;
Servo myservo;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  Serial.println();
  connect();
  myservo.attach(9);
  client.setServer(mqttServer, mqttPort);
  delay(100);
}

void connect()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("=");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  int counter = 0;
  while (!client.connected())
  {
    if (counter == 5)
    {
      ESP.restart();
    }
    counter += 1;
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientName, mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  for (int i = 0; i < 180; i++)
  {
    myservo.write(i);
    delay(15);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    dist = duration * 0.034 / 2;
    Serial.print("Distance:");
    Serial.println(dist);
    sprintf(radarData, "{\"deg\":\"%s\",\"dist\":\"%s\"}", i, dist);
    client.publish(mqttRadar, String(radarData).c_str(), true);
  }
  for (int i = 179; i >= 0; i--)
  {
    myservo.write(i);
    delay(15);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    dist = duration * 0.034 / 2;
    Serial.print("Distance:");
    Serial.println(dist);
    sprintf(radarData, "{\"deg\":\"%s\",\"dist\":\"%s\"}", i, dist);
    client.publish(mqttRadar, String(radarData).c_str(), true);
  }
}
