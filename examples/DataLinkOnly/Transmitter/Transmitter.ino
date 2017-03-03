#include <ClockLess.h>

ClockLessDataLink link;
uint32_t time;

void setup() {
  Serial.begin(115200);
  link.setPins(8, 9);
  link.begin();
  time = millis();
}

void loop() {
  while((uint32_t)(millis() - time) > 1000) {
    if(link.canStart())
      link.sendString((uint8_t *)"ciao\n", 4);
    time = millis();
  }
  link.transmit();
}
