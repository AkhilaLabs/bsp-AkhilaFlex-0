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

#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXThermometer.h"
#include "AKHILAFLEXSystemTimer.h"
#include "AKHILAFLEXFiber.h"

/*
 * The underlying Nordic libraries that support BLE do not compile cleanly with the stringent GCC settings we employ
 * If we're compiling under GCC, then we suppress any warnings generated from this code (but not the rest of the DAL)
 * The ARM cc compiler is more tolerant. We don't test __GNUC__ here to detect GCC as ARMCC also typically sets this
 * as a compatability option, but does not support the options used...
 */
#if !defined(__arm)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "nrf_soc.h"
#include "nrf_sdm.h"

/*
 * Return to our predefined compiler settings.
 */
#if !defined(__arm)
#pragma GCC diagnostic pop
#endif

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
AKHILAFLEXThermometer::AKHILAFLEXThermometer(AKHILAFLEXStorage& _storage, uint16_t id) :
    storage(&_storage)
/*AKHILAFLEXThermometer::AKHILAFLEXThermometer(AKHILAFLEXStorage& _storage, uint16_t id) :
    storage(&_storage)  */  
{
    this->id = id;
    this->samplePeriod = AKHILAFLEX_THERMOMETER_PERIOD;
   /* this->samplePeriod = AKHILAFLEX_THERMOMETER_PERIOD; */
    this->sampleTime = 0;
    this->offset = 0;

    KeyValuePair *tempCalibration =  storage->get("tempCal");

    if(tempCalibration != NULL)
    {
        memcpy(&offset, tempCalibration->value, sizeof(int16_t));
        delete tempCalibration;
    }
}

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
AKHILAFLEXThermometer::AKHILAFLEXThermometer(uint16_t id) :
/*AKHILAFLEXThermometer::AKHILAFLEXThermometer(uint16_t id) : */	
    storage(NULL)
{
    this->id = id;
    this->samplePeriod = AKHILAFLEX_THERMOMETER_PERIOD;
   /* this->samplePeriod = AKHILAFLEX_THERMOMETER_PERIOD; */
    this->sampleTime = 0;
    this->offset = 0;
}

/**
  * Gets the current temperature of the AKHILAFLEX.
  *
  * @return the current temperature, in degrees celsius.
  *
  * @code
  * thermometer.getTemperature();
  * @endcode
  */
int AKHILAFLEXThermometer::getTemperature()
/*int AKHILAFLEXThermometer::getTemperature() */
{
    updateSample();
    return temperature - offset;
}


/**
  * Updates the temperature sample of this instance of AKHILAFLEXThermometer
  * only if isSampleNeeded() indicates that an update is required.
  *
  * This call also will add the thermometer to fiber components to receive
  * periodic callbacks.
  *
  * @return AKHILAFLEX_OK on success.
  */
int AKHILAFLEXThermometer::updateSample()
/*int AKHILAFLEXThermometer::updateSample() */
{
    if(!(status & AKHILAFLEX_THERMOMETER_ADDED_TO_IDLE))
   /* if(!(status & AKHILAFLEX_THERMOMETER_ADDED_TO_IDLE)) */
    {
        // If we're running under a fiber scheduer, register ourselves for a periodic callback to keep our data up to date.
        // Otherwise, we do just do this on demand, when polled through our read() interface.
        fiber_add_idle_component(this);
        status |= AKHILAFLEX_THERMOMETER_ADDED_TO_IDLE;
       /* status |= AKHILAFLEX_THERMOMETER_ADDED_TO_IDLE; */
    }

    // check if we need to update our sample...
    if(isSampleNeeded())
    {
        int32_t processorTemperature;
        uint8_t sd_enabled;

        // For now, we just rely on the nrf senesor to be the most accurate.
        // The compass module also has a temperature sensor, and has the lowest power consumption, so will run the cooler...
        // ...however it isn't trimmed for accuracy during manufacture, so requires calibration.

        sd_softdevice_is_enabled(&sd_enabled);

        if (sd_enabled)
        {
            // If Bluetooth is enabled, we need to go through the Nordic software to safely do this
            sd_temp_get(&processorTemperature);
        }
        else
        {
            // Othwerwise, we access the information directly...
            uint32_t *TEMP = (uint32_t *)0x4000C508;

            NRF_TEMP->TASKS_START = 1;

            while (NRF_TEMP->EVENTS_DATARDY == 0);

            NRF_TEMP->EVENTS_DATARDY = 0;

            processorTemperature = *TEMP;

            NRF_TEMP->TASKS_STOP = 1;
        }


        // Record our reading...
        temperature = processorTemperature / 4;

        // Schedule our next sample.
        sampleTime = system_timer_current_time() + samplePeriod;

        // Send an event to indicate that we'e updated our temperature.
        AKHILAFLEXEvent e(id, AKHILAFLEX_THERMOMETER_EVT_UPDATE);
       /* AKHILAFLEXEvent e(id, AKHILAFLEX_THERMOMETER_EVT_UPDATE); */
    }

    return AKHILAFLEX_OK;
   /* return AKHILAFLEX_OK; */
};

/**
  * Periodic callback from AKHILAFLEX idle thread.
  */
void AKHILAFLEXThermometer::idleTick()
/*void AKHILAFLEXThermometer::idleTick() */
{
    updateSample();
}

/**
  * Determines if we're due to take another temperature reading
  *
  * @return 1 if we're due to take a temperature reading, 0 otherwise.
  */
int AKHILAFLEXThermometer::isSampleNeeded()
/*int AKHILAFLEXThermometer::isSampleNeeded() */
{
    return  system_timer_current_time() >= sampleTime;
}

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
void AKHILAFLEXThermometer::setPeriod(int period)
/*void AKHILAFLEXThermometer::setPeriod(int period) */
{
    updateSample();
    samplePeriod = period;
}

/**
  * Reads the currently configured sample rate of the thermometer.
  *
  * @return The time between samples, in milliseconds.
  */
int AKHILAFLEXThermometer::getPeriod()
/*int AKHILAFLEXThermometer::getPeriod() */
{
    return samplePeriod;
}

/**
  * Set the value that is used to offset the raw silicon temperature.
  *
  * @param offset the offset for the silicon temperature
  *
  * @return AKHILAFLEX_OK on success
  */
int AKHILAFLEXThermometer::setOffset(int offset)
/*int AKHILAFLEXThermometer::setOffset(int offset) */
{
    if(this->storage != NULL)
        this->storage->put(ManagedString("tempCal"), (uint8_t *)&offset, sizeof(int));

    this->offset = offset;

    return AKHILAFLEX_OK;
    /* return AKHILAFLEX_OK; */
}

/**
  * Retreive the value that is used to offset the raw silicon temperature.
  *
  * @return the current offset.
  */
int AKHILAFLEXThermometer::getOffset()
/*int AKHILAFLEXThermometer::getOffset() */
{
    return offset;
}

/**
  * This member function fetches the raw silicon temperature, and calculates
  * the value used to offset the raw silicon temperature based on a given temperature.
  *
  * @param calibrationTemp the temperature used to calculate the raw silicon temperature
  * offset.
  *
  * @return AKHILAFLEX_OK on success
  */
int AKHILAFLEXThermometer::setCalibration(int calibrationTemp)
/*int AKHILAFLEXThermometer::setCalibration(int calibrationTemp) */
{
    updateSample();
    return setOffset(temperature - calibrationTemp);
}
