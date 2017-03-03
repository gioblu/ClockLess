#include <ClockLess.h>

ClockLess pair;
uint32_t time;

void setup() {
  Serial.begin(115200);
  pair.dataLink.setPins(8, 9);
  pair.dataLink.begin();
  time = millis();
}

void loop() {
  pair.sendPacket((uint8_t *)"@", 1);
  pair.dataLink.transmit();
}
