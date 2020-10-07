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
 * Class definition for a AKHILAFLEX Device Firmware Update loader.
 *
 * This is actually just a frontend to a memory resident nordic DFU loader.
 *
 * We rely on the BLE standard pairing processes to provide encryption and authentication.
 * We assume any device that is paied with the AKHILAFLEX is authorized to reprogram the device.
 *
 */
#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXDFUService.h"
#include "ble/UUID.h"
#include "AKHILAFLEXConfig.h"

#if !defined(__arm)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

/*
 * The underlying Nordic libraries that support BLE do not compile cleanly with the stringent GCC settings we employ
 * If we're compiling under GCC, then we suppress any warnings generated from this code (but not the rest of the DAL)
 * The ARM cc compiler is more tolerant. We don't test __GNUC__ here to detect GCC as ARMCC also typically sets this
 * as a compatability option, but does not support the options used...
 */
extern "C" {
#include "dfu_app_handler.h"
}

/*
 * Return to our predefined compiler settings.
 */
#if !defined(__arm)
#pragma GCC diagnostic pop
#endif


/**
  * Constructor.
  * Initialise the Device Firmware Update service.
  * @param _ble The instance of a BLE device that we're running on.
  */
/*AKHILAFLEXDFUService::AKHILAFLEXDFUService(BLEDevice &_ble) :
    ble(_ble) */
 AKHILAFLEXDFUService::AKHILAFLEXDFUService(BLEDevice &_ble) :
    ble(_ble) 
   
{
    // Opcodes can be issued here to control the AKHILAFLEXDFU Service, as defined above.
  /*  GattCharacteristic  AKHILAFLEXDFUServiceControlCharacteristic(AKHILAFLEXDFUServiceControlCharacteristicUUID, &controlByte, 0, sizeof(uint8_t),
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE); */
 GattCharacteristic  AKHILAFLEXDFUServiceControlCharacteristic(AKHILAFLEXDFUServiceControlCharacteristicUUID, &controlByte, 0, sizeof(uint8_t),
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE); 

    controlByte = 0x00;

    // Set default security requirements
  /*  AKHILAFLEXDFUServiceControlCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL); */
   AKHILAFLEXDFUServiceControlCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL); 

   /* GattCharacteristic *characteristics[] = {&AKHILAFLEXDFUServiceControlCharacteristic};
    GattService         service(AKHILAFLEXDFUServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *)); */
    GattCharacteristic *characteristics[] = {&AKHILAFLEXDFUServiceControlCharacteristic};
    GattService         service(AKHILAFLEXDFUServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

   /* AKHILAFLEXDFUServiceControlCharacteristicHandle = AKHILAFLEXDFUServiceControlCharacteristic.getValueHandle(); */
 AKHILAFLEXDFUServiceControlCharacteristicHandle = AKHILAFLEXDFUServiceControlCharacteristic.getValueHandle();

   /* ble.gattServer().write(AKHILAFLEXDFUServiceControlCharacteristicHandle, &controlByte, sizeof(uint8_t));
    ble.gattServer().onDataWritten(this, &AKHILAFLEXDFUService::onDataWritten); */
    ble.gattServer().write(AKHILAFLEXDFUServiceControlCharacteristicHandle, &controlByte, sizeof(uint8_t));
    ble.gattServer().onDataWritten(this, &AKHILAFLEXDFUService::onDataWritten); 

}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
/*void AKHILAFLEXDFUService::onDataWritten(const GattWriteCallbackParams *params) */
void AKHILAFLEXDFUService::onDataWritten(const GattWriteCallbackParams *params) 
{
   /* if (params->handle == AKHILAFLEXDFUServiceControlCharacteristicHandle) */
   if (params->handle == AKHILAFLEXDFUServiceControlCharacteristicHandle) 
    {
       /* if(params->len > 0 && params->data[0] == AKHILAFLEX_DFU_OPCODE_START_DFU) */
        if(params->len > 0 && params->data[0] == AKHILAFLEX_DFU_OPCODE_START_DFU) 
        {
            // TODO: Raise a SYSTEM event here.
            //uBit.display.stopAnimation();
            //uBit.display.clear();

/*#if CONFIG_ENABLED(AKHILAFLEX_DBG)*/
#if CONFIG_ENABLED(AKHILAFLEX_DBG) 
            printf("  ACTIVATING BOOTLOADER.\n");
#endif

            // Perform an explicit disconnection to assist our peer to reconnect to the DFU service
            ble.disconnect(Gap::REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF);

            wait_ms(1000);

            // Call bootloader_start implicitly trough a event handler call
            // it is a work around for bootloader_start not being public in sdk 8.1
            ble_dfu_t p_dfu;
            ble_dfu_evt_t p_evt;

            p_dfu.conn_handle = params->connHandle;
            p_evt.ble_dfu_evt_type = BLE_DFU_START;

            dfu_app_on_dfu_evt(&p_dfu, &p_evt);
        }
    }
}


/**
  * UUID definitions for BLE Services and Characteristics.
  */
/*
const uint8_t              AKHILAFLEXDFUServiceUUID[] = {
    0x00,0x00,0x93,0xb0,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t              AKHILAFLEXDFUServiceUUID[] = {
    0x00,0x00,0x93,0xb0,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 
/*
const uint8_t              AKHILAFLEXDFUServiceControlCharacteristicUUID[] = {
    0x00,0x00,0x93,0xb1,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t              AKHILAFLEXDFUServiceControlCharacteristicUUID[] = {
    0x00,0x00,0x93,0xb1,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

