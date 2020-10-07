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
  * Class definition for AKHILAFLEXMultiButton.
  *
  * Represents a virtual button, capable of reacting to simultaneous presses of two
  * other buttons.
  */
#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXMultiButton.h"

/**
  * Constructor.
  *
  * Create a representation of a virtual button, that generates events based upon the combination
  * of two given buttons.
  *
  * @param button1 the unique ID of the first button to watch.
  *
  * @param button2 the unique ID of the second button to watch.
  *
  * @param id the unique EventModel id of this AKHILAFLEXMultiButton instance.
  *
  * @code
  * multiButton(AKHILAFLEX_ID_BUTTON_A, AKHILAFLEX_ID_BUTTON_B, AKHILAFLEX_ID_BUTTON_AB);
  * @endcode
  */
AKHILAFLEXMultiButton::AKHILAFLEXMultiButton(uint16_t button1, uint16_t button2, uint16_t id)
{
    this->id = id;
    this->button1 = button1;
    this->button2 = button2;
    this->eventConfiguration = AKHILAFLEX_BUTTON_SIMPLE_EVENTS;

    if (EventModel::defaultEventBus)
    {
        EventModel::defaultEventBus->listen(button1, AKHILAFLEX_EVT_ANY, this, &AKHILAFLEXMultiButton::onButtonEvent,  MESSAGE_BUS_LISTENER_IMMEDIATE);
        EventModel::defaultEventBus->listen(button2, AKHILAFLEX_EVT_ANY, this, &AKHILAFLEXMultiButton::onButtonEvent,  MESSAGE_BUS_LISTENER_IMMEDIATE);
    }
}

/**
  * Retrieves the button id for the alternate button id given.
  *
  * @param b the id of the button whose state we would like to retrieve.
  *
  * @return the other sub button id.
  */
uint16_t AKHILAFLEXMultiButton::otherSubButton(uint16_t b)
{
    return (b == button1 ? button2 : button1);
}

/**
  * Determines if the given button id is marked as pressed.
  *
  * @param button the id of the button whose state we would like to retrieve.
  *
  * @return 1 if pressed, 0 if not.
  */
int AKHILAFLEXMultiButton::isSubButtonPressed(uint16_t button)
{
    if (button == button1)
        return status & AKHILAFLEX_MULTI_BUTTON_STATE_1;

    if (button == button2)
        return status & AKHILAFLEX_MULTI_BUTTON_STATE_2;

    return 0;
}

/**
  * Determines if the given button id is marked as held.
  *
  * @param button the id of the button whose state we would like to retrieve.
  *
  * @return 1 if held, 0 if not.
  */
int AKHILAFLEXMultiButton::isSubButtonHeld(uint16_t button)
{
    if (button == button1)
        return status & AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_1;

    if (button == button2)
        return status & AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_2;

    return 0;
}

/**
  * Determines if the given button id is marked as supressed.
  *
  * @param button the id of the button whose state we would like to retrieve.
  *
  * @return 1 if supressed, 0 if not.
  */
int AKHILAFLEXMultiButton::isSubButtonSupressed(uint16_t button)
{
    if (button == button1)
        return status & AKHILAFLEX_MULTI_BUTTON_SUPRESSED_1;

    if (button == button2)
        return status & AKHILAFLEX_MULTI_BUTTON_SUPRESSED_2;

    return 0;
}

/**
  * Configures the button pressed state for the given button id.
  *
  * @param button the id of the button whose state requires updating.
  *
  * @param value the value to set for this buttons state. (Transformed into a logical 0 or 1).
  */
void AKHILAFLEXMultiButton::setButtonState(uint16_t button, int value)
{
    if (button == button1)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_STATE_1;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_STATE_1;
    }

    if (button == button2)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_STATE_2;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_STATE_2;
    }
}

/**
  * Configures the button held state for the given button id.
  *
  * @param button the id of the button whose state requires updating.
  *
  * @param value the value to set for this buttons state. (Transformed into a logical 0 or 1).
  */
void AKHILAFLEXMultiButton::setHoldState(uint16_t button, int value)
{
    if (button == button1)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_1;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_1;
    }

    if (button == button2)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_2;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_HOLD_TRIGGERED_2;
    }
}

