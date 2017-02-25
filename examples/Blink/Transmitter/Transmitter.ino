#define CLDL_DEBUG true 
#include <ClockLess.h>

ClockLessDataLink link;

void setup() {
  Serial.begin(115200);
  link.setPins(8, 9);
  link.begin();
  link.sendString((uint8_t *)"ciao", 4);
}

void loop() {
  link.transmit();
}
