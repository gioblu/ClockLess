
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

#include "interfaces/Interfaces.h"

/* Maximum random delay before transmission (16 microseconds) */
#ifndef CLDL_COLLISION_DELAY
  #define CLDL_COLLISION_DELAY 16
#endif

/* Maximum waiting time (3 seconds) before returning CLDL_TIMEOUT */
#ifndef CLDL_MAX_TIMEOUT
  #define CLDL_MAX_TIMEOUT 3000000
#endif

#define CLDL_FAIL           -1
#define CLDL_BOTH_PORTS_UP  -2
#define CLDL_SAMPLING       -3
#define CLDL_TRANSMITTING   -4
#define CLDL_TIMEOUT        -5

class ClockLessDataLink {
  public:
    uint8_t pin0;
    uint8_t pin1;
    bool    sampling     = false;
    bool    transmitting = false;

    boolean begin() {
      setBaseState();
      return true;
    };

    bool canStart() {
      if(tx || rx) return false;
      setBaseState();
      if(CLDL_READ(pin0) || CLDL_READ(pin1)) return false;
      CLDL_DELAY_MICROSECONDS(CLDL_RANDOM(0, CLDL_COLLISION_DELAY));
      if(CLDL_READ(pin0) || CLDL_READ(pin1)) return false;
      return true;
    };

    int16_t receive(uint8_t l = 8) {
      if(transmitting) return CLDL_TRANSMITTING;
      if(!rx && !tx) {
        if(!sampling) {
          setBaseState();
          if(CLDL_READ(pin0) && CLDL_READ(pin1)) return CLDL_BOTH_PORTS_UP;
          bitIndex = 0;
          byteValue = B00000000;
          sampling = true;
        }
        if(CLDL_READ(pin0)) {
          rx = true;
          byteValue += 0 << bitIndex;
        }
        if(CLDL_READ(pin1)) {
          if(rx == true) return CLDL_BOTH_PORTS_UP;
          rx = true;
          byteValue += 1 << bitIndex;
        }
      }
      if(rx && !tx) {
        CLDL_WRITE((byteValue & (1 << bitIndex)) ? pin0 : pin1, HIGH);
        CLDL_MODE((byteValue & (1 << bitIndex)) ? pin0 : pin1, OUTPUT);
        tx = true;
      }
      if(rx && tx) {
        if(!CLDL_READ((byteValue & (1 << bitIndex)) ? pin1 : pin0)) {
          rx = false;
          tx = false;
          CLDL_WRITE((byteValue & (1 << bitIndex)) ? pin0 : pin1, LOW);
          CLDL_MODE((byteValue & (1 << bitIndex)) ? pin0 : pin1, INPUT);
          if((bitIndex + 1) < l) bitIndex++;
          else {
            sampling = false;
            return byteValue;
          }
        }
      }
      if(sampling) return CLDL_SAMPLING;
      else return CLDL_FAIL;
    };

    int16_t sendString(uint8_t *s, uint16_t l) {
      if(transmitting) return CLDL_TRANSMITTING;
      if(sampling) return CLDL_SAMPLING;
      source = s;
      length = l;
      byteIndex  = 0;
      bitIndex = 0x01;
      transmitting = true;
      return CLDL_TRANSMITTING;
    };

    void setBaseState() {
      CLDL_WRITE(pin0, LOW);
      CLDL_WRITE(pin1, LOW);
      CLDL_MODE(pin0, INPUT);
      CLDL_MODE(pin1, INPUT);
    };

    void setPins(uint8_t p0, uint8_t p1) {
      pin0 = p0;
      pin1 = p1;
    };

    void transmit() {
      if(!transmitting || sampling || source == NULL) return;
      if(!tx && !rx) {
        tx = true;
        CLDL_WRITE(((source[byteIndex] & bitIndex) ? pin0 : pin1), LOW);
        CLDL_MODE(((source[byteIndex] & bitIndex) ? pin0 : pin1), INPUT);
        CLDL_WRITE(((source[byteIndex] & bitIndex) ? pin1 : pin0), HIGH);
        CLDL_MODE(((source[byteIndex] & bitIndex) ? pin1 : pin0), OUTPUT);
      }
      if(tx && !rx)
        if(CLDL_READ((source[byteIndex] & bitIndex) ? pin0 : pin1))
          rx = true;
      if(tx && rx) {
        CLDL_WRITE((source[byteIndex] & bitIndex) ? pin1 : pin0, LOW);
        CLDL_MODE((source[byteIndex] & bitIndex) ? pin1 : pin0, INPUT);
        tx = false;
      }
      if(!tx && rx) {
        if(!CLDL_READ((source[byteIndex] & bitIndex) ? pin0 : pin1)) {
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
      transmit();
    };

  private:
    bool      rx = false;
    bool      tx = false;
    uint8_t   bitIndex;
    uint8_t  *source;
    uint8_t   byteValue;
    uint16_t  length = 0;
    uint8_t   byteIndex = 0;
};
