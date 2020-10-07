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

/**
  * Class definition for a AKHILAFLEX BLE Event Service.
  * Provides a BLE gateway onto an Event Model.
  */

#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXEventService.h"
#include "ble/UUID.h"
#include "ExternalEvents.h"
#include "AKHILAFLEXFiber.h"

/**
  * Constructor.
  * Create a representation of the EventService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _messageBus An instance of an EventModel which events will be mirrored from.
  */
/*AKHILAFLEXEventService::AKHILAFLEXEventService(BLEDevice &_ble, EventModel &_messageBus) :
        ble(_ble),messageBus(_messageBus) */
AKHILAFLEXEventService::AKHILAFLEXEventService(BLEDevice &_ble, EventModel &_messageBus) :
        ble(_ble),messageBus(_messageBus) 

{
/*GattCharacteristic  AKHILAFLEXEventCharacteristic(AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID, (uint8_t *)&AKHILAFLEXEventBuffer, 0, sizeof(EventServiceEvent),
GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);*/
GattCharacteristic  AKHILAFLEXEventCharacteristic(AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID, (uint8_t *)&AKHILAFLEXEventBuffer, 0, sizeof(EventServiceEvent),
GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);  

/* GattCharacteristic  clientEventCharacteristic(AKHILAFLEXEventServiceClientEventCharacteristicUUID, (uint8_t *)&clientEventBuffer, 0, sizeof(EventServiceEvent),
    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);*/

GattCharacteristic  clientEventCharacteristic(AKHILAFLEXEventServiceClientEventCharacteristicUUID, (uint8_t *)&clientEventBuffer, 0, sizeof(EventServiceEvent),
    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE); 

  /*  GattCharacteristic  clientRequirementsCharacteristic(AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID, (uint8_t *)&clientRequirementsBuffer, 0, sizeof(EventServiceEvent), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    AKHILAFLEXRequirementsCharacteristic = new GattCharacteristic(AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID, (uint8_t *)&AKHILAFLEXRequirementsBuffer, 0, sizeof(EventServiceEvent), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    AKHILAFLEXRequirementsCharacteristic->setReadAuthorizationCallback(this, &AKHILAFLEXEventService::onRequirementsRead);*/
    GattCharacteristic  clientRequirementsCharacteristic(AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID, (uint8_t *)&clientRequirementsBuffer, 0, sizeof(EventServiceEvent), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    AKHILAFLEXRequirementsCharacteristic = new GattCharacteristic(AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID, (uint8_t *)&AKHILAFLEXRequirementsBuffer, 0, sizeof(EventServiceEvent), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    AKHILAFLEXRequirementsCharacteristic->setReadAuthorizationCallback(this, &AKHILAFLEXEventService::onRequirementsRead);


    clientEventBuffer.type = 0x00;
    clientEventBuffer.reason = 0x00;

 /*AKHILAFLEXEventBuffer = AKHILAFLEXRequirementsBuffer = clientRequirementsBuffer = clientEventBuffer; */
    AKHILAFLEXEventBuffer = AKHILAFLEXRequirementsBuffer = clientRequirementsBuffer = clientEventBuffer; 

    messageBusListenerOffset = 0;

    // Set default security requirements
  /*AKHILAFLEXEventCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    clientEventCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    clientRequirementsCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    AKHILAFLEXRequirementsCharacteristic->requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);*/
     AKHILAFLEXEventCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL); 
    clientEventCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    clientRequirementsCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    AKHILAFLEXRequirementsCharacteristic->requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL); 


   /*attCharacteristic *characteristics[] = {&AKHILAFLEXEventCharacteristic, &clientEventCharacteristic, &clientRequirementsCharacteristic, AKHILAFLEXRequirementsCharacteristic};
    GattService         service(AKHILAFLEXEventServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *)); */
GattCharacteristic *characteristics[] = {&AKHILAFLEXEventCharacteristic, &clientEventCharacteristic, &clientRequirementsCharacteristic, AKHILAFLEXRequirementsCharacteristic};
    GattService         service(AKHILAFLEXEventServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *)); 

    ble.addService(service);

  /*AKHILAFLEXEventCharacteristicHandle = AKHILAFLEXEventCharacteristic.getValueHandle();*/
   AKHILAFLEXEventCharacteristicHandle = AKHILAFLEXEventCharacteristic.getValueHandle();
    clientEventCharacteristicHandle = clientEventCharacteristic.getValueHandle();
    clientRequirementsCharacteristicHandle = clientRequirementsCharacteristic.getValueHandle();

  /*ble.onDataWritten(this, &AKHILAFLEXEventService::onDataWritten); */
    ble.onDataWritten(this, &AKHILAFLEXEventService::onDataWritten); 

    fiber_add_idle_component(this);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
/*
void AKHILAFLEXEventService::onDataWritten(const GattWriteCallbackParams *params) */
void AKHILAFLEXEventService::onDataWritten(const GattWriteCallbackParams *params) 
{
    int len = params->len;
    EventServiceEvent *e = (EventServiceEvent *)params->data;

    if (params->handle == clientEventCharacteristicHandle) {

        // Read and fire all events...
        while (len >= 4)
        {
            AKHILAFLEXEvent evt(e->type, e->reason);
            /* AKHILAFLEXEvent evt(e->type, e->reason); */
            len-=4;
            e++;
        }
        return;
    }

    if (params->handle == clientRequirementsCharacteristicHandle) {
        // Read and register for all the events given...
        while (len >= 4)
        {
        /* messageBus.listen(e->type, e->reason, this, &AKHILAFLEXEventService::onAKHILAFLEXEvent, MESSAGE_BUS_LISTENER_IMMEDIATE); */
          messageBus.listen(e->type, e->reason, this, &AKHILAFLEXEventService::onAKHILAFLEXEvent, MESSAGE_BUS_LISTENER_IMMEDIATE); 

            len-=4;
            e++;
        }
        return;
    }
}

