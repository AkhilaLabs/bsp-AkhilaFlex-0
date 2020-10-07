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

#ifndef AKHILAFLEX_THERMOMETER_H
#define AKHILAFLEX_THERMOMETER_H

#include "mbed.h"
#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXComponent.h"
#include "AKHILAFLEXStorage.h"

#define AKHILAFLEX_THERMOMETER_PERIOD             1000


#define MAG3110_SAMPLE_RATES                    11

/*
 * Temperature events
 */
#define AKHILAFLEX_THERMOMETER_EVT_UPDATE         1

#define AKHILAFLEX_THERMOMETER_ADDED_TO_IDLE      2

/**
  * Class definition for AKHILAFLEX Thermometer.
  *
  * Infers and stores the ambient temoperature based on the surface temperature
  * of the various chips on the micro:bit.
  *
  */
class AKHILAFLEXThermometer : public AKHILAFLEXComponent
{
    unsigned long           sampleTime;
    uint32_t                samplePeriod;
    int16_t                 temperature;
    int16_t                 offset;
    AKHILAFLEXStorage*        storage;

    public:

    /**
      * Constructor.
      * Create new AKHILAFLEXThermometer that gives an indication of the current temperature.
      *
      * @param _storage an instance of AKHILAFLEXStorage used to persist temperature offset data
      *
      * @param id the unique EventModel id of this component. Defaults to AKHILAFLEX_ID_THERMOMETER.
      *
      * @code
      * AKHILAFLEXStorage storage;
      * AKHILAFLEXThermometer thermometer(storage);
      * @endcode
      */
    AKHILAFLEXThermometer(AKHILAFLEXStorage& _storage, uint16_t id = AKHILAFLEX_ID_THERMOMETER);

    /**
      * Constructor.
      * Create new AKHILAFLEXThermometer that gives an indication of the current temperature.
      *
      * @param id the unique EventModel id of this component. Defaults to AKHILAFLEX_ID_THERMOMETER.
      *
      * @code
      * AKHILAFLEXThermometer thermometer;
      * @endcode
      */
    AKHILAFLEXThermometer(uint16_t id = AKHILAFLEX_ID_THERMOMETER);

    /**
      * Set the sample rate at which the temperatureis read (in ms).
      *
      * The default sample period is 1 second.
      *
      * @param period the requested time between samples, in milliseconds.
      *
      * @note the temperature is always read in the background, and is only updated
      * when the processor is idle, or when the temperature is explicitly read.
      */
    void setPeriod(int period);

    /**
      * Reads the currently configured sample rate of the thermometer.
      *
      * @return The time between samples, in milliseconds.
      */
    int getPeriod();

    /**
      * Set the value that is used to offset the raw silicon temperature.
      *
      * @param offset the offset for the silicon temperature
      *
      * @return AKHILAFLEX_OK on success
      */
    int setOffset(int offset);

    /**
      * Retreive the value that is used to offset the raw silicon temperature.
      *
      * @return the current offset.
      */
    int getOffset();

    /**
      * This member function fetches the raw silicon temperature, and calculates
      * the value used to offset the raw silicon temperature based on a given temperature.
      *
      * @param calibrationTemp the temperature used to calculate the raw silicon temperature
      * offset.
      *
      * @return AKHILAFLEX_OK on success
      */
    int setCalibration(int calibrationTemp);

    /**
      * Gets the current temperature of the AKHILAFLEX.
      *
      * @return the current temperature, in degrees celsius.
      *
      * @code
      * thermometer.getTemperature();
      * @endcode
      */
    int getTemperature();

    /**
      * Updates the temperature sample of this instance of AKHILAFLEXThermometer
      * only if isSampleNeeded() indicates that an update is required.
      *
      * This call also will add the thermometer to fiber components to receive
      * periodic callbacks.
      *
      * @return AKHILAFLEX_OK on success.
      */
    int updateSample();

    /**
      * Periodic callback from AKHILAFLEX idle thread.
      */
    virtual void idleTick();

    private:

    /**
      * Determines if we're due to take another temperature reading
      *
      * @return 1 if we're due to take a temperature reading, 0 otherwise.
      */
    int isSampleNeeded();
};

#endif
