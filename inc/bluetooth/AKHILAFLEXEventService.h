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

#ifndef AKHILAFLEX_EVENT_SERVICE_H
#define AKHILAFLEX_EVENT_SERVICE_H
/*#ifndef AKHILAFLEX_EVENT_SERVICE_H
#define AKHILAFLEX_EVENT_SERVICE_H */


#include "AKHILAFLEXConfig.h"
#include "ble/BLE.h"
#include "AKHILAFLEXEvent.h"
#include "EventModel.h"

// UUIDs for our service and characteristics
extern const uint8_t  AKHILAFLEXEventServiceUUID[];
extern const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceClientEventCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID[];
/*extern const uint8_t  AKHILAFLEXEventServiceUUID[];
extern const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceClientEventCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID[];
extern const uint8_t  AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID[]; */


struct EventServiceEvent
{
    uint16_t    type;
    uint16_t    reason;
};


/**
  * Class definition for a AKHILAFLEX BLE Event Service.
  * Provides a BLE gateway onto an Event Model.
  */
class AKHILAFLEXEventService : public AKHILAFLEXComponent
/*class AKHILAFLEXEventService : public AKHILAFLEXComponent */
{
    public:

    /**
      * Constructor.
      * Create a representation of the EventService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _messageBus An instance of an EventModel which events will be mirrored from.
      */
    AKHILAFLEXEventService(BLEDevice &_ble, EventModel &_messageBus);
    /*AKHILAFLEXEventService(BLEDevice &_ble, EventModel &_messageBus);*/


    /**
     * Periodic callback from AKHILAFLEX scheduler.
     * If we're no longer connected, remove any registered Message Bus listeners.
     */
    virtual void idleTick();

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const GattWriteCallbackParams *params);

    /**
      * Callback. Invoked when any events are sent on the AKHILAFLEX message bus.
      */
    void onAKHILAFLEXEvent(AKHILAFLEXEvent evt);
   /* void onAKHILAFLEXEvent(AKHILAFLEXEvent evt); */

    /**
      * Read callback on AKHILAFLEXRequirements characteristic.
      *
      * Used to iterate through the events that the code on this micro:bit is interested in.
      */
    void onRequirementsRead(GattReadAuthCallbackParams *params);

    private:

    // Bluetooth stack we're running on.
    BLEDevice           &ble;
	EventModel	        &messageBus;

    // memory for our event characteristics.
    EventServiceEvent   clientEventBuffer;
    EventServiceEvent   AKHILAFLEXEventBuffer;
    EventServiceEvent   AKHILAFLEXRequirementsBuffer;
    EventServiceEvent   clientRequirementsBuffer;
    /*EventServiceEvent   clientEventBuffer;
    EventServiceEvent   AKHILAFLEXEventBuffer;
    EventServiceEvent   AKHILAFLEXRequirementsBuffer;
    EventServiceEvent   clientRequirementsBuffer; */

    // handles on this service's characterisitics.
    GattAttribute::Handle_t AKHILAFLEXEventCharacteristicHandle;
    GattAttribute::Handle_t clientRequirementsCharacteristicHandle;
    GattAttribute::Handle_t clientEventCharacteristicHandle;
    GattCharacteristic *AKHILAFLEXRequirementsCharacteristic;
   /*     GattAttribute::Handle_t AKHILAFLEXEventCharacteristicHandle;
    GattAttribute::Handle_t clientRequirementsCharacteristicHandle;
    GattAttribute::Handle_t clientEventCharacteristicHandle;
    GattCharacteristic *AKHILAFLEXRequirementsCharacteristic; */


    // Message bus offset last sent to the client...
    uint16_t messageBusListenerOffset;

};


#endif
