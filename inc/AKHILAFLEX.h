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

#ifndef AKHILAFLEX_H
#define AKHILAFLEX_H

#include "mbed.h"

#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXHeapAllocator.h"
#include "AKHILAFLEXDevice.h"
#include "ErrorNo.h"
#include "AKHILAFLEXSystemTimer.h"
#include "AKHILAFLEXCompat.h"
#include "AKHILAFLEXComponent.h"
#include "ManagedType.h"
#include "ManagedString.h"
#include "AKHILAFLEXImage.h"
#include "AKHILAFLEXFont.h"
#include "AKHILAFLEXEvent.h"
#include "DynamicPwm.h"
#include "AKHILAFLEXI2C.h"
#include "NotifyEvents.h"

#include "AKHILAFLEXButton.h"
#include "AKHILAFLEXPin.h"
#include "AKHILAFLEXCompass.h"
#include "AKHILAFLEXCompassCalibrator.h"
#include "AKHILAFLEXAccelerometer.h"
#include "AKHILAFLEXThermometer.h"
#include "AKHILAFLEXLightSensor.h"
#include "AKHILAFLEXMultiButton.h"

#include "AKHILAFLEXSerial.h"
#include "AKHILAFLEXIO.h"
#include "AKHILAFLEXMatrixMaps.h"
#include "AKHILAFLEXDisplay.h"

#include "AKHILAFLEXFiber.h"
#include "AKHILAFLEXMessageBus.h"

#include "AKHILAFLEXBLEManager.h"
#include "AKHILAFLEXRadio.h"
#include "AKHILAFLEXStorage.h"

// AKHILAFLEX::flags values
#define AKHILAFLEX_INITIALIZED                    0x01

/**
  * Class definition for a AKHILAFLEX device.
  *
  * Represents the device as a whole, and includes member variables that represent various device drivers
  * used to control aspects of the AKHILAFLEX.
  */
class AKHILAFLEX
{
    private:

    /**
      * A listener to perform actions as a result of Message Bus reflection.
      *
      * In some cases we want to perform lazy instantiation of components, such as
      * the compass and the accelerometer, where we only want to add them to the idle
      * fiber when someone has the intention of using these components.
      */
    void                        onListenerRegisteredEvent(AKHILAFLEXEvent evt);

    uint8_t                     status;

    public:

    // Serial Interface
    AKHILAFLEXSerial              serial;

	// Reset Button
	InterruptIn     		    resetButton;

    // Persistent key value store
    AKHILAFLEXStorage             storage;

    // I2C Interface
    AKHILAFLEXI2C                 i2c;

    // Device level Message Bus abstraction
    AKHILAFLEXMessageBus          messageBus;

    // Member variables to represent each of the core components on the device.
    AKHILAFLEXDisplay             display;
    AKHILAFLEXButton              buttonA;
    AKHILAFLEXButton              buttonB;
    AKHILAFLEXMultiButton         buttonAB;
    AKHILAFLEXAccelerometer       &accelerometer;
    AKHILAFLEXCompass             &compass;
    AKHILAFLEXCompassCalibrator   compassCalibrator;
    AKHILAFLEXThermometer         thermometer;

    //An object of available IO pins on the device
    AKHILAFLEXIO                  io;

    // Bluetooth related member variables.
	AKHILAFLEXBLEManager		    bleManager;
    AKHILAFLEXRadio               radio;
    BLEDevice                   *ble;

    /**
      * Constructor.
      *
      * Create a representation of a AKHILAFLEX device, which includes member variables
      * that represent various device drivers used to control aspects of the AKHILAFLEX.
      */
    AKHILAFLEX();

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
    void init();

    /**
      * Return the friendly name for this device.
      *
      * @return A ManagedString representing the friendly name of this device.
      *
      * @code
      * ManagedString name = uBit.getName();
      * @endcode
      */
    static ManagedString getName();

    /**
      * Return the serial number of this device.
      *
      * @return A ManagedString representing the serial number of this device.
      *
      * @code
      * ManagedString serialNumber = uBit.getSerial();
      * @endcode
      */
    static ManagedString getSerial();

    /**
      * Will reset the AKHILAFLEX when called.
      *
      * @code
      * uBit.reset();
      * @endcode
      */
    void reset();

    /**
      * Delay execution for the given amount of time.
      *
      * If the scheduler is running, this will deschedule the current fiber and perform
      * a power efficient, concurrent sleep operation.
      *
      * If the scheduler is disabled or we're running in an interrupt context, this
      * will revert to a busy wait.
      *
      * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
      * operation.
      *
      * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
      *
      * @return AKHILAFLEX_OK on success, AKHILAFLEX_INVALID_PARAMETER milliseconds is less than zero.
      *
      * @code
      * uBit.sleep(20); //sleep for 20ms
      * @endcode
      *
      * @note This operation is currently limited by the rate of the system timer, therefore
      *       the granularity of the sleep operation is limited to 6 ms unless the rate of
      *       the system timer is modified.
      */
    void sleep(uint32_t milliseconds);

