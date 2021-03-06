/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef AKHILAFLEX_RADIO_H
#define AKHILAFLEX_RADIO_H

class AKHILAFLEXRadio;
struct FrameBuffer;

#include "mbed.h"
#include "AKHILAFLEXConfig.h"
#include "PacketBuffer.h"
#include "AKHILAFLEXRadioDatagram.h"
#include "AKHILAFLEXRadioEvent.h"

/**
 * Provides a simple broadcast radio abstraction, built upon the raw nrf51822 RADIO module.
 *
 * The nrf51822 RADIO module supports a number of proprietary modes of operation in addition to the typical BLE usage.
 * This class uses one of these modes to enable simple, point to multipoint communication directly between AKHILAFLEXs.
 *
 * TODO: The protocols implemented here do not currently perform any significant form of energy management,
 * which means that they will consume far more energy than their BLE equivalent. Later versions of the protocol
 * should look to address this through energy efficient broadcast techniques / sleep scheduling. In particular, the GLOSSY
 * approach to efficienct rebroadcast and network synchronisation would likely provide an effective future step.
 *
 * TODO: Meshing should also be considered - again a GLOSSY approach may be effective here, and highly complementary to
 * the master/slave arachitecture of BLE.
 *
 * TODO: This implementation only operates whilst the BLE stack is disabled. The nrf51822 provides a timeslot API to allow
 * BLE to cohabit with other protocols. Future work to allow this colocation would be benefical, and would also allow for the
 * creation of wireless BLE bridges.
 *
 * NOTE: This API does not contain any form of encryption, authentication or authorization. It's purpose is solely for use as a
 * teaching aid to demonstrate how simple communications operates, and to provide a sandpit through which learning can take place.
 * For serious applications, BLE should be considered a substantially more secure alternative.
 */

// Status Flags
#define AKHILAFLEX_RADIO_STATUS_INITIALISED       0x0001

// Default configuration values
#define AKHILAFLEX_RADIO_BASE_ADDRESS             0x75626974
#define AKHILAFLEX_RADIO_DEFAULT_GROUP            0
#define AKHILAFLEX_RADIO_DEFAULT_TX_POWER         6
#define AKHILAFLEX_RADIO_MAX_PACKET_SIZE          32
#define AKHILAFLEX_RADIO_HEADER_SIZE              4
#define AKHILAFLEX_RADIO_MAXIMUM_RX_BUFFERS       4

// Known Protocol Numbers
#define AKHILAFLEX_RADIO_PROTOCOL_DATAGRAM        1       // A simple, single frame datagram. a little like UDP but with smaller packets. :-)
#define AKHILAFLEX_RADIO_PROTOCOL_EVENTBUS        2       // Transparent propogation of events from one AKHILAFLEX to another.

// Events
#define AKHILAFLEX_RADIO_EVT_DATAGRAM             1       // Event to signal that a new datagram has been received.


struct FrameBuffer
{
    uint8_t         length;                             // The length of the remaining bytes in the packet. includes protocol/version/group fields, excluding the length field itself.
    uint8_t         version;                            // Protocol version code.
    uint8_t         group;                              // ID of the group to which this packet belongs.
    uint8_t         protocol;                           // Inner protocol number c.f. those issued by IANA for IP protocols

    uint8_t         payload[AKHILAFLEX_RADIO_MAX_PACKET_SIZE];    // User / higher layer protocol data
    FrameBuffer     *next;                              // Linkage, to allow this and other protocols to queue packets pending processing.
    int             rssi;                               // Received signal strength of this frame.
};


class AKHILAFLEXRadio : AKHILAFLEXComponent
{
    uint8_t                 group;      // The radio group to which this AKHILAFLEX belongs.
    uint8_t                 queueDepth; // The number of packets in the receiver queue.
    int                     rssi;
    FrameBuffer             *rxQueue;   // A linear list of incoming packets, queued awaiting processing.
    FrameBuffer             *rxBuf;     // A pointer to the buffer being actively used by the RADIO hardware.

