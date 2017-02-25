#define CLDL_DEBUG true

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
  link.receive();
  if(micros() - time > 1000000) {
  for(uint8_t i = 0; i < CLDL_MAX_LENGTH; i++) {
    Serial.print(" ");
    Serial.print((char)link.data[i]);
    time = micros();
  }
  Serial.println();
  }
}