    /**
      * Seed the pseudo random number generator using the hardware random number generator.
      *
      * @code
      * uBit.seedRandom();
      * @endcode
      */
    void seedRandom();

    /**
      * Seed the pseudo random number generator using the given value.
      *
      * @param seed The 32-bit value to seed the generator with.
      *
      * @code
      * uBit.seedRandom(0xBB5EED);
      * @endcode
      */
    void seedRandom(uint32_t seed);


    /**
      * Generate a random number in the given range.
      * We use a simple Galois LFSR random number generator here,
      * as a Galois LFSR is sufficient for our applications, and much more lightweight
      * than the hardware random number generator built int the processor, which takes
      * a long time and uses a lot of energy.
      *
      * KIDS: You shouldn't use this in the real world to generate cryptographic keys though...
      * have a think why not. :-)
      *
      * @param max the upper range to generate a number for. This number cannot be negative.
      *
      * @return A random, natural number between 0 and the max-1. Or AKHILAFLEX_INVALID_VALUE if max is <= 0.
      *
      * @code
      * uBit.random(200); //a number between 0 and 199
      * @endcode
      */
    int random(int max);

    /**
      * Determine the time since this AKHILAFLEX was last reset.
      *
      * @return The time since the last reset, in milliseconds.
      *
      * @note This will value overflow after 1.6 months.
      */
      //TODO: handle overflow case.
    unsigned long systemTime();

    /**
      * Determine the version of the AKHILAFLEX runtime currently in use.
      *
      * @return A textual description of the version of the AKHILAFLEX runtime that
      *         is currently running on this device.
      */
    const char *systemVersion();

    /**
      * Triggers a AKHILAFLEX panic where an loop will display a panic face
      * and the status code, if provided.
      *
      * This loop will continue for panic_timeout iterations, defaults to 0 (infinite).
      *
      * panic_timeout can be configured via a call to AKHILAFLEX_panic_timeout.
      *
      * @param statusCode the status code of the associated error.
      *
      * @code
      * AKHILAFLEX_panic_timeout(4);
      *
      * // will display loop for 4 iterations.
      * uBit.panic(10);
      * @endcode
      */
    void panic(int statusCode = 0);

    /**
      * Add a component to the array of system components. This component will then receive
      * periodic callbacks, once every tick period in interrupt context.
      *
      * @param component The component to add.
      *
      * @return AKHILAFLEX_OK on success or AKHILAFLEX_NO_RESOURCES if the component array is full.
      *
      * @code
      * // heap allocated - otherwise it will be paged out!
      * AKHILAFLEXDisplay* display = new AKHILAFLEXDisplay();
      *
      * uBit.addSystemComponent(display);
      * @endcode
      *
      * @note This interface is now deprecated, and will be removed in the next major release. Please use system_timer_add_component().
      */
	int addSystemComponent(AKHILAFLEXComponent *component);

    /**
      * Remove a component from the array of system components. This component will no longer receive
      * periodic callbacks.
      *
      * @param component The component to remove.
      *
      * @return AKHILAFLEX_OK on success or AKHILAFLEX_INVALID_PARAMETER is returned if the given component has not been previously added.
      *
      * @code
      * // heap allocated - otherwise it will be paged out!
      * AKHILAFLEXDisplay* display = new AKHILAFLEXDisplay();
      *
      * uBit.addSystemComponent(display);
      *
      * uBit.removeSystemComponent(display);
      * @endcode
      *
      * @note This interface is now deprecated, and will be removed in the next major release. Please use system_timer_remove_component().
      */
	int removeSystemComponent(AKHILAFLEXComponent *component);

    /**
      * Adds a component to the array of idle thread components, which are processed
      * when the run queue is empty.
      *
      * The system timer will poll isIdleCallbackNeeded on each component to determine
      * if the scheduler should schedule the idle_task imminently.
      *
      * @param component The component to add to the array.
      *
      * @return AKHILAFLEX_OK on success or AKHILAFLEX_NO_RESOURCES if the fiber components array is full.
      *
      * @code
      * AKHILAFLEXI2C i2c(I2C_SDA0, I2C_SCL0);
      *
      * // heap allocated - otherwise it will be paged out!
      * AKHILAFLEXAccelerometer* accelerometer = new AKHILAFLEXAccelerometer(i2c);
      *
      * fiber_add_idle_component(accelerometer);
      * @endcode
      *
      * @note This interface is now deprecated, and will be removed in the next major release. Please use fiber_add_idle_component().
      */
	int addIdleComponent(AKHILAFLEXComponent *component);

