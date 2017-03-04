
#include <ClockLess.h>

ClockLess pair;

void setup() {
  pair.dataLink.setPins(8, 9);
  pair.dataLink.begin();
}

void loop() {
  pair.receive();
}
