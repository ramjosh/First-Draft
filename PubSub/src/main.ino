#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>


const char *ssid = "JAILAVANYA";
const char *password = "imrich325@@DRDO";

//-------------------------MQTT Setup Start----------------------------------
#include <PubSubClient.h>
const char *mqttServer = "192.168.0.108";
const int mqttPort = 1883;
const char *mqttUser = "mqtt2020";
const char *mqttPassword = "mqtt2020";
const char *mqttClientName = "esp32";

WiFiClient esp32_msr;
PubSubClient client(esp32_msr);

#define mqttTempExt "home/tempExt"
#define mqttHumExt "home/humExt"
//---------------------------MQTT Setup End-------------------------------------

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float tempExt, humExt;
//---------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println();

  //-----------------------------begin Wifi connect------------------------------------------
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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
  //-------------------------------end Wifi connect-------------------------------------------

  client.setServer(mqttServer, mqttPort);

  dht.begin();
  delay(5000);
}

void getValues()
{

  tempExt = dht.readTemperature();
  humExt = dht.readHumidity();

  Serial.print("Ext Temp = ");
  Serial.print(tempExt);
  Serial.println(" *C");

  Serial.print("Ext Humidity = ");
  Serial.print(humExt);
  Serial.println(" %");

  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected

  int counter = 0;
  while (!client.connected())
  {
    if (counter == 5)
    {
      ESP.restart();
    }
    counter += 1;
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

  client.publish(mqttTempExt, String(tempExt).c_str(), true);
  client.publish(mqttHumExt, String(humExt).c_str(), true);
  delay(1000);
}