    /**
      * Remove a component from the array of idle thread components
      *
      * @param component The component to remove from the idle component array.
      *
      * @return AKHILAFLEX_OK on success. AKHILAFLEX_INVALID_PARAMETER is returned if the given component has not been previously added.
      *
      * @code
      * AKHILAFLEXI2C i2c(I2C_SDA0, I2C_SCL0);
      *
      * // heap allocated - otherwise it will be paged out!
      * AKHILAFLEXAccelerometer* accelerometer = new AKHILAFLEXAccelerometer(i2c);
      *
      * uBit.addIdleComponent(accelerometer);
      *
      * uBit.removeIdleComponent(accelerometer);
      * @endcode
      *
      * @note This interface is now deprecated, and will be removed in the next major release. Please use fiber_remove_idle_component().
      */
	int removeIdleComponent(AKHILAFLEXComponent *component);
};

/**
  * Return the friendly name for this device.
  *
  * @return A ManagedString representing the friendly name of this device.
  *
  * @code
  * ManagedString name = uBit.getName();
  * @endcode
  */
inline ManagedString AKHILAFLEX::getName()
{
    return ManagedString(AKHILAFLEX_friendly_name());
}

/**
  * Return the serial number of this device.
  *
  * @return A ManagedString representing the serial number of this device.
  *
  * @code
  * ManagedString serialNumber = uBit.getSerial();
  * @endcode
  */
inline ManagedString AKHILAFLEX::getSerial()
{
    // We take to 16 bit numbers here, as we want the full range of ID bits, but don't want negative numbers...
    int n1 = AKHILAFLEX_serial_number() & 0xffff;
    int n2 = (AKHILAFLEX_serial_number() >> 16) & 0xffff;

    // Simply concat the two numbers.
    ManagedString s1(n1);
    ManagedString s2(n2);

    return s1 + s2;
}

/**
  * Will reset the AKHILAFLEX when called.
  *
  * @code
  * uBit.reset();
  * @endcode
  */
inline void AKHILAFLEX::reset()
{
    if(ble && ble->getGapState().connected) {

        // We have a connected BLE peer. Disconnect the BLE session.
        ble->gap().disconnect(Gap::REMOTE_USER_TERMINATED_CONNECTION);

        // Wait a little while for the connection to drop.
        wait_ms(100);
    }

    AKHILAFLEX_reset();
}

/**
  * Delay execution for the given amount of time.
  *
  * If the scheduler is running, this will deschedule the current fiber and perform
  * a power efficient, concurrent sleep operation.
  *
  * If the scheduler is disabled or we're running in an interrupt context, this
  * will revert to a busy wait.
  *
  * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
  * operation.
  *
  * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
  *
  * @return AKHILAFLEX_OK on success, AKHILAFLEX_INVALID_PARAMETER milliseconds is less than zero.
  *
  * @code
  * uBit.sleep(20); //sleep for 20ms
  * @endcode
  *
  * @note This operation is currently limited by the rate of the system timer, therefore
  *       the granularity of the sleep operation is limited to 6 ms unless the rate of
  *       the system timer is modified.
  */
inline void AKHILAFLEX::sleep(uint32_t milliseconds)
{
    fiber_sleep(milliseconds);
}

/**
  * Generate a random number in the given range.
  * We use a simple Galois LFSR random number generator here,
  * as a Galois LFSR is sufficient for our applications, and much more lightweight
  * than the hardware random number generator built int the processor, which takes
  * a long time and uses a lot of energy.
  *
  * KIDS: You shouldn't use this is the real world to generate cryptographic keys though...
  * have a think why not. :-)
  *
  * @param max the upper range to generate a number for. This number cannot be negative.
  *
  * @return A random, natural number between 0 and the max-1. Or AKHILAFLEX_INVALID_VALUE if max is <= 0.
  *
  * @code
  * uBit.random(200); //a number between 0 and 199
  * @endcode
  */
inline int AKHILAFLEX::random(int max)
{
    return AKHILAFLEX_random(max);
}

/**
  * Seed the pseudo random number generator using the hardware random number generator.
  *
  * @code
  * uBit.seedRandom();
  * @endcode
  */
inline void AKHILAFLEX::seedRandom()
{
    AKHILAFLEX_seed_random();
}


/**
  * Seed the pseudo random number generator using the given value.
  *
  * @param seed The 32-bit value to seed the generator with.
  *
  * @code
  * uBit.seedRandom(0xBB5EED);
  * @endcode
  */
inline void AKHILAFLEX::seedRandom(uint32_t seed)
{
    AKHILAFLEX_seed_random(seed);
}


