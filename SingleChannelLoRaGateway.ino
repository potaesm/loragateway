#include <ESP8266WiFi.h>
#include "SX1278.h"
#include <SPI.h>

#define LORA_MODE  4
#define LORA_CHANNEL  CH_1_BW_125
#define LORA_ADDRESS  4

int e;

const char* ssid     = "ModDevLab_2.4GHz";
const char* password = "3313033130A*";

const char* host = "202.44.12.165";

char* device_id = "0000";
char my_packet[49];

void setup() {
  
  Serial.begin(9600);
  
  // Power ON the module
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));
  } else {
    Serial.println(F("Setting power ON: ERROR "));
  }

  // Set transmission mode and print the result
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
  } else {
    Serial.println(F("Setting Mode: ERROR "));
  }

  // Set header
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
  }

  // Set CRC
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
  } else {
    Serial.println(F("Setting Power: ERROR "));
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
  } else {
    Serial.println(F("Setting node address: ERROR "));
  }

  //Channel select
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting channel: SUCCESS "));
  } else {
    Serial.println(F("Setting channel : ERROR "));
  }

  //Connect to MySQL
  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
  //LoRa Reciever
  e = sx1278.receivePacketTimeout(1900);
  if (e == 0) {
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      }
    for (unsigned int i = 0; i <= 3 ; i++) {
      device_id[i] = my_packet[i];
      }
    Serial.print(F("Message: "));
    Serial.println(my_packet);
  }
  else {
    Serial.print(F("Package received ERROR\n"));
  }
  
  //Connect to MySQL

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  String url = "/LoRaGateway/fwd.php?device_id=" + String(device_id) + "&coor=" + my_packet;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 6000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println(" --> closing connection");
}
