
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


#ifndef CLDL_FAIL
  #define CLDL_FAIL           -1
#endif

#ifndef CLDL_BUSY
  #define CLDL_BUSY           -2
#endif

#ifndef CLDL_BOTH_PORTS_UP
  #define CLDL_BOTH_PORTS_UP  -3
#endif

#ifndef CLDL_SAMPLING
  #define CLDL_SAMPLING       -4
#endif


class ClockLessDataLink {
  public:
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


    int16_t receive() {
      if(transmitting) return CLDL_BUSY;

      if(!rx && !tx) {
        /* If ready to sample a byte: */
        if(!sampling) {
          digitalWrite(pin0, LOW);
          digitalWrite(pin1, LOW);
          pinMode(pin0, INPUT);
          pinMode(pin1, INPUT);
          if(digitalRead(pin0) && digitalRead(pin1)) return CLDL_BOTH_PORTS_UP;
          bitIndex = 0;
          byteValue = B00000000;
        }

        /* Sample data bit: */
        if(digitalRead(pin0)) {
          sampling = true;
          rx = true;
          byteValue += 0 << bitIndex;
        }
        if(digitalRead(pin1)) {
          if(rx == true) return CLDL_BUSY;
          sampling = true;
          rx = true;
          byteValue += 1 << bitIndex;
        }
        #ifdef CLDL_DEBUG
          if(sampling) {
            Serial.print("RX | 1 - Received and acking: ");
            Serial.println(byteValue, BIN);
          }
        #endif
      }

      if(rx && !tx) {
        digitalWrite((byteValue & (1 << bitIndex)) ? pin0 : pin1, HIGH);
        pinMode((byteValue & (1 << bitIndex)) ? pin0 : pin1, OUTPUT);
        tx = true;
        #ifdef CLDL_DEBUG
          Serial.println("RX | 2 - transmit ACK");
        #endif
      }
      if(rx && tx) {
        if(!digitalRead((byteValue & (1 << bitIndex)) ? pin1 : pin0)) {
          #ifdef CLDL_DEBUG
            Serial.println("RX | 3 - transmitter set data port low");
          #endif
          rx = false;
          tx = false;
          pinMode((byteValue & (1 << bitIndex)) ? pin0 : pin1, INPUT);
          digitalWrite((byteValue & (1 << bitIndex)) ? pin0 : pin1, LOW);
          if((bitIndex + 1) < 8) {
            bitIndex++;
            #ifdef CLDL_DEBUG
              Serial.println("RX | Next bit");
            #endif
          }
          else {
            #ifdef CLDL_DEBUG
              Serial.print("RX | End byte: ");
              Serial.println(byteValue);
            #endif
            sampling = false;
            return byteValue;
          }
        }
      }
      if(sampling) return CLDL_SAMPLING;
      else return CLDL_FAIL;
    };


    void sendString(uint8_t *s, uint16_t l) {
      source = s;
      length = l;
      byteIndex  = 0;
      bitIndex = 0x01;
      transmitting = true;
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
        #ifdef CLDL_DEBUG
          Serial.println("TX | Init bit tx");
        #endif
        digitalWrite(((source[byteIndex] & bitIndex) ? pin0 : pin1), LOW);
        pinMode(((source[byteIndex] & bitIndex) ? pin0 : pin1), INPUT);
        digitalWrite(((source[byteIndex] & bitIndex) ? pin1 : pin0), HIGH);
        pinMode(((source[byteIndex] & bitIndex) ? pin1 : pin0), OUTPUT);
      }
      if(tx && !rx)
        if(digitalRead((source[byteIndex] & bitIndex) ? pin0 : pin1)) {
          rx = true;
          #ifdef CLDL_DEBUG
            Serial.println("TX | 2 Got ACK");
          #endif
        }
      if(tx && rx) {
        pinMode((source[byteIndex] & bitIndex) ? pin1 : pin0, INPUT);
        digitalWrite((source[byteIndex] & bitIndex) ? pin1 : pin0, LOW);
        #ifdef CLDL_DEBUG
          Serial.println("TX | 3 Data port down");
        #endif
        tx = false;
      }
      if(!tx && rx) {
        if(!digitalRead((source[byteIndex] & bitIndex) ? pin0 : pin1)) {
          #ifdef CLDL_DEBUG
            Serial.print("TX | 4 bit: ");
            Serial.println(bitIndex);
          #endif
          rx = false;
          bitIndex <<= 1;
          if(!bitIndex) {
            #ifdef CLDL_DEBUG
              Serial.print("Byte index: ");
              Serial.print(byteIndex);
              Serial.print(" | String length: ");
              Serial.println(length);
            #endif
            if(byteIndex + 1 < length) {
              #ifdef CLDL_DEBUG
                Serial.println("TX | 5 - Next byte");
              #endif
              byteIndex++;
              bitIndex = 0x01;
            } else {
              source = NULL;
              length = 0;
              byteIndex = 0;
              transmitting = false;
              #ifdef CLDL_DEBUG
                Serial.println("TX END");
              #endif
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
    uint8_t   bitIndex;
    uint8_t  *source;
    uint8_t   byteValue;
    uint16_t  length = 0;
    uint8_t   byteIndex = 0;
};
