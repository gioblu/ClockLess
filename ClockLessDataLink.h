
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

#pragma once

#include <Arduino.h>

#ifndef CLDL_COLLISION_DELAY
  #define CLDL_COLLISION_DELAY 16
#endif

#ifndef CLDL_MAX_LENGTH
  #define CLDL_MAX_LENGTH 10
#endif


class ClockLess {
  public:
    uint8_t data[CLDL_MAX_LENGTH];
    uint8_t pin0;
    uint8_t pin1;
    bool    sampling = false;
    bool    transmitting = false;

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


    void receive() {
      if(transmitting) return;
      
      if(!sampling) {
        sampling = true;
        mask = 0;
        byteValue = B00000000;
        digitalWrite(pin0, LOW);
        digitalWrite(pin1, LOW);
        pinMode(pin0, INPUT);
        pinMode(pin1, INPUT);
        if(digitalRead(pin0) && digitalRead(pin1)) return;
      }

      if(!rx && !tx) {
        if(digitalRead(pin0)) byteValue += 0 << mask;
        if(digitalRead(pin1)) byteValue += 1 << mask;
        rx = true;
      }
      if(rx && !tx) {
        digitalWrite((byteValue & (1 << mask)) ? pin0 : pin1, HIGH);
        pinMode((byteValue & (1 << mask)) ? pin0 : pin1, OUTPUT);
        tx = true;
      }
      if(rx && tx) {
        if(!digitalRead((byteValue & (1 << mask)) ? pin1 : pin0)) {
          rx = false;
          tx = false;
          pinMode((byteValue & (1 << mask)) ? pin1 : pin0, INPUT);
          digitalWrite((byteValue & (1 << mask)) ? pin1 : pin0, LOW);
          if((mask + 1) < 8) mask++;
          else {
            saveByteInBuffer(byteValue);
            sampling = false;
          }
        }
      }
    };


    void sendString(uint8_t *string, uint16_t length) {
      source = string;
      length = length;
      index  = 0;
      mask = 0x01;
      transmitting = true;
    };


    void saveByteInBuffer(uint8_t value) {
      for(uint8_t i = 0; i < CLDL_MAX_LENGTH - 1; i++)
        data[i] = data[i + 1];
      data[CLDL_MAX_LENGTH - 1] = value;
    };


    void setPins(uint8_t p0, uint8_t p1) {
      pin0 = p0;
      pin1 = p1;
    };


    void transmit() {
      if(!transmitting || sampling || source == NULL) return;
      if(!tx && !rx) {
        if(canStart()) tx = true;
        else return;
        digitalWrite(((source[index] & mask) ? pin0 : pin1), LOW);
        pinMode(((source[index] & mask) ? pin0 : pin1), INPUT);
        digitalWrite(((source[index] & mask) ? pin1 : pin0), HIGH);
        pinMode(((source[index] & mask) ? pin1 : pin0), OUTPUT);
      }
      if(tx && !rx)
        if(digitalRead((source[index] & mask) ? pin0 : pin1))
          rx = true;
      if(tx && rx) {
        pinMode((source[index] & mask) ? pin1 : pin0, INPUT);
        digitalWrite((source[index] & mask) ? pin1 : pin0, LOW);
        tx = false;
      }
      if(!tx && rx) {
        if(!digitalRead((source[index] & mask) ? pin0 : pin1)) {
          rx = false;
          mask <<= 1;
          if(!mask) {
            if(index + 1 < length) index++;
            else {
              source = NULL;
              length = 0;
              index = 0;
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
    bool    rx = false;
    bool    tx = false;
    uint8_t   mask;
    uint8_t  *source;
    uint8_t   byteValue;
    uint16_t  length = 0;
    uint8_t   index = 0;
};
