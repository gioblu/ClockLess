```cpp
/*
Milan, Italy - 24/02/2017
CLDL (ClockLess data link) specification is an invention and intellectual property
of Giovanni Blu Mitolo and Fred Larsen - Copyright 2017 All rights reserved
*/
```
###CLDL (ClockLess Data Link)
CLDL (ClockLess Data Link) has been specified to provide with an efficient, long range, 2 wires, half-duplex data link. It is engineered for devices running time-critical algorithms, long duration tasks or interrupt driven procedures, that are generally disrupting other data link implementations based on synchronization.

###Basic concepts
* One to one data link on 2 communication wires: Port 0, Port 1
* Transmission ports are used to transmit and acknowledge data
* Transmission port is chosen according to the value to be transmitted
* Acknowledgement occurs on the remaining port

####Bit transmission

1. Transmitter starts setting logic state 1 to the port equal to the bit value to be transmitted (PORT 1)
2. Receiver sets logic state 1 to the unused port (PORT 0)
3. Transmitter detects PORT 0 logic state 1 and sets back logic state 0 to PORT 1
4. Both ports are set to logic state 0 and are ready to be used to transmit again

```cpp
Bit 0 transmission sample   
 ________     ______            ________
| PORT 0 |   | TX 0 |          | PORT 0 |
|________|___|______|__________|________|
 ________         _______       ________
| PORT 1 |       |  ACK  |     | PORT 1 |
|________|_______|_______|_____|________|

TRANSMITTER                     RECEIVER

Bit 1 transmission sample
 ________         _______       ________
| PORT 0 |       |  ACK  |     | PORT 0 |
|________|_______|_______|_____|________|
 ________     ______            ________
| PORT 1 |   | TX 1 |          | PORT 1 |
|________|___|______|__________|________|

TRANSMITTER                     RECEIVER

```

####Byte transmission
Bit transmission procedure is repeated to obtain a byte transmission.

```cpp
Byte B10101010 transmission sample

VALUE      1       0       1       0       1       0       1       0
______   _____   _____   _____   _____   _____   _____   _____   _____
      | |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK|
PORT 1| | 1 | | | | 1 | | 1 | | | | 1 | | 1 | | | | 1 | | 1 | | | | 1 |
______| |___|_| |_|___| |___|_| |_|___| |___|_| |_|___| |___|_| |_|___|
      | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | |
PORT 0| | | 1 | | 1 | | | | 1 | | 1 | | | | 1 | | 1 | | | | 1 | | 1 | |
______| |_|___| |___|_| |_|___| |___|_| |_|___| |___|_| |_|___| |___|_|
```
