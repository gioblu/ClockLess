
/* ClockLess
   2 wires half-duplex point-to-point software emulated digital data link layer
   Compliant with the CLDL data link layer specification v0.1
   _____________________________________________________________________________

    Copyright 2017 Giovanni Blu Mitolo and Fred Larsen

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. */

class ClockLess {
  public:
    uint8_t pin0;
    uint8_t pin1;

    boolean begin(uint8_t additional_randomness = 0) {
      digitalWrite(pin0, LOW);
      digitalWrite(pin1, LOW);
      pinMode(pin0, INPUT);
      pinMode(pin1, INPUT);
      return true;
    };


    boolean canStart() {
      digitalWrite(pin0, LOW);
      digitalWrite(pin1, LOW);
      pinMode(pin0, INPUT);
      pinMode(pin1, INPUT);
      if(digitalRead(pin0) || digitalRead(pin1)) return false;
      delayMicroseconds(random(0, collisionDelay));
      if(digitalRead(pin0) || digitalRead(pin1)) return false;
      return true;
    };


    bool sendString(uint8_t *string, uint16_t length) {
      if(canStart()) /* TODO */ ;
      else return false;
      return true;
    };

    /* Set a pair of communication pins: */

    void setPins(uint8_t pin0, uint8_t pin1) {
      pin0 = pin0;
      pin1 = pin1;
    };

  private:
    uint8_t byteValue = 0;
    uint8_t bitIndex = 0;
    uint8_t byteIndex = 0;
};
