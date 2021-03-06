```cpp
/*
Milan, Italy - 24/02/2017
CLDL (ClockLess data link) is an asynchronous 4-phase level-signaling protocol.
Specification drafted by Giovanni Blu Mitolo and Fred Larsen
*/
```
###CLDL (ClockLess Data Link)
CLDL (ClockLess Data Link) has been specified to provide with an efficient, long range, 2 wires, half-duplex data link. It is engineered for devices running time-critical algorithms, long duration tasks or interrupt driven procedures, that are generally disrupting other data link implementations based on synchronization and/or time domain.

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
Byte B01010101 transmission sample

 VALUE      0       1       0       1       0       1       0       1
 ______   _____ _ _____ _ _____ _ _____ _ _____ _ _____ _ _____ _ _____   ______
|      | |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |      |
|PORT 0| | 1 |0|0|0| 1 |0| 1 |0|0|0| 1 |0| 1 |0|0|0| 1 |0| 1 |0|0|0| 1 | |PORT 0|
|______| |___|_|_|_|___|_|___|_|_|_|___|_|___|_|_|_|___|_|___|_|_|_|___| |______|
|      | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | | | |ACK| |TX | | |      |
|PORT 1| |0| 1 |0| 1 |0|0|0| 1 |0| 1 |0|0|0| 1 |0| 1 |0|0|0| 1 |0| 1 |0| |PORT 1|
|______| |_|___|_|___|_|_|_|___|_|___|_|_|_|___|_|___|_|_|_|___|_|___|_| |______|
```

####Byte stream transmission
Both devices can attempt to send a byte stream to the other. Before initiating transmission both ports are read to check if there is an ongoing communication. If both ports have logic state 0, a small random time is passed, and both ports are still having logic state 0 transmission can be initiated.  

```cpp
 ______      _____________________________________________________      ______
|      |    | CHANNEL ANALYSIS | TRANSMISSION                     |    |      |
|PORT 0|0000|000000000000000000| data                             |0000|PORT 0|
|______|____|__________________|__________________________________|____|______|
|      |    |                  |                                  |    |      |
|PORT 1|0000|000000000000000000| data                             |0000|PORT 1|
|______|____|__________________|__________________________________|____|______|
```
