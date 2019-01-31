#include <ESP8266WiFi.h>
#include "SX1278.h"
#include <SPI.h>

#define LORA_MODE  4
#define LORA_CHANNEL1  CH_1_BW_125
#define LORA_CHANNEL4  CH_4_BW_125
#define LORA_CHANNEL7  CH_7_BW_125
#define LORA_ADDRESS  4

int e;
byte multichan = 0;
char my_packet[100];

const char* ssid     = "ModDevLab_2.4GHz";
const char* password = "3313033130A*";

const char* host = "202.44.12.165";

String device_id  = "1000000";
char* device_id_re = "1000000";
String coor = "0000000000000000000000000000000000000000000000000";
char* coor_re = "0000000000000000000000000000000000000000000000000";

void setup() {
  
  Serial.begin(9600);

  //LoRa Reciever
  
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
  switch (multichan) {
    case 0 : sx1278.setChannel(LORA_CHANNEL1); break;
    case 1 : sx1278.setChannel(LORA_CHANNEL4); break;
    case 2 : sx1278.setChannel(LORA_CHANNEL7); break;
    default : break;
  }
  
  if (multichan >= 3) {
    multichan = 0;
  }
  else {
    multichan++;
  }

  e = sx1278.receivePacketTimeout(1900);
  if (e == 0) {
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      if (i <= 6) {
        device_id_re[i] = (char)sx1278.packet_received.data[i];
        }
        else {
          coor_re[i-7] = (char)sx1278.packet_received.data[i];
          }
    }

    Serial.print(F("Message: "));
    Serial.println(my_packet);

    device_id = String(device_id_re);
    coor = String(coor_re);
    
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
  
  String url = "/LoRaGateway/fwd.php?device_id=" + device_id + "&coor=" + coor;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 4000) {
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

  Serial.println();
  Serial.println("closing connection");
}
