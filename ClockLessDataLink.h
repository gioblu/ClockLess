
/* ClockLessDataLink
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

#pragma once
#include <Arduino.h>

#ifndef CLDL_COLLISION_DELAY
  #define CLDL_COLLISION_DELAY 16
#endif

#define CLDL_FAIL           -1
#define CLDL_BUSY           -2
#define CLDL_BOTH_PORTS_UP  -3
#define CLDL_SAMPLING       -4
#define CLDL_TRANSMITTING   -5

class ClockLessDataLink {
  public:
    uint8_t pin0;
    uint8_t pin1;
    bool    sampling     = false;
    bool    transmitting = false;
    uint32_t  timeIn     =     1; // 150 microseconds minimum bit duration

    boolean begin() {
      digitalWrite(pin0, LOW);
      digitalWrite(pin1, LOW);
      pinMode(pin0, INPUT);
      pinMode(pin1, INPUT);
      return true;
    };

    bool canStart() {
      if(tx || rx) return false;
      digitalWrite(pin0, LOW);
      digitalWrite(pin1, LOW);
      pinMode(pin0, INPUT);
      pinMode(pin1, INPUT);
      if(digitalRead(pin0) || digitalRead(pin1)) return false;
      delayMicroseconds(random(0, CLDL_COLLISION_DELAY));
      if(digitalRead(pin0) || digitalRead(pin1)) return false;
      return true;
    };

    int16_t receive() {
      if(transmitting) return CLDL_BUSY;
      if(!rx && !tx) {
        if(!sampling) {
          digitalWrite(pin0, LOW);
          digitalWrite(pin1, LOW);
          pinMode(pin0, INPUT);
          pinMode(pin1, INPUT);
          if(digitalRead(pin0) && digitalRead(pin1)) return CLDL_BOTH_PORTS_UP;
          bitIndex = 0;
          byteValue = B00000000;
          bitValue = 0;
        }
        if(digitalRead(pin0)) {
          if(timeIn < (uint32_t)(micros() - t)) {
            sampling = true;
            rx = true;
            bitValue = 0;
            byteValue += bitValue << bitIndex;
          }
        }
        if(digitalRead(pin1)) {
          if(rx == true) return CLDL_BUSY;
          if(timeIn < (uint32_t)(micros() - t)) {
            sampling = true;
            rx = true;
            bitValue = 1;
            byteValue += bitValue << bitIndex;
          }
        }
      }
      if(rx && !tx) {
        digitalWrite((byteValue & (1 << bitIndex)) ? pin0 : pin1, HIGH);
        pinMode((byteValue & (1 << bitIndex)) ? pin0 : pin1, OUTPUT);
        tx = true;
        t = micros();
      }
      if(rx && tx) {
        if(
          (timeIn < (uint32_t)(micros() - t)) &&
          !digitalRead((byteValue & (1 << bitIndex)) ? pin1 : pin0)
        ) {
          t = micros();
          rx = false;
          tx = false;
          digitalWrite((byteValue & (1 << bitIndex)) ? pin0 : pin1, LOW);
          pinMode((byteValue & (1 << bitIndex)) ? pin0 : pin1, INPUT);
          if((bitIndex + 1) < 8) bitIndex++;
          else {
            sampling = false;
            return byteValue;
          }
        }
      }
      if(sampling) return CLDL_SAMPLING;
      else return CLDL_FAIL;
    };

    bool sendString(uint8_t *s, uint16_t l) {
      if(!transmitting && !sampling) {
        source = s;
        length = l;
        byteIndex  = 0;
        bitIndex = 0x01;
        transmitting = true;
        return true;
      }
      return false;
    };

    void setPins(uint8_t p0, uint8_t p1) {
      pin0 = p0;
      pin1 = p1;
    };

    void transmit() {
      if(!transmitting || sampling || source == NULL) return;
      if(!tx && !rx) {
        if(!canStart()) return;
        tx = true;
        digitalWrite(((source[byteIndex] & bitIndex) ? pin0 : pin1), LOW);
        pinMode(((source[byteIndex] & bitIndex) ? pin0 : pin1), INPUT);
        digitalWrite(((source[byteIndex] & bitIndex) ? pin1 : pin0), HIGH);
        pinMode(((source[byteIndex] & bitIndex) ? pin1 : pin0), OUTPUT);
        t = micros();
      }
      if(tx && !rx && (timeIn < (uint32_t)(micros() - t)))
        if(digitalRead((source[byteIndex] & bitIndex) ? pin0 : pin1)) {
          rx = true;
          t = micros();
        }
      if(tx && rx && (timeIn < (uint32_t)(micros() - t))) {
        digitalWrite((source[byteIndex] & bitIndex) ? pin1 : pin0, LOW);
        pinMode((source[byteIndex] & bitIndex) ? pin1 : pin0, INPUT);
        tx = false;
        t = micros();
      }
      if(!tx && rx && (timeIn < (uint32_t)(micros() - t))) {
        if(!digitalRead((source[byteIndex] & bitIndex) ? pin0 : pin1)) {
          rx = false;
          bitIndex <<= 1;
          if(!bitIndex) {
            if(byteIndex + 1 < length) {
              byteIndex++;
              bitIndex = 0x01;
            } else {
              source = NULL;
              length = 0;
              byteIndex = 0;
              transmitting = false;
            }
          }
        }
      }
    };

    void update() {
      receive();
      transmit();
    };

  private:
    bool      rx = false;
    bool      tx = false;
    uint8_t   bitIndex;
    bool      bitValue;
    uint8_t  *source;
    uint8_t   byteValue;
    uint16_t  length = 0;
    uint8_t   byteIndex = 0;
    uint32_t  t;
};
