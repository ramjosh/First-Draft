#include <Servo.h>
#include <PubSubClient.h>
#include <WiFi.h> //#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h> //check
char *ssid = "JAILAVANYA";
char *password = "imrich325@@DRDO";

const char *mqttServer = "192.168.0.103";
const int mqttPort = 1883;
const char *mqttUser = "mqtt";
const char *mqttPassword = "mqtt";
const char *mqttClientName = "ultrasonic sensor";

WiFiClient ultrasonicsensor;
PubSubClient client(ultrasonicsensor);

#define mqttTempExt "home/latExt" //change topic and variable
#define mqttHumExt "home/lngExt"  //change topic and variable
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distinCM;
Servo radarServo;
float lngExt;           //------
float latEx           //------
const char *mqttLatExt; //-------
const char *mqttLngExt; //-------

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  radarServo.attach(11);

  // void connectWifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
  delay(5000);
}
void reconnect()
{
  // Loop until we're reconnected
  //------------How to restart Nodemcu
  int counter = 0;
  while (!client.connected())
  {
    //    if (counter == 5)
    //    {
    //      ESP.restart();
    //    }
    //    counter += 1; wait i will do
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect(mqttClientName, mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
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
  client.publish(mqttLatExt, String(latExt).c_str(), true); // CHange respective topics
  client.publish(mqttLngExt, String(lngExt).c_str(), true); //---do--
  delay(1000);

  for (int i = 0; i <= 360; i++)
  {
    radarServo.write(i);
    delay(50);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distinCM = duration * 0.034 / 2;
    Serial.print(i);
    Serial.print("");
    Serial.print(distinCM);
    Serial.print("#");
  }
  for (int i = 360; i >= 0; i--)
  {
    radarServo.write(i);
    delay(50);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distinCM = duration * 0.034 / 2;
    Serial.print(i);
    Serial.print("");
    Serial.print(distinCM);
    Serial.print("#");
  }
}


// Format to send data to mqtt
//{"deg": 180,"dis":30}
//sprintf(data, "{"deg": %s,"dis":%s}", degree, distance);
//Use stepper motor
