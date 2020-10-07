/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

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
 * Class definition for an FXS8700 3 axis accelerometer.
 *
 * Represents an implementation of the Freescale FXS8700 3 axis accelerometer
 * Also includes basic data caching and on demand activation.
 */
#include "FXOS8700.h"
#include "ErrorNo.h"
#include "AKHILAFLEXEvent.h"
#include "AKHILAFLEXCompat.h"
#include "AKHILAFLEXFiber.h"
#include "AKHILAFLEXAccelerometer.h"

//
// Configuration table for available g force ranges.
// Maps g -> XYZ_DATA_CFG bit [0..1]
//
static const KeyValueTableEntry accelerometerRangeData[] = {
    {2,0},
    {4,1},
    {8,2}
};
CREATE_KEY_VALUE_TABLE(accelerometerRange, accelerometerRangeData);

//
// Configuration table for available data update frequency.
// maps microsecond period -> CTRL_REG1 data rate selection bits [3..5]
//
static const KeyValueTableEntry accelerometerPeriodData[] = {
    {2500,0x00},
    {5000,0x08},
    {10000,0x10},
    {20000,0x18},
    {80000,0x20},
    {160000,0x28},
    {320000,0x30},
    {1280000,0x38}
};
CREATE_KEY_VALUE_TABLE(accelerometerPeriod, accelerometerPeriodData);


/**
  * Configures the accelerometer for G range and sample rate defined
  * in this object. The nearest values are chosen to those defined
  * that are supported by the hardware. The instance variables are then
  * updated to reflect reality.
  *
  * @return AKHILAFLEX_OK on success, AKHILAFLEX_I2C_ERROR if the accelerometer could not be configured.
  */
