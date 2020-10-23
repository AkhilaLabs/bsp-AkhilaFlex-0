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

#ifndef AKHILAFLEX_LED_SERVICE_H
#define AKHILAFLEX_LED_SERVICE_H

#include "AKHILAFLEXConfig.h"
#include "ble/BLE.h"
#include "AKHILAFLEXDisplay.h"

// Defines the buffer size for scrolling text over BLE, hence also defines
// the maximum string length that can be scrolled via the BLE service.
#define AKHILAFLEX_BLE_MAXIMUM_SCROLLTEXT         20

// UUIDs for our service and characteristics
extern const uint8_t  AKHILAFLEXLEDServiceUUID[];
extern const uint8_t  AKHILAFLEXLEDServiceMatrixUUID[];
extern const uint8_t  AKHILAFLEXLEDServiceTextUUID[];
extern const uint8_t  AKHILAFLEXLEDServiceScrollingSpeedUUID[];


/**
  * Class definition for the custom AKHILAFLEX LED Service.
  * Provides a BLE service to remotely read and write the state of the LED display.
  */
class AKHILAFLEXLEDService
{
    public:

    /**
      * Constructor.
      * Create a representation of the LEDService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _display An instance of AKHILAFLEXDisplay to interface with.
      */
    AKHILAFLEXLEDService(BLEDevice &_ble, AKHILAFLEXDisplay &_display);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const GattWriteCallbackParams *params);

    /**
      * Callback. Invoked when any of our attributes are read via BLE.
      */
    void onDataRead(GattReadAuthCallbackParams *params);

    private:

    // Bluetooth stack we're running on.
    BLEDevice           &ble;
    AKHILAFLEXDisplay     &display;

    // memory for our 8 bit control characteristics.
    uint8_t             matrixCharacteristicBuffer[5];
    uint16_t            scrollingSpeedCharacteristicBuffer;
    uint8_t             textCharacteristicBuffer[AKHILAFLEX_BLE_MAXIMUM_SCROLLTEXT];

    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t matrixCharacteristicHandle;
    GattAttribute::Handle_t textCharacteristicHandle;
    GattAttribute::Handle_t scrollingSpeedCharacteristicHandle;

    // We hold a copy of the GattCharacteristic, as mbed's BLE API requires this to provide read callbacks (pity!).
    GattCharacteristic  matrixCharacteristic;
};


#endif
