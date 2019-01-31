#include "SX1278.h"
#include <SPI.h>

#define LORA_MODE  4
#define LORA_CHANNEL  CH_5_BW_125 //CH_1_BW_125, CH_4_BW_125, CH_7_BW_125 แต่ละตัวเลือกได้แค่ 1 ใน 3 CH ห้ามซ้ำกัน
#define LORA_ADDRESS  2

int e;

char* device_id = "0002"; //ตั้งค่า Device id
char* coordinate = "000000000000000000000000000000000000000000000"; //ค่าพิกัดเริ่มต้น หากโปรแกรมทำงานผิดพลาดจะส่งค่านี้ไป
char message [49] = {0};

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Print a start message
  Serial.println(F("sx1278 module and Arduino: send two packets (One to an addrees and another one in broadcast)"));

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

  // Select frequency channel
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
  } else {
    Serial.println(F("Setting Channel: ERROR "));
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

  // Print a success message
  Serial.println(F("sx1278 configured finished"));
  Serial.println();
}

void loop(void)
{
  coordinate = "000CF15698AD000CF15698AD000CF15698AD045070079"; //ค่าที่ได้จาก Wifi 45B
  for (byte i = 0; i <= 48; i++) {
    message [i] = {0};
    }
  strcat(message, device_id);
  strcat(message, coordinate);

  // Send message2 broadcast and print the result
  e = sx1278.sendPacketTimeout(0, message);
  Serial.print(F("Packet sent, state "));
  Serial.println(e, DEC);
  Serial.println(message);
  delay(2300);
}
