#include <ClockLess.h>

ClockLessDataLink link;

void setup() {
  Serial.begin(115200);
  link.setPins(8, 9);
  link.timeIn = 1;
  link.begin();
} 

void loop() {
  link.sendString((uint8_t *)"ciao\n", 4);
  link.transmit();
}