/**
  * Add a component to the array of system components. This component will then receive
  * periodic callbacks, once every tick period in interrupt context.
  *
  * @param component The component to add.
  *
  * @return AKHILAFLEX_OK on success or AKHILAFLEX_NO_RESOURCES if the component array is full.
  *
  * @code
  * // heap allocated - otherwise it will be paged out!
  * AKHILAFLEXDisplay* display = new AKHILAFLEXDisplay();
  *
  * uBit.addSystemComponent(display);
  * @endcode
  *
  * @note This interface is now deprecated, and will be removed in the next major release. Please use system_timer_add_component().
  */
inline int AKHILAFLEX::addSystemComponent(AKHILAFLEXComponent *component)
{
	return system_timer_add_component(component);
}

/**
  * Remove a component from the array of system components. This component will no longer receive
  * periodic callbacks.
  *
  * @param component The component to remove.
  *
  * @return AKHILAFLEX_OK on success or AKHILAFLEX_INVALID_PARAMETER is returned if the given component has not been previously added.
  *
  * @code
  * // heap allocated - otherwise it will be paged out!
  * AKHILAFLEXDisplay* display = new AKHILAFLEXDisplay();
  *
  * uBit.addSystemComponent(display);
  *
  * uBit.removeSystemComponent(display);
  * @endcode
  *
  * @note This interface is now deprecated, and will be removed in the next major release. Please use system_timer_remove_component().
  */
inline int AKHILAFLEX::removeSystemComponent(AKHILAFLEXComponent *component)
{
	return system_timer_remove_component(component);
}

/**
  * Adds a component to the array of idle thread components, which are processed
  * when the run queue is empty.
  *
  * The system timer will poll isIdleCallbackNeeded on each component to determine
  * if the scheduler should schedule the idle_task imminently.
  *
  * @param component The component to add to the array.
  *
  * @return AKHILAFLEX_OK on success or AKHILAFLEX_NO_RESOURCES if the fiber components array is full.
  *
  * @code
  * AKHILAFLEXI2C i2c(I2C_SDA0, I2C_SCL0);
  *
  * // heap allocated - otherwise it will be paged out!
  * AKHILAFLEXAccelerometer* accelerometer = new AKHILAFLEXAccelerometer(i2c);
  *
  * fiber_add_idle_component(accelerometer);
  * @endcode
  *
  * @note This interface is now deprecated, and will be removed in the next major release. Please use fiber_add_idle_component().
  */
inline int AKHILAFLEX::addIdleComponent(AKHILAFLEXComponent *component)
{
	return fiber_add_idle_component(component);
}

/**
  * Remove a component from the array of idle thread components
  *
  * @param component The component to remove from the idle component array.
  *
  * @return AKHILAFLEX_OK on success. AKHILAFLEX_INVALID_PARAMETER is returned if the given component has not been previously added.
  *
  * @code
  * AKHILAFLEXI2C i2c(I2C_SDA0, I2C_SCL0);
  *
  * // heap allocated - otherwise it will be paged out!
  * AKHILAFLEXAccelerometer* accelerometer = new AKHILAFLEXAccelerometer(i2c);
  *
  * uBit.addIdleComponent(accelerometer);
  *
  * uBit.removeIdleComponent(accelerometer);
  * @endcode
  *
  * @note This interface is now deprecated, and will be removed in the next major release. Please use fiber_remove_idle_component().
  */
inline int AKHILAFLEX::removeIdleComponent(AKHILAFLEXComponent *component)
{
	return fiber_remove_idle_component(component);
}


/**
  * Determine the time since this AKHILAFLEX was last reset.
  *
  * @return The time since the last reset, in milliseconds.
  *
  * @note This will value overflow after 1.6 months.
  */
inline unsigned long AKHILAFLEX::systemTime()
{
    return system_timer_current_time();
}


/**
  * Determine the version of the AKHILAFLEX runtime currently in use.
  *
  * @return A textual description of the version of the AKHILAFLEX runtime that
  *         is currently running on this device.
  */
inline const char *AKHILAFLEX::systemVersion()
{
    return AKHILAFLEX_dal_version();
}

/**
  * Triggers a AKHILAFLEX panic where an loop will display a panic face
  * and the status code, if provided.
  *
  * This loop will continue for panic_timeout iterations, defaults to 0 (infinite).
  *
  * panic_timeout can be configured via a call to AKHILAFLEX_panic_timeout.
  *
  * @param statusCode the status code of the associated error.
  *
  * @code
  * AKHILAFLEX_panic_timeout(4);
  *
  * // will display loop for 4 iterations.
  * uBit.panic(10);
  * @endcode
  */
inline void AKHILAFLEX::panic(int statusCode)
{
    //show error and enter infinite while
	AKHILAFLEX_panic(statusCode);
}

#endif