/**
  * Configures the button suppressed state for the given button id.
  *
  * @param button the id of the button whose state requires updating.
  *
  * @param value the value to set for this buttons state. (Transformed into a logical 0 or 1).
  */
void AKHILAFLEXMultiButton::setSupressedState(uint16_t button, int value)
{
    if (button == button1)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_SUPRESSED_1;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_SUPRESSED_1;
    }

    if (button == button2)
    {
        if (value)
            status |= AKHILAFLEX_MULTI_BUTTON_SUPRESSED_2;
        else
            status &= ~AKHILAFLEX_MULTI_BUTTON_SUPRESSED_2;
    }
}

/**
  * Changes the event configuration of this button to the given AKHILAFLEXButtonEventConfiguration.
  * All subsequent events generated by this button will then be informed by this configuration.
  *
  * @param config The new configuration for this button. Legal values are AKHILAFLEX_BUTTON_ALL_EVENTS or AKHILAFLEX_BUTTON_SIMPLE_EVENTS.
  *
  * @code
  * // Configure a button to generate all possible events.
  * buttonAB.setEventConfiguration(AKHILAFLEX_BUTTON_ALL_EVENTS);
  *
  * // Configure a button to suppress AKHILAFLEX_BUTTON_EVT_CLICK and AKHILAFLEX_BUTTON_EVT_LONG_CLICK events.
  * buttonAB.setEventConfiguration(AKHILAFLEX_BUTTON_SIMPLE_EVENTS);
  * @endcode
  */
void AKHILAFLEXMultiButton::setEventConfiguration(AKHILAFLEXButtonEventConfiguration config)
{
    this->eventConfiguration = config;
}

/**
  * A member function that is invoked when any event is detected from the two
  * button IDs this AKHILAFLEXMultiButton instance was constructed with.
  *
  * @param evt the event received from the default EventModel.
  */
void AKHILAFLEXMultiButton::onButtonEvent(AKHILAFLEXEvent evt)
{
    int button = evt.source;
    int otherButton = otherSubButton(button);

    switch(evt.value)
    {
        case AKHILAFLEX_BUTTON_EVT_DOWN:
            setButtonState(button, 1);
            if(isSubButtonPressed(otherButton))
                AKHILAFLEXEvent e(id, AKHILAFLEX_BUTTON_EVT_DOWN);

        break;

        case AKHILAFLEX_BUTTON_EVT_HOLD:
            setHoldState(button, 1);
            if(isSubButtonHeld(otherButton))
                AKHILAFLEXEvent e(id, AKHILAFLEX_BUTTON_EVT_HOLD);

        break;

        case AKHILAFLEX_BUTTON_EVT_UP:
            if(isSubButtonPressed(otherButton))
            {
                AKHILAFLEXEvent e(id, AKHILAFLEX_BUTTON_EVT_UP);

                if (isSubButtonHeld(button) && isSubButtonHeld(otherButton))
                    AKHILAFLEXEvent e(id, AKHILAFLEX_BUTTON_EVT_LONG_CLICK);
                else
                    AKHILAFLEXEvent e(id, AKHILAFLEX_BUTTON_EVT_CLICK);

                setSupressedState(otherButton, 1);
            }
            else if (!isSubButtonSupressed(button) && eventConfiguration == AKHILAFLEX_BUTTON_ALL_EVENTS)
            {
                if (isSubButtonHeld(button))
                    AKHILAFLEXEvent e(button, AKHILAFLEX_BUTTON_EVT_LONG_CLICK);
                else
                    AKHILAFLEXEvent e(button, AKHILAFLEX_BUTTON_EVT_CLICK);
            }

            setButtonState(button, 0);
            setHoldState(button, 0);
            setSupressedState(button, 0);

        break;

    }
}


/**
  * Tests if this AKHILAFLEXMultiButton instance is virtually pressed.
  *
  * @return 1 if both physical buttons are pressed simultaneously.
  *
  * @code
  * if(buttonAB.isPressed())
  *     display.scroll("Pressed!");
  * @endcode
  */
int AKHILAFLEXMultiButton::isPressed()
{
    return ((status & AKHILAFLEX_MULTI_BUTTON_STATE_1) && (status & AKHILAFLEX_MULTI_BUTTON_STATE_2));
}
