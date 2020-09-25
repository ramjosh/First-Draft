
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include<SoftwareSerial.h>
#define DHTPIN 2
#define DHTTYPE DHT22

#include<TinyGPS.h>
#include <WiFi.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

static const int RXPin = 4, TXPin = 5;  
static const uint32_t GPSBaud = 9600; 
TinyGPSPlus gps; 
TinyGPSLocation location(int,float,float,const char*);

 

DHT dht(DHTPIN,DHTTYPE);
int spd;
int sats;
float lngExt;
float latExt;
const char* mqttLatExt;
const char* mqttLngExt;
int checkGPS;

String bearing;
int move_index;
SoftwareSerial gpsSerial(RXPin,TXPin); 
 char *ssid = "JAILAVANYA";
 char *password = "imrich325@@DRDO";

 #include <PubSubClient.h>
const char *mqttServer = "192.168.0.103";
const int mqttPort = 1883;
const char *mqttUser = "mqtt";
const char *mqttPassword = "mqtt";
const char *mqttClientName = "nodemcu";//

WiFiClient nodemcu;
PubSubClient client(nodemcu);

#define mqttTempExt "home/latExt"
#define mqttHumExt "home/lngExt"
                                  




  

void setup()
{
  Serial.begin(115200);
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  Serial.println();
  
 
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

  dht.begin();
  delay(5000);

}
//void Checkgps()
void getValues()
{
//while(gpsSerial.available())
  //{
   //if(gps.encode(gpsSerial.read()))
   //{
    // gps.f_get_position(&latExt,&lngExt);
   ////   //Serial.print("POsiiton");
     // Serial.print("lat:");Serial.print(latExt);
     // Serial.print(" ");
      //Serial.print("lon");
      //Serial.print(lngExt);
      //}
    

  latExt = gps.location.lat();
  lngExt = gps.location.lng();

  Serial.print("Ext Lat = ");
  Serial.print(latExt);
  Serial.println(" *C");

  Serial.print("Ext Longitude = ");
  Serial.print(lngExt);
  Serial.println(" %");

  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected

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
  getValues();

  client.publish(mqttLatExt, String(latExt).c_str(), true);
  client.publish(mqttLngExt, String(lngExt).c_str(), true);
  delay(1000);
}

void displayInfo()
{ 
 

  if (gps.location.isValid() ) 
  {
    
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    
    location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
     
       sats = gps.satellites.value();    //get number of satellites
       
       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction 
  }
  
  Serial.println();
}