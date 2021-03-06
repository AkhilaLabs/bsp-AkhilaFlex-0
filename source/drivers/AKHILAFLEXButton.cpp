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
#include "AKHILAFLEXButton.h"
#include "AKHILAFLEXSystemTimer.h"

/**
  * Constructor.
  *
  * Create a software representation of a button.
  *
  * @param name the physical pin on the processor that should be used as input.
  *
  * @param id the ID of the new AKHILAFLEXButton object.
  *
  * @param eventConfiguration Configures the events that will be generated by this AKHILAFLEXButton instance.
  *                           Defaults to AKHILAFLEX_BUTTON_ALL_EVENTS.
  *
  * @param mode the configuration of internal pullups/pulldowns, as defined in the mbed PinMode class. PullNone by default.
  *
  * @code
  * buttonA(AKHILAFLEX_PIN_BUTTON_A, AKHILAFLEX_ID_BUTTON_A);
  * @endcode
  */
AKHILAFLEXButton::AKHILAFLEXButton(PinName name, uint16_t id, AKHILAFLEXButtonEventConfiguration eventConfiguration, PinMode mode) : pin(name, mode)
{
    this->id = id;
    this->name = name;
    this->eventConfiguration = eventConfiguration;
    this->downStartTime = 0;
    this->sigma = 0;
    system_timer_add_component(this);
}

/**
  * Changes the event configuration used by this button to the given AKHILAFLEXButtonEventConfiguration.
  *
  * All subsequent events generated by this button will then be informed by this configuraiton.
  *
  * @param config The new configuration for this button. Legal values are AKHILAFLEX_BUTTON_ALL_EVENTS or AKHILAFLEX_BUTTON_SIMPLE_EVENTS.
  *
  * Example:
  * @code
  * // Configure a button to generate all possible events.
  * buttonA.setEventConfiguration(AKHILAFLEX_BUTTON_ALL_EVENTS);
  *
  * // Configure a button to suppress AKHILAFLEX_BUTTON_EVT_CLICK and AKHILAFLEX_BUTTON_EVT_LONG_CLICK events.
  * buttonA.setEventConfiguration(AKHILAFLEX_BUTTON_SIMPLE_EVENTS);
  * @endcode
  */
void AKHILAFLEXButton::setEventConfiguration(AKHILAFLEXButtonEventConfiguration config)
{
    this->eventConfiguration = config;
}

/**
  * periodic callback from AKHILAFLEX system timer.
  *
  * Check for state change for this button, and fires various events on a state change.
  */
void AKHILAFLEXButton::systemTick()
{
    //
    // If the pin is pulled low (touched), increment our culumative counter.
    // otherwise, decrement it. We're essentially building a lazy follower here.
    // This makes the output debounced for buttons, and desensitizes touch sensors
    // (particularly in environments where there is mains noise!)
    //
    if(!pin)
    {
        if (sigma < AKHILAFLEX_BUTTON_SIGMA_MAX)
            sigma++;
    }
    else
    {
        if (sigma > AKHILAFLEX_BUTTON_SIGMA_MIN)
            sigma--;
    }

    // Check to see if we have off->on state change.
    if(sigma > AKHILAFLEX_BUTTON_SIGMA_THRESH_HI && !(status & AKHILAFLEX_BUTTON_STATE))
    {
        // Record we have a state change, and raise an event.
        status |= AKHILAFLEX_BUTTON_STATE;
        AKHILAFLEXEvent evt(id,AKHILAFLEX_BUTTON_EVT_DOWN);

        //Record the time the button was pressed.
        downStartTime = system_timer_current_time();
    }

    // Check to see if we have on->off state change.
    if(sigma < AKHILAFLEX_BUTTON_SIGMA_THRESH_LO && (status & AKHILAFLEX_BUTTON_STATE))
    {
        status = 0;
        AKHILAFLEXEvent evt(id,AKHILAFLEX_BUTTON_EVT_UP);

       if (eventConfiguration == AKHILAFLEX_BUTTON_ALL_EVENTS)
       {
           //determine if this is a long click or a normal click and send event
           if((system_timer_current_time() - downStartTime) >= AKHILAFLEX_BUTTON_LONG_CLICK_TIME)
               AKHILAFLEXEvent evt(id,AKHILAFLEX_BUTTON_EVT_LONG_CLICK);
           else
               AKHILAFLEXEvent evt(id,AKHILAFLEX_BUTTON_EVT_CLICK);
       }
    }

    //if button is pressed and the hold triggered event state is not triggered AND we are greater than the button debounce value
    if((status & AKHILAFLEX_BUTTON_STATE) && !(status & AKHILAFLEX_BUTTON_STATE_HOLD_TRIGGERED) && (system_timer_current_time() - downStartTime) >= AKHILAFLEX_BUTTON_HOLD_TIME)
    {
        //set the hold triggered event flag
        status |= AKHILAFLEX_BUTTON_STATE_HOLD_TRIGGERED;

        //fire hold event
        AKHILAFLEXEvent evt(id,AKHILAFLEX_BUTTON_EVT_HOLD);
    }
}

/**
  * Tests if this Button is currently pressed.
  *
  * @code
  * if(buttonA.isPressed())
  *     display.scroll("Pressed!");
  * @endcode
  *
  * @return 1 if this button is pressed, 0 otherwise.
  */
int AKHILAFLEXButton::isPressed()
{
    return status & AKHILAFLEX_BUTTON_STATE ? 1 : 0;
}

/**
  * Destructor for AKHILAFLEXButton, where we deregister this instance from the array of fiber components.
  */
AKHILAFLEXButton::~AKHILAFLEXButton()
{
    system_timer_remove_component(this);
}
