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

#include "AKHILAFLEX.h"

#include "nrf_soc.h"

/*
 * Return to our predefined compiler settings.
 */
#if !defined(__arm)
#pragma GCC diagnostic pop
#endif

#if CONFIG_ENABLED(AKHILAFLEX_DBG)
// We create and initialize to NULL here, but AKHILAFLEXSerial will automatically update this as needed in its constructor.
RawSerial* SERIAL_DEBUG = NULL;
#endif

/**
  * Constructor.
  *
  * Create a representation of a AKHILAFLEX device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
AKHILAFLEX::AKHILAFLEX() :
    serial(USBTX, USBRX),
	resetButton(AKHILAFLEX_PIN_BUTTON_RESET),
    storage(),
    i2c(I2C_SDA0, I2C_SCL0),
    messageBus(),
    display(),
    buttonA(AKHILAFLEX_PIN_BUTTON_A, AKHILAFLEX_ID_BUTTON_A),
    buttonB(AKHILAFLEX_PIN_BUTTON_B, AKHILAFLEX_ID_BUTTON_B),
    buttonAB(AKHILAFLEX_ID_BUTTON_A,AKHILAFLEX_ID_BUTTON_B, AKHILAFLEX_ID_BUTTON_AB),
    accelerometer(AKHILAFLEXAccelerometer::autoDetect(i2c)),
    compass(AKHILAFLEXCompass::autoDetect(i2c)),
    compassCalibrator(compass, accelerometer, display, storage),
    thermometer(storage),
    io(AKHILAFLEX_ID_IO_P0,AKHILAFLEX_ID_IO_P1,AKHILAFLEX_ID_IO_P2,
       AKHILAFLEX_ID_IO_P3,AKHILAFLEX_ID_IO_P4,AKHILAFLEX_ID_IO_P5,
       AKHILAFLEX_ID_IO_P6,AKHILAFLEX_ID_IO_P7,AKHILAFLEX_ID_IO_P8,
       AKHILAFLEX_ID_IO_P9,AKHILAFLEX_ID_IO_P10,AKHILAFLEX_ID_IO_P11,
       AKHILAFLEX_ID_IO_P12,AKHILAFLEX_ID_IO_P13,AKHILAFLEX_ID_IO_P14,
       AKHILAFLEX_ID_IO_P15,AKHILAFLEX_ID_IO_P16,AKHILAFLEX_ID_IO_P19,
       AKHILAFLEX_ID_IO_P20),
    bleManager(storage),
    radio(),
    ble(NULL)
{
    // Clear our status
    status = 0;

    // Bring up soft reset functionality as soon as possible.
    resetButton.mode(PullUp);
    resetButton.fall(this, &AKHILAFLEX::reset);
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * uBit.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained by uBit.
  */
void AKHILAFLEX::init()
{
    if (status & AKHILAFLEX_INITIALIZED)
        return;

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    // Seed our random number generator
    seedRandom();

    // Create an event handler to trap any handlers being created for I2C services.
    // We do this to enable initialisation of those services only when they're used,
    // which saves processor time, memeory and battery life.
    messageBus.listen(AKHILAFLEX_ID_MESSAGE_BUS_LISTENER, AKHILAFLEX_EVT_ANY, this, &AKHILAFLEX::onListenerRegisteredEvent);

    status |= AKHILAFLEX_INITIALIZED;

#if CONFIG_ENABLED(AKHILAFLEX_BLE_PAIRING_MODE)
    int i=0;
    // Test if we need to enter BLE pairing mode
    // If a RebootMode Key has been set boot straight into BLE mode
    KeyValuePair* RebootMode = storage.get("RebootMode");
    KeyValuePair* flashIncomplete = storage.get("flashIncomplete");
    sleep(100);
    // Animation
    uint8_t x = 0; uint8_t y = 0;
    while ((buttonA.isPressed()  && i<25) || RebootMode != NULL || flashIncomplete != NULL)
    {
        display.image.setPixelValue(x,y,255);
        sleep(50);
        i++; x++;

        // Gradually fill screen
        if(x == 5){
          y++; x = 0;
        }

        if (i == 25 || RebootMode != NULL)
        {
            // Remove KV if it exists
            if(RebootMode != NULL){
                storage.remove("RebootMode");
            }
            delete RebootMode;
            delete flashIncomplete;

#if CONFIG_ENABLED(AKHILAFLEX_HEAP_REUSE_SD)
            AKHILAFLEX_create_heap(AKHILAFLEX_SD_GATT_TABLE_START + AKHILAFLEX_SD_GATT_TABLE_SIZE, AKHILAFLEX_SD_LIMIT);
#endif
            // Start the BLE stack, if it isn't already running.
            if (!ble)
            {
                bleManager.init(getName(), getSerial(), messageBus, true);
                ble = bleManager.ble;
            }

            // Enter pairing mode, using the LED matrix for any necessary pairing operations
            bleManager.pairingMode(display, buttonA);
        }
    }
#endif

    // Attempt to bring up a second heap region, using unused memory normally reserved for Soft Device.
#if CONFIG_ENABLED(AKHILAFLEX_HEAP_REUSE_SD)
#if CONFIG_ENABLED(AKHILAFLEX_BLE_ENABLED)
    AKHILAFLEX_create_heap(AKHILAFLEX_SD_GATT_TABLE_START + AKHILAFLEX_SD_GATT_TABLE_SIZE, AKHILAFLEX_SD_LIMIT);
#else
    AKHILAFLEX_create_heap(AKHILAFLEX_SRAM_BASE, AKHILAFLEX_SD_LIMIT);
#endif
#endif

#if CONFIG_ENABLED(AKHILAFLEX_BLE_ENABLED)
    // Start the BLE stack, if it isn't already running.
    if (!ble)
    {
        bleManager.init(getName(), getSerial(), messageBus, false);
        ble = bleManager.ble;
    }
#endif
    //display.rotateTo(AKHILAFLEX_DISPLAY_ROTATION_270);
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void AKHILAFLEX::onListenerRegisteredEvent(AKHILAFLEXEvent evt)
{
    switch(evt.value)
    {
        case AKHILAFLEX_ID_BUTTON_AB:
            // A user has registered to receive events from the buttonAB multibutton.
            // Disable click events from being generated by ButtonA and ButtonB, and defer the
            // control of this to the multibutton handler.
            //
            // This way, buttons look independent unless a buttonAB is requested, at which
            // point button A+B clicks can be correclty handled without breaking
            // causal ordering.
            buttonA.setEventConfiguration(AKHILAFLEX_BUTTON_SIMPLE_EVENTS);
            buttonB.setEventConfiguration(AKHILAFLEX_BUTTON_SIMPLE_EVENTS);
            buttonAB.setEventConfiguration(AKHILAFLEX_BUTTON_ALL_EVENTS);
            break;

        case AKHILAFLEX_ID_COMPASS:
            // A listener has been registered for the compass.
            // The compass uses lazy instantiation, we just need to read the data once to start it running.
            compass.getSample();

            break;

        case AKHILAFLEX_ID_ACCELEROMETER:
        case AKHILAFLEX_ID_GESTURE:
            // A listener has been registered for the accelerometer.
            // The accelerometer uses lazy instantiation, we just need to read the data once to start it running.
            accelerometer.getSample();
            break;

        case AKHILAFLEX_ID_THERMOMETER:
            // A listener has been registered for the thermometer.
            // The thermometer uses lazy instantiation, we just need to read the data once to start it running.
            thermometer.updateSample();
            break;
    }
}
