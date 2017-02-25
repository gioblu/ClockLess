
#include <ClockLess.h>

ClockLess link;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
  link.setPins(8, 9);
  link.begin();
}

void loop() {
  link.update();
  for(uint8_t i = 0; i < CLDL_MAX_LENGTH; i++) {
    Serial.print(link.data[i]);
    Serial.print(" ");
    if(link.data[i] == "B") {
      digitalWrite(13, HIGH);
      delay(2);
      digitalWrite(13, LOW);
      delay(2);
    }
  }
  Serial.println();
}
