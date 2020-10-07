
#include<ESP8266WiFi.h>
#include<NewPing.h>
#include<Stepper.h>

const char *ssid = "JAILAVANYA";
const char *password = " ";

#include<PubSubClient.h>

const char *mqttServer = "192.168.0.108";
const int mqttPort = 1883;
const char *mqttUser = "mqtt2020";
const char *mqttPassword = "mqtt2020";
const char *mqttClientName = "ultrasonic sensor";
WiFiClient ULTRASONICSENSOR;
PubSubClient client(ULTRASONICSENSOR);

#define ultrangle "home/deg"
#define ultradist "home/dist"
#define mqttRadar "home/radarData"

const float STEPS_PER_REV = 32;
const float GEAR_RED = 64;
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
int StepsRequired = 2048;
Stepper steppermotor(STEPS_PER_REV,8,10,9,11);
long duration;
int distance;
const int trigPin = 9;
const int echoPin = 10; 
int deg,dist;
String radarData;

void setup()
{
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.begin(9600);
Serial.println();
connect();
client.setServer(mqttServer, mqttPort);
delay();
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

void getValues()
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
dist = duration * 0.034 / 2;
Serial.print("Distance:");
Serial.println(dist);
if(dist>=10)
{
steppermotor.setSpeed(700); 
steppermotor.step(StepsRequired);
deg=StepsRequired/360;
Serial.println("angle");
Serial.println(deg);
}
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
getValues();
client.publish(mqttRadar,String(radarData).c_str(),true);
delay(1000);
sprintf(radarData,"{"deg":"%s","dist":"%s"}",ultrangle,ultradist);
}
