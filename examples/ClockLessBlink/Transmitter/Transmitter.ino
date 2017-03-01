#include <ClockLess.h>

ClockLess pair;
uint32_t time;

void setup() {
  Serial.begin(115200);
  pair.dataLink.setPins(8, 9);
  pair.dataLink.begin();
  pair.setInitializer(655356);
  time = millis();
}

void loop() {
  while((uint32_t)(millis() - time) > 1000) {
    pair.sendPacket((uint8_t *)"@", 1);
    time = millis();
  }
  pair.dataLink.transmit();
}
