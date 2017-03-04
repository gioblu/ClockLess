#include <ClockLess.h>

ClockLess pair;
uint32_t time;
uint16_t packetsDelivered = 0;
uint32_t interval = 1000000; // 1 second

void setup() {
  Serial.begin(115200);
  pair.dataLink.setPins(8, 9);
  pair.dataLink.begin();
  time = micros();
  Serial.println("Starting 1 second test...");
  Serial.println("transmitting continuously 20 bytes dummy packet to the other end...");
}

void loop() {
  if(pair.sendPacket((uint8_t *)"01234567890123456789", 20) == CLDL_TRANSMITTING)
    packetsDelivered++;
  pair.dataLink.update();

  if(micros() - time > interval) {
    Serial.print("Overhead: ");
    Serial.println(pair.packetOverhead(0));
    Serial.print("Packets delivered: ");
    Serial.println(packetsDelivered);
    Serial.print("Maximum bandwidth: ");
    Serial.print((packetsDelivered + pair.packetOverhead(0)) * 20);
    Serial.println("B/s");
    Serial.print("Data throughput: ");
    Serial.println(packetsDelivered * 20);
    Serial.println();
    Serial.flush();
    packetsDelivered = 0;
    time = micros();
  }
}
