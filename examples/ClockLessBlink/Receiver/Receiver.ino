
#include <ClockLess.h>

ClockLess pair;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
  pair.dataLink.setPins(8, 9);
  pair.dataLink.begin();
  pair.setInitializer(65535);
  pair.setReceiver(receiver);
}

void receiver(uint8_t *payload, uint16_t length) {
  if(payload[0] == '@') {
    Serial.println(payload[0]);
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
    delay(10);
  }
}

void loop() {
  pair.receive();
}