int FXOS8700::configure()
{
    int result;
    uint8_t value;

    // First find the nearest sample rate to that specified.
    AKHILAFLEXAccelerometer::samplePeriod = accelerometerPeriod.getKey(AKHILAFLEXAccelerometer::samplePeriod * 2000) / 1000;
    AKHILAFLEXAccelerometer::sampleRange = accelerometerRange.getKey(AKHILAFLEXAccelerometer::sampleRange);
    AKHILAFLEXCompass::samplePeriod = AKHILAFLEXAccelerometer::samplePeriod;

    // Now configure the accelerometer accordingly.
    // Firstly, disable the module (as some registers cannot be changed while its running).
    value = 0x00;
    result = i2c.writeRegister(address, FXOS8700_CTRL_REG1, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Enter hybrid mode (interleave accelerometer and magnetometer samples).
    // Also, select full oversampling on the magnetometer
    // TODO: Determine power / accuracy tradeoff here.
    value = 0x1F;
    result = i2c.writeRegister(address, FXOS8700_M_CTRL_REG1, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Select the auto incremement mode, which allows a contiguous I2C block
    // read of both acceleromter and magnetometer data despite them being non-contguous
    // in memory... funky!
    value = 0x20;
    result = i2c.writeRegister(address, FXOS8700_M_CTRL_REG2, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Configure Open Drain Active LOW interrupt mode.
    // n.b. This may need to be reconfigured if the interrupt line is shared.
    value = 0x01;
    result = i2c.writeRegister(address, FXOS8700_CTRL_REG3, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Enable a data ready interrupt.
    // TODO: This is currently PUSHPULL mode. This may nede to be reconfigured
    // to OPEN_DRAIN if the interrupt line is shared.
    value = 0x01;
    result = i2c.writeRegister(address, FXOS8700_CTRL_REG4, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Route the data ready interrupt to INT1 pin.
    value = 0x01;
    result = i2c.writeRegister(address, FXOS8700_CTRL_REG5, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Configure acceleromter g range.
    value = accelerometerRange.get(AKHILAFLEXAccelerometer::sampleRange);
    result = i2c.writeRegister(address, FXOS8700_XYZ_DATA_CFG, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    // Configure sample rate and re-enable the sensor.
    value = accelerometerPeriod.get(AKHILAFLEXAccelerometer::samplePeriod * 1000) | 0x01;
    result = i2c.writeRegister(address, FXOS8700_CTRL_REG1, value);
    if (result != 0)
        return AKHILAFLEX_I2C_ERROR;

    return AKHILAFLEX_OK;
}

/**
  * Constructor.
  * Create a software abstraction of an FXSO8700 combined accelerometer/magnetometer
  *
  * @param _i2c an instance of I2C used to communicate with the device.
  *
  * @param address the default I2C address of the accelerometer. Defaults to: FXS8700_DEFAULT_ADDR.
  *
 */
FXOS8700::FXOS8700(AKHILAFLEXI2C &_i2c, AKHILAFLEXPin _int1, CoordinateSpace &coordinateSpace, uint16_t address, uint16_t aid, uint16_t cid) : 
    AKHILAFLEXAccelerometer(coordinateSpace, aid),
    AKHILAFLEXCompass(coordinateSpace, cid),
    i2c(_i2c), int1(_int1)
{
    // Store our identifiers.
    this->address = address;

    // Enable pullup on the interrupt line
    int1.setPull(PullUp);

    // Configure and enable the accelerometer.
    configure();
}

/**
 * Attempts to read the 8 bit WHO_AM_I value from the accelerometer
 *
 * @return true if the WHO_AM_I value is succesfully read. false otherwise.
 */
int FXOS8700::isDetected(AKHILAFLEXI2C &i2c, uint16_t address)
{
    return i2c.readRegister(address, FXOS8700_WHO_AM_I) == FXOS8700_WHOAMI_VAL;
}

/**
 * Poll to see if new data is available from the hardware. If so, update it.
 * n.b. it is not necessary to explicitly call this funciton to update data
 * (it normally happens in the background when the scheduler is idle), but a check is performed
 * if the user explicitly requests up to date data.
 *
 * @return AKHILAFLEX_OK on success, AKHILAFLEX_I2C_ERROR if the update fails.
 *
 * @note This method should be overidden by the hardware driver to implement the requested
 * changes in hardware.
 */
int FXOS8700::requestUpdate()
{
    // Ensure we're scheduled to update the data periodically
    if(!(AKHILAFLEXAccelerometer::status & AKHILAFLEX_ACCEL_ADDED_TO_IDLE))
    {
        fiber_add_idle_component((AKHILAFLEXAccelerometer *)this);
        AKHILAFLEXAccelerometer::status |= AKHILAFLEX_ACCEL_ADDED_TO_IDLE;
    }

    // Poll interrupt line from device (ACTIVE LOW)
    if(int1.getDigitalValue() == 0)
    {
        uint8_t data[12];
        int16_t s;
        uint8_t *lsb = (uint8_t *) &s;
        uint8_t *msb = lsb + 1;
        Sample3D accelerometerSample;
        Sample3D compassSample;
        int result;

        // Read the combined accelerometer and magnetometer data.
        result = i2c.readRegister(address, FXOS8700_OUT_X_MSB, data, 12);

        if (result !=0)
            return AKHILAFLEX_I2C_ERROR;

        
        // read sensor data (and translate into signed little endian)
        *msb = data[0];
        *lsb = data[1];
        accelerometerSample.x = s;

        *msb = data[2];
        *lsb = data[3];
        accelerometerSample.y = s;

        *msb = data[4];
        *lsb = data[5];
        accelerometerSample.z = s;

        *msb = data[6];
        *lsb = data[7];
        compassSample.x = s;

        *msb = data[8];
        *lsb = data[9];
        compassSample.y = s;

        *msb = data[10];
        *lsb = data[11];
        compassSample.z = s;

        // scale the 14 bit accelerometer data (packed into 16 bits) into SI units (milli-g), and translate to ENU coordinate system
        AKHILAFLEXAccelerometer::sampleENU.x = (-accelerometerSample.y * AKHILAFLEXAccelerometer::sampleRange) / 32;
        AKHILAFLEXAccelerometer::sampleENU.y = (accelerometerSample.x * AKHILAFLEXAccelerometer::sampleRange) / 32;
        AKHILAFLEXAccelerometer::sampleENU.z = (accelerometerSample.z * AKHILAFLEXAccelerometer::sampleRange) / 32;

        // translate magnetometer data into ENU coordinate system and normalise into nano-teslas
        AKHILAFLEXCompass::sampleENU.x = FXOS8700_NORMALIZE_SAMPLE(-compassSample.y);
        AKHILAFLEXCompass::sampleENU.y = FXOS8700_NORMALIZE_SAMPLE(compassSample.x);
        AKHILAFLEXCompass::sampleENU.z = FXOS8700_NORMALIZE_SAMPLE(compassSample.z);

        AKHILAFLEXAccelerometer::update();
        AKHILAFLEXCompass::update();
    }

    return AKHILAFLEX_OK;
}


/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  *
  * Internally calls updateSample().
  */
void FXOS8700::idleTick()
{
    requestUpdate();
}

/**
  * Destructor for FXS8700, where we deregister from the array of fiber components.
  */
FXOS8700::~FXOS8700()
{
}

