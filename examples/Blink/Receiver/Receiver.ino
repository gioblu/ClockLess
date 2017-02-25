//#define CLDL_DEBUG true

#include <ClockLess.h>

ClockLessDataLink link;
uint32_t time;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
  link.setPins(8, 9);
  link.begin();
  time = micros();
}

void loop() {
  int receivedByte = link.receive();
  if(receivedByte > 0) {
    Serial.print((char)receivedByte);
    Serial.println(" <- RECEIVED");
  }
}