    public:
    AKHILAFLEXRadioDatagram   datagram;   // A simple datagram service.
    AKHILAFLEXRadioEvent      event;      // A simple event handling service.
    static AKHILAFLEXRadio    *instance;  // A singleton reference, used purely by the interrupt service routine.

    /**
      * Constructor.
      *
      * Initialise the AKHILAFLEXRadio.
      *
      * @note This class is demand activated, as a result most resources are only
      *       committed if send/recv or event registrations calls are made.
      */
    AKHILAFLEXRadio(uint16_t id = AKHILAFLEX_ID_RADIO);

    /**
      * Change the output power level of the transmitter to the given value.
      *
      * @param power a value in the range 0..7, where 0 is the lowest power and 7 is the highest.
      *
      * @return AKHILAFLEX_OK on success, or AKHILAFLEX_INVALID_PARAMETER if the value is out of range.
      */
    int setTransmitPower(int power);

    /**
      * Change the transmission and reception band of the radio to the given channel
      *
      * @param band a frequency band in the range 0 - 100. Each step is 1MHz wide, based at 2400MHz.
      *
      * @return AKHILAFLEX_OK on success, or AKHILAFLEX_INVALID_PARAMETER if the value is out of range,
      *         or AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int setFrequencyBand(int band);

    /**
      * Retrieve a pointer to the currently allocated receive buffer. This is the area of memory
      * actively being used by the radio hardware to store incoming data.
      *
      * @return a pointer to the current receive buffer.
      */
    FrameBuffer * getRxBuf();

    /**
      * Attempt to queue a buffer received by the radio hardware, if sufficient space is available.
      *
      * @return AKHILAFLEX_OK on success, or AKHILAFLEX_NO_RESOURCES if a replacement receiver buffer
      *         could not be allocated (either by policy or memory exhaustion).
      */
    int queueRxBuf();

    /**
      * Sets the RSSI for the most recent packet.
      * The value is measured in -dbm. The higher the value, the stronger the signal.
      * Typical values are in the range -42 to -128.
      *
      * @param rssi the new rssi value.
      *
      * @note should only be called from RADIO_IRQHandler...
      */
    int setRSSI(int rssi);

    /**
      * Retrieves the current RSSI for the most recent packet.
      * The return value is measured in -dbm. The higher the value, the stronger the signal.
      * Typical values are in the range -42 to -128.
      *
      * @return the most recent RSSI value or AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int getRSSI();

    /**
      * Initialises the radio for use as a multipoint sender/receiver
      *
      * @return AKHILAFLEX_OK on success, AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int enable();

    /**
      * Disables the radio for use as a multipoint sender/receiver.
      *
      * @return AKHILAFLEX_OK on success, AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int disable();

    /**
      * Sets the radio to listen to packets sent with the given group id.
      *
      * @param group The group to join. A AKHILAFLEX can only listen to one group ID at any time.
      *
      * @return AKHILAFLEX_OK on success, or AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int setGroup(uint8_t group);

    /**
      * A background, low priority callback that is triggered whenever the processor is idle.
      * Here, we empty our queue of received packets, and pass them onto higher level protocol handlers.
      */
    virtual void idleTick();

    /**
      * Determines the number of packets ready to be processed.
      *
      * @return The number of packets in the receive buffer.
      */
    int dataReady();

    /**
      * Retrieves the next packet from the receive buffer.
      * If a data packet is available, then it will be returned immediately to
      * the caller. This call will also dequeue the buffer.
      *
      * @return The buffer containing the the packet. If no data is available, NULL is returned.
      *
      * @note Once recv() has been called, it is the callers responsibility to
      *       delete the buffer when appropriate.
      */
    FrameBuffer* recv();

    /**
      * Transmits the given buffer onto the broadcast radio.
      * The call will wait until the transmission of the packet has completed before returning.
      *
      * @param data The packet contents to transmit.
      *
      * @return AKHILAFLEX_OK on success, or AKHILAFLEX_NOT_SUPPORTED if the BLE stack is running.
      */
    int send(FrameBuffer *buffer);
};

#endif
