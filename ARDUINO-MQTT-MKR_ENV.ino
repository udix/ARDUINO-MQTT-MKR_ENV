#include <SPI.h>
#include <PubSubClient.h>
#include <Arduino_MKRENV.h>
#include <stdio.h>

#include <WiFi101.h>

#include "myWiFiSettings.h"

char ssid[] = SECRET_SSID;            // your network SSID (name)
char pass[] = SECRET_PASS;            // your network password
char server[] = "192.168.1.38";       // Raspberry's IP

// Initialize the Ethernet client object
WiFiClient client;
// Initialize MQTT client object
PubSubClient MQTTClient(client);

void setup() {

  //Initialize serial
  Serial.begin(9600);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // initialize MKR ENV board
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }

  // attempt to connect to Wifi network
  Connect2WiFi();

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  MQTTClient.setServer(server, 1883);

  // Setup finished
  Serial.println("Setup finished!");
  printCurrentNet();
  printWifiData();
}

void loop() {

    // Check if device is connected to WiFi network, if it isn't try to connect
    if (WiFi.status() != WL_CONNECTED) { Connect2WiFi(); }

    char temp[20];
    char hr[20];
    char pres[20];
    char UVA[20];
    char UVB[20];
    char UVidx[20];

    sprintf(temp,  "%f", ENV.readTemperature());
    sprintf(hr,    "%f", ENV.readHumidity());
    sprintf(pres,  "%f", ENV.readPressure());
    sprintf(UVA,   "%f", ENV.readUVA());
    sprintf(UVB,   "%f", ENV.readUVB());
    sprintf(UVidx, "%f", ENV.readUVIndex());

    if (!MQTTClient.connected()) {
        Serial.print("Connecting ...\n");
        Serial.println(MQTTClient.connect("Arduino Client", "mqtt", "123456"));
    } else {
      // Envio
      MQTTClient.publish("/homie/myHouse/weatherStation/temperature", temp);
      MQTTClient.publish("/homie/myHouse/weatherStation/humidity",    hr);
      MQTTClient.publish("/homie/myHouse/weatherStation/pressure",    pres);
      MQTTClient.publish("/homie/myHouse/weatherStation/UVa",         UVA);
      MQTTClient.publish("/homie/myHouse/weatherStation/UVb",         UVB);
      MQTTClient.publish("/homie/myHouse/weatherStation/UVidx",       UVidx);
    }

  // Tiempo entre envios (en ms)
  delay(5000);
}

void printWifiData() {
  // print device's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

//Connect to WiFi network
void Connect2WiFi() {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFi.begin(ssid, pass);

    // wait 10 seconds
    delay(10000);
  }
}