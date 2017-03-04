#include <ClockLessDataLink.h>

/* ClockLess
   2 wires half-duplex point-to-point software emulated communication protocol
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
#include <ClockLessDataLink.h>
#include "utils/ClockLessCrc8.h"
#include "utils/ClockLessCrc32.h"

#ifndef CLOCKLESS_MAX_LENGTH
  #define CLOCKLESS_MAX_LENGTH 100
#endif

#ifndef CLOCKLESS_INITIALIZER
  #define CLOCKLESS_INITIALIZER (uint32_t)(0B10110111011110011001111011101101)
#endif

#define CLOCKLESS_FAIL         CLDL_FAIL
#define CLOCKLESS_ACK          -6
#define CLOCKLESS_BUSY         -7
#define CLOCKLESS_PROCESSING   -8
#define CLOCKLESS_BUFFER_FULL  -9
#define CLOCKLESS_NAK          -21

/* 0: 1 byte length (255 max bytes), 1: 2 bytes length (max 65535 bytes) */
#define CLOCKLESS_EXT_LEN_BIT  B01000000
/* CRC bit  0: CRC8, 1: CRC32 */
#define CLOCKLESS_CRC_BIT      B00100000

typedef void (* ClockLessReceiver)(uint8_t *payload, uint16_t length);
static  void ClockLessReceiverDummy(uint8_t *payload, uint16_t length) {};

class ClockLess {
  public:
    ClockLessDataLink dataLink; // ClockLessDataLink Data-link layer instance

    /* Compose packet in ClockLess format
    INPUT: Header config bit mask, source pointer to data, length (1-65535 byte)
    OUTPUT: Positive number new computed length, or CLOCKLESS_BUFFER_FULL if the
    content is longer than the space available in buffer (CLOCKLESS_MAX_LENGTH) */

    int16_t composePacket(uint8_t *source, uint16_t length, uint8_t header = 0) {
      uint16_t newLength = length + packetOverhead(header);
      if(newLength > CLOCKLESS_MAX_LENGTH)
        return CLOCKLESS_BUFFER_FULL;
      if(newLength > 255) header |= CLOCKLESS_EXT_LEN_BIT | CLOCKLESS_CRC_BIT;
      memcpy(data, &initializer, 4);
      data[4] = header;
      if(newLength < 255) data[5] = newLength;
      else memcpy(data + 5, &newLength, 2);
      memcpy(
        data + (packetOverhead(header) - crcOverhead(header)),
        source,
        length
      );
      if(header & CLOCKLESS_CRC_BIT) {
        uint32_t computedCrc =
          ClockLessCrc32::compute(data + 4, newLength - 8);
        data[newLength - 4] = (uint32_t)(computedCrc) >> 24;
        data[newLength - 3] = (uint32_t)(computedCrc) >> 16;
        data[newLength - 2] = (uint32_t)(computedCrc) >>  8;
        data[newLength - 1] = (uint32_t)(computedCrc);
      } else data[newLength - 1] =
        ClockLessCrc8::compute(data + 4, newLength - 5);
      return newLength;
    };

    /* Check if a ClockLess initializer is received
    INPUT: None, OUTPUT: true if detected, false if not detected */

    bool getInitializer() {
      int16_t result = dataLink.receive(1);
      if(result >= 0) {
        buffer >>= 1;
        buffer |= ((uint32_t)result << 31);
      }
      if(buffer == initializer)
        return true;
      return false;
    };

    /* Calculate packet overhead
    INPUT: Header bitmask, OUTPUT: Computed overhead */

    uint8_t packetOverhead(uint8_t header = 0) {
      return (
        ((header & CLOCKLESS_EXT_LEN_BIT) ? 2 : 1) +
        crcOverhead(header) + 4 // initializer
        + 1 // header
      );
    };

    uint8_t crcOverhead(uint8_t header = 0) {
      return (header & CLOCKLESS_CRC_BIT) ? 4 : 1;
    };

    /* Try to receive data
    INPUT: None, OUTPUT: If positive number, a byte is returned, if negative
    number, an error from data-link layer bubbled up. */

    int16_t receive() {
      if(dataLink.transmitting) return CLOCKLESS_BUSY;
      int16_t result = CLOCKLESS_FAIL;
      if(buffer == initializer) result = dataLink.receive();
      else if(getInitializer()) {
        index = 0;
        result = dataLink.receive();
      }
      if(result >= 0) {
        data[index] = result;
        result = parse();
        if(result == CLOCKLESS_NAK || result == CLOCKLESS_BUFFER_FULL) {
          index = 0;
          buffer = 0;
          return result;
        }
        if(result > packetOverhead(0)  && result < CLOCKLESS_MAX_LENGTH) {
          receiver(
            data + (packetOverhead(data[0]) - crcOverhead(data[0]) - 4),
            result - crcOverhead(data[0])
          );
          index = 0;
          buffer = 0;
          return result;
        }
        if(index + 1 < CLOCKLESS_MAX_LENGTH) index += 1;
        else {
          index = 0;
          buffer = 0;
          return CLOCKLESS_BUFFER_FULL;
        }
      }
      return result;
    };

    /* Try to parse received data
    INPUT: None, OUTPUT: If positive number parsed packet length is returned.
    CLOCKLESS_BUFFER_FULL is returned if packet length is longer than buffer. */

    int16_t parse() {
      uint16_t length = CLOCKLESS_MAX_LENGTH - 1;
      if(index >= 2) {
        if(data[0] & CLOCKLESS_EXT_LEN_BIT) {
          length = data[1] << 8 | data[2] & 0xFF;
        } else length = data[1];
        if(length > CLOCKLESS_MAX_LENGTH)
          return CLOCKLESS_BUFFER_FULL;
      }
      if(index == (length - 5)) {
        bool computedCrc;
        if(data[0] & CLOCKLESS_CRC_BIT)
          computedCrc = ClockLessCrc32::compare(
            ClockLessCrc32::compute(data, length - 4), data + (length - 4)
          );
        else computedCrc = !ClockLessCrc8::compute(data, length - 4);
        if(!computedCrc) return CLOCKLESS_NAK;
        return length;
      }
      return CLOCKLESS_PROCESSING;
    };

    /* Try to send a packet
    INPUT: Header bit-mask, pointer to data source and its length
    OUTPUT: Bubbles up result from data-link layer
    CLOCKLESS_BUFFER_FULL is returned if length computed is longer than buffer */

    int16_t sendPacket(uint8_t *source, uint16_t length, uint8_t header = 0) {
      if(dataLink.sampling || dataLink.transmitting || !dataLink.canStart())
        return CLOCKLESS_BUSY;
      if(!(length = composePacket(source, length, header)))
        return CLOCKLESS_BUFFER_FULL;
      return dataLink.sendString(data, length);
    };

    /* Set unique initializer: */

    void setInitializer(uint32_t i) {
      initializer = i;
    };

    /* Set receiver function: */

    void setReceiver(ClockLessReceiver r) {
      receiver = r;
    };

    /* Update data-link */

    void update() { dataLink.update(); };

  private:
    uint32_t  buffer = 0;
    uint8_t   data[CLOCKLESS_MAX_LENGTH];
    uint32_t  initializer = CLOCKLESS_INITIALIZER;
    uint16_t  index = 0;
    ClockLessReceiver receiver = ClockLessReceiverDummy;
};