/**
  * Callback. Invoked when any events are sent on the AKHILAFLEX message bus.
  */
/*void AKHILAFLEXEventService::onAKHILAFLEXEvent(AKHILAFLEXEvent evt)*/
void AKHILAFLEXEventService::onAKHILAFLEXEvent(AKHILAFLEXEvent evt) 
{
  /*EventServiceEvent *e = &AKHILAFLEXEventBuffer; */
    EventServiceEvent *e = &AKHILAFLEXEventBuffer; 

    if (ble.getGapState().connected) {
        e->type = evt.source;
        e->reason = evt.value;

      /*  ble.gattServer().notify(AKHILAFLEXEventCharacteristicHandle, (const uint8_t *)e, sizeof(EventServiceEvent)); */
        ble.gattServer().notify(AKHILAFLEXEventCharacteristicHandle, (const uint8_t *)e, sizeof(EventServiceEvent)); 
    }
}

/**
  * Periodic callback from AKHILAFLEX scheduler.
  * If we're no longer connected, remove any registered Message Bus listeners.
  */
/*void AKHILAFLEXEventService::idleTick() */
void AKHILAFLEXEventService::idleTick() 
{
    if (!ble.getGapState().connected && messageBusListenerOffset >0) {
        messageBusListenerOffset = 0;
       /* messageBus.ignore(AKHILAFLEX_ID_ANY, AKHILAFLEX_EVT_ANY, this, &AKHILAFLEXEventService::onAKHILAFLEXEvent); */
       messageBus.ignore(AKHILAFLEX_ID_ANY, AKHILAFLEX_EVT_ANY, this, &AKHILAFLEXEventService::onAKHILAFLEXEvent); 
    }
}

/**
  * Read callback on AKHILAFLEXRequirements characteristic.
  *
  * Used to iterate through the events that the code on this AKHILAFLEX is interested in.
  */
/*void AKHILAFLEXEventService::onRequirementsRead(GattReadAuthCallbackParams *params) */
void AKHILAFLEXEventService::onRequirementsRead(GattReadAuthCallbackParams *params) 
{
    /*if (params->handle == AKHILAFLEXRequirementsCharacteristic->getValueHandle()) */
     if (params->handle == AKHILAFLEXRequirementsCharacteristic->getValueHandle()) 

    {
        // Walk through the lsit of message bus listeners.
        // We send one at a time, and our client can keep reading from this characterisitic until we return an emtpy value.
       /* AKHILAFLEXListener *l = messageBus.elementAt(messageBusListenerOffset++); */
     AKHILAFLEXListener *l = messageBus.elementAt(messageBusListenerOffset++); 

        if (l != NULL)
        {
           /* AKHILAFLEXRequirementsBuffer.type = l->id;
            AKHILAFLEXRequirementsBuffer.reason = l->value;*/
             AKHILAFLEXRequirementsBuffer.type = l->id;
            AKHILAFLEXRequirementsBuffer.reason = l->value; 
   
            /*ble.gattServer().write(AKHILAFLEXRequirementsCharacteristic->getValueHandle(), (uint8_t *)&AKHILAFLEXRequirementsBuffer, sizeof(EventServiceEvent));
        }*/
          ble.gattServer().write(AKHILAFLEXRequirementsCharacteristic->getValueHandle(), (uint8_t *)&AKHILAFLEXRequirementsBuffer, sizeof(EventServiceEvent));
        }  
        else {
           /* ble.gattServer().write(AKHILAFLEXRequirementsCharacteristic->getValueHandle(), (uint8_t *)&AKHILAFLEXRequirementsBuffer, 0); */
        ble.gattServer().write(AKHILAFLEXRequirementsCharacteristic->getValueHandle(), (uint8_t *)&AKHILAFLEXRequirementsBuffer, 0); 

        }

    }
}

/*const uint8_t  AKHILAFLEXEventServiceUUID[] = {
    0x00,0x00,0x93,0xaf,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXEventServiceUUID[] = {
    0x00,0x00,0x93,0xaf,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 

/*const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID[] = {
    0x00,0x00,0x97,0x75,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXEventCharacteristicUUID[] = {
    0x00,0x00,0x97,0x75,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 

/*
const uint8_t  AKHILAFLEXEventServiceClientEventCharacteristicUUID[] = {
    0x00,0x00,0x54,0x04,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXEventServiceClientEventCharacteristicUUID[] = {
    0x00,0x00,0x54,0x04,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 


/*const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID[] = {
    0x00,0x00,0xb8,0x4c,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXEventServiceAKHILAFLEXRequirementsCharacteristicUUID[] = {
    0x00,0x00,0xb8,0x4c,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 


/*const uint8_t  AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID[] = {
    0x00,0x00,0x23,0xc4,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};*/
const uint8_t  AKHILAFLEXEventServiceClientRequirementsCharacteristicUUID[] = {
    0x00,0x00,0x23,0xc4,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

