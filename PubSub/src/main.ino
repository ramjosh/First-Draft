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
#define mqttLedStateR "home/ledStateR"
#define mqttLedStateG "home/ledStateG"
//---------------------------MQTT Setup End-------------------------------------

#define DHTPIN D2 // DHT Pin
#define DHTTYPE DHT11
const int ledPinR = D4; // LED Pin
const int ledPinG = D0;
bool ledStateR = false; //setting led off initially
bool ledStateG = false; //setting led off initially
bool msgCame = false;
long lastMsg = 0;

DHT dht(DHTPIN, DHTTYPE);

float tempExt, humExt;

//-------------------------setup and loop-------------------------------------
void setup()
{
    Serial.begin(115200);
    Serial.println();
    pinMode(ledPinR, OUTPUT);
    pinMode(ledPinG, OUTPUT);

    connectWifi(); //connect to wifi

    client.setServer(mqttServer, mqttPort); //connect to mqtt server
    client.setCallback(callback);           // setting Callback to callback function
    client.subscribe(mqttLedStateR);        // Subscribe to the topic
    client.subscribe(mqttLedStateG);        // Subscribe to the topic

    dht.begin(); // start dht sensor
    delay(1000);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop(); // checks if there are any messages in the subscribed topic
    if (msgCame)
    {
        if (ledStateR)
        { // check the ledState and toggle the LED accordingly
            digitalWrite(ledPinR, HIGH);
            Serial.println("LedR is turned On");
        }
        else if (!ledStateR)
        { // check the ledState and
            digitalWrite(ledPinR, LOW);
            Serial.println("LedR is turned Off");
        }
        if (ledStateG)
        { // check the ledState and toggle the LED accordingly
            digitalWrite(ledPinG, HIGH);
            Serial.println("LedG is turned On");
        }
        else if (!ledStateG)
        { // check the ledState and
            digitalWrite(ledPinG, LOW);
            Serial.println("LedG is turned Off");
        }
        msgCame = false;
    }

    long now = millis();
    if (now - lastMsg > 1000)
    { //allow to publish the values once in 5 seconds
        lastMsg = now;

        //reads temp and hum and publish them to their respective topics
        getValues();
        client.publish(mqttTempExt, String(tempExt).c_str(), true);
        client.publish(mqttHumExt, String(humExt).c_str(), true);
    }
}
//------------------------wifi connect function----------------------------
void connectWifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(2000);

    WiFi.begin(ssid, password);

    //wait untill wifi is connected

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // wifi is connected

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
//-------------------------------------------------------------------------

//---------------------------callback function-----------------------------

void callback(String topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();
    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
    // Changes the output state according to the message
    if (topic == mqttLedStateR)
    {
        if (messageTemp == "true")
        {
            ledStateR = true;
            Serial.println("LedStateR = true");
        }
        else
        {
            ledStateR = false;
            Serial.println("LedStateR = false");
        }
        msgCame = true;
    }
    if (topic == mqttLedStateG)
    {
        if (messageTemp == "true")
        {
            ledStateG = true;
            Serial.println("LedStateG = true");
        }
        else
        {
            ledStateG = false;
            Serial.println("LedStateR = false");
        }
        msgCame = true;
    }
    Serial.println();
}
//-------------------------------------------------------------------------

//---------------------------getValues function-----------------------------

void getValues()
{
    // reads Temp and Hum and assign values to temExt and humExt
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
//-------------------------------------------------------------------------

//---------------------------reconnect function-----------------------------
void reconnect()
{
    // Loop until we're reconnected

    //int counter = 0;
    while (!client.connected())
    {
        /*if (counter == 5)
        {
            ESP.restart();
        }
        counter += 1;*/
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect

        if (client.connect(mqttClientName, mqttUser, mqttPassword))
        {
            Serial.println("connected");
            client.subscribe(mqttLedStateR); // Subscribe to the topic
            client.subscribe(mqttLedStateG); // Subscribe to the topic
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(1000);
        }
    }
}
//-------------------------------------------------------------------------