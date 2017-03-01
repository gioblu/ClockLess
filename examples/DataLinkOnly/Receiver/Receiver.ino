
#include <ClockLess.h>

ClockLessDataLink link;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
  link.setPins(8, 9);
  link.begin();
}

void loop() {
  int receivedByte = link.receive();
  if(receivedByte > 0) {
    Serial.print((char)receivedByte);
    if(receivedByte == 'o') Serial.println();

    if(receivedByte == 'o') {
      digitalWrite(13, HIGH);
      delay(1);
      digitalWrite(13, LOW);
    }
  }
}
