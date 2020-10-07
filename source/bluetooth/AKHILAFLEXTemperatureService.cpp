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
  * Class definition for the custom MicroBit Temperature Service.
  * Provides a BLE service to remotely read the silicon temperature of the nRF51822.
  */
#include "AKHILAFLEXConfig.h"
#include "ble/UUID.h"

#include "AKHILAFLEXTemperatureService.h"

/**
  * Constructor.
  * Create a representation of the TemperatureService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _thermometer An instance of MicroBitThermometer to use as our temperature source.
  */
/*MicroBitTemperatureService::MicroBitTemperatureService(BLEDevice &_ble, MicroBitThermometer &_thermometer) :     
        ble(_ble), thermometer(_thermometer) */
AKHILAFLEXTemperatureService::AKHILAFLEXTemperatureService(BLEDevice &_ble, AKHILAFLEXThermometer &_thermometer) :
        ble(_ble), thermometer(_thermometer) 

{
    // Create the data structures that represent each of our characteristics in Soft Device.
   /* GattCharacteristic  temperatureDataCharacteristic(MicroBitTemperatureServiceDataUUID, (uint8_t *)&temperatureDataCharacteristicBuffer, 0,
    sizeof(temperatureDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY); */
 GattCharacteristic  temperatureDataCharacteristic(AKHILAFLEXTemperatureServiceDataUUID, (uint8_t *)&temperatureDataCharacteristicBuffer, 0,
  sizeof(temperatureDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

   /* GattCharacteristic  temperaturePeriodCharacteristic(MicroBitTemperatureServicePeriodUUID, (uint8_t *)&temperaturePeriodCharacteristicBuffer, 0,
    sizeof(temperaturePeriodCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE); */
GattCharacteristic  temperaturePeriodCharacteristic(AKHILAFLEXTemperatureServicePeriodUUID, (uint8_t *)&temperaturePeriodCharacteristicBuffer, 0,
sizeof(temperaturePeriodCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);
                                                                                                                                                      
    // Initialise our characteristic values.
    temperatureDataCharacteristicBuffer = 0;
    temperaturePeriodCharacteristicBuffer = thermometer.getPeriod();

    // Set default security requirements
  /*  temperatureDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    temperaturePeriodCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL); */
    temperatureDataCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL);
    temperaturePeriodCharacteristic.requireSecurity(SecurityManager::AKHILAFLEX_BLE_SECURITY_LEVEL); 


  /*  GattCharacteristic *characteristics[] = {&temperatureDataCharacteristic, &temperaturePeriodCharacteristic};
    GattService         service(MicroBitTemperatureServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *)); */
GattCharacteristic *characteristics[] = {&temperatureDataCharacteristic, &temperaturePeriodCharacteristic};
    GattService        service(AKHILAFLEXTemperatureServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    temperatureDataCharacteristicHandle = temperatureDataCharacteristic.getValueHandle();
    temperaturePeriodCharacteristicHandle = temperaturePeriodCharacteristic.getValueHandle();

    ble.gattServer().write(temperatureDataCharacteristicHandle,(uint8_t *)&temperatureDataCharacteristicBuffer, sizeof(temperatureDataCharacteristicBuffer));
    ble.gattServer().write(temperaturePeriodCharacteristicHandle,(uint8_t *)&temperaturePeriodCharacteristicBuffer, sizeof(temperaturePeriodCharacteristicBuffer));

   /* ble.onDataWritten(this, &MicroBitTemperatureService::onDataWritten); */
    ble.onDataWritten(this, &AKHILAFLEXTemperatureService::onDataWritten); 
    if (EventModel::defaultEventBus)
  EventModel::defaultEventBus->listen(AKHILAFLEX_ID_THERMOMETER, AKHILAFLEX_THERMOMETER_EVT_UPDATE, this, &AKHILAFLEXTemperatureService::temperatureUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Temperature update callback
  */
/*void MicroBitTemperatureService::temperatureUpdate(MicroBitEvent) */
void AKHILAFLEXTemperatureService::temperatureUpdate(AKHILAFLEXEvent) 
{
    if (ble.getGapState().connected)
    {
        temperatureDataCharacteristicBuffer = thermometer.getTemperature();
      ble.gattServer().notify(temperatureDataCharacteristicHandle,(uint8_t *)&temperatureDataCharacteristicBuffer, sizeof(temperatureDataCharacteristicBuffer));
    }
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
/*void MicroBitTemperatureService::onDataWritten(const GattWriteCallbackParams *params) */
void AKHILAFLEXTemperatureService::onDataWritten(const GattWriteCallbackParams *params) 
{
   if (params->handle == temperaturePeriodCharacteristicHandle && params->len >= sizeof(temperaturePeriodCharacteristicBuffer))
    {
        temperaturePeriodCharacteristicBuffer = *((uint16_t *)params->data);
        thermometer.setPeriod(temperaturePeriodCharacteristicBuffer);

        // The accelerometer will choose the nearest period to that requested that it can support
        // Read back the ACTUAL period it is using, and report this back.
        temperaturePeriodCharacteristicBuffer = thermometer.getPeriod();
        ble.gattServer().write(temperaturePeriodCharacteristicHandle, (const uint8_t *)&temperaturePeriodCharacteristicBuffer, sizeof(temperaturePeriodCharacteristicBuffer));
    }
}

/*
const uint8_t  MicroBitTemperatureServiceUUID[] = {
    0x00,0x00,0x61,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXTemperatureServiceUUID[] = {
    0x00,0x00,0x61,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 

/*
const uint8_t  MicroBitTemperatureServiceDataUUID[] = {
    0x00,0x00,0x92,0x50,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXTemperatureServiceDataUUID[] = {
    0x00,0x00,0x92,0x50,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 

/*
const uint8_t  MicroBitTemperatureServicePeriodUUID[] = {
    0x00,0x00,0x1b,0x25,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; */
const uint8_t  AKHILAFLEXTemperatureServicePeriodUUID[] = {
    0x00,0x00,0x1b,0x25,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
}; 

