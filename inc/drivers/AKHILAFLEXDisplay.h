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

#ifndef AKHILAFLEX_DISPLAY_H
#define AKHILAFLEX_DISPLAY_H

#include "mbed.h"
#include "AKHILAFLEXConfig.h"
#include "ManagedString.h"
#include "AKHILAFLEXComponent.h"
#include "AKHILAFLEXImage.h"
#include "AKHILAFLEXFont.h"
#include "AKHILAFLEXMatrixMaps.h"
#include "AKHILAFLEXLightSensor.h"

/**
  * Event codes raised by AKHILAFLEXDisplay
  */
#define AKHILAFLEX_DISPLAY_EVT_ANIMATION_COMPLETE         1
#define AKHILAFLEX_DISPLAY_EVT_LIGHT_SENSE                2

//
// Internal constants
//
#define AKHILAFLEX_DISPLAY_DEFAULT_AUTOCLEAR      1
#define AKHILAFLEX_DISPLAY_SPACING                1
#define AKHILAFLEX_DISPLAY_GREYSCALE_BIT_DEPTH    8
#define AKHILAFLEX_DISPLAY_ANIMATE_DEFAULT_POS    -255

enum AnimationMode {
    ANIMATION_MODE_NONE,
    ANIMATION_MODE_STOPPED,
    ANIMATION_MODE_SCROLL_TEXT,
    ANIMATION_MODE_PRINT_TEXT,
    ANIMATION_MODE_SCROLL_IMAGE,
    ANIMATION_MODE_ANIMATE_IMAGE,
    ANIMATION_MODE_ANIMATE_IMAGE_WITH_CLEAR,
    ANIMATION_MODE_PRINT_CHARACTER
};

enum DisplayMode {
    DISPLAY_MODE_BLACK_AND_WHITE,
    DISPLAY_MODE_GREYSCALE,
    DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE
};

enum DisplayRotation {
    AKHILAFLEX_DISPLAY_ROTATION_0,
    AKHILAFLEX_DISPLAY_ROTATION_90,
    AKHILAFLEX_DISPLAY_ROTATION_180,
    AKHILAFLEX_DISPLAY_ROTATION_270
};

/**
  * Class definition for AKHILAFLEXDisplay.
  *
  * A AKHILAFLEXDisplay represents the LED matrix array on the AKHILAFLEX.
  */
class AKHILAFLEXDisplay : public AKHILAFLEXComponent
{
    uint8_t width;
    uint8_t height;
    uint8_t brightness;
    uint8_t strobeRow;
    uint8_t rotation;
    uint8_t mode;
    uint8_t greyscaleBitMsk;
    uint8_t timingCount;
    uint32_t col_mask;

    Timeout renderTimer;
    PortOut *LEDMatrix;

    //
    // State used by all animation routines.
    //

    // The animation mode that's currently running (if any)
    volatile AnimationMode animationMode;

    // The time in milliseconds between each frame update.
    uint16_t animationDelay;

    // The time in milliseconds since the frame update.
    uint16_t animationTick;

    // Stop playback of any animations
    void stopAnimation(int delay);

    //
    // State for scrollString() method.
    // This is a surprisingly intricate method.
    //
    // The text being displayed.
    ManagedString scrollingText;

    // The index of the character currently being displayed.
    uint16_t scrollingChar;

    // The number of pixels the current character has been shifted on the display.
    uint8_t scrollingPosition;

    //
    // State for printString() method.
    //
    // The text being displayed. NULL if no message is scheduled for playback.
    // We *could* get some reuse in here with the scroll* variables above,
    // but best to keep it clean in case kids try concurrent operation (they will!),
    // given the small RAM overhead needed to maintain orthogonality.
    ManagedString printingText;

    // The index of the character currently being displayed.
    uint16_t printingChar;

    //
    // State for scrollImage() method.
    //
    // The image being displayed.
    AKHILAFLEXImage scrollingImage;

    // The number of pixels the image has been shifted on the display.
    int16_t scrollingImagePosition;

    // The number of pixels the image is shifted on the display in each quantum.
    int8_t scrollingImageStride;

    // A pointer to an instance of light sensor, if in use
    AKHILAFLEXLightSensor* lightSensor;

    // Flag to indicate if image has been rendered to screen yet (or not)
    bool scrollingImageRendered;

    const MatrixMap &matrixMap;

    // Internal methods to handle animation.

    /**
      *  Periodic callback, that we use to perform any animations we have running.
      */
    void animationUpdate();

    /**
      *  Called by the display in an interval determined by the brightness of the display, to give an impression
      *  of brightness.
      */
    void renderFinish();

    /**
      * Translates a bit mask to a bit mask suitable for the nrf PORT0 and PORT1.
      * Brightness has two levels on, or off.
      */
    void render();

    /**
      * Renders the current image, and drops the fourth frame to allow for
      * sensors that require the display to operate.
      */
    void renderWithLightSense();

    /**
      * Translates a bit mask into a timer interrupt that gives the appearence of greyscale.
      */
    void renderGreyscale();

    /**
      * Internal scrollText update method.
      * Shift the screen image by one pixel to the left. If necessary, paste in the next char.
      */
    void updateScrollText();

    /**
      * Internal printText update method.
      * Paste the next character in the string.
      */
    void updatePrintText();

    /**
      * Internal scrollImage update method.
      * Paste the stored bitmap at the appropriate point.
      */
    void updateScrollImage();

    /**
      * Internal animateImage update method.
      * Paste the stored bitmap at the appropriate point and stop on the last frame.
      */
    void updateAnimateImage();

    /**
     * Broadcasts an event onto the defult EventModel indicating that the
     * current animation has completed.
     */
    void sendAnimationCompleteEvent();

    /**
      * Blocks the current fiber until the display is available (i.e. does not effect is being displayed).
      * Animations are queued until their time to display.
      */
    void waitForFreeDisplay();

    /**
      * Blocks the current fiber until the current animation has finished.
      * If the scheduler is not running, this call will essentially perform a spinning wait.
      */
    void fiberWait();

    /**
      * Enables or disables the display entirely, and releases the pins for other uses.
      *
      * @param enableDisplay true to enabled the display, or false to disable it.
      */
    void setEnable(bool enableDisplay);

public:
    // The mutable bitmap buffer being rendered to the LED matrix.
    AKHILAFLEXImage image;

    /**
      * Constructor.
      *
      * Create a software representation the AKHILAFLEX's 5x5 LED matrix.
      * The display is initially blank.
      *
      * @param id The id the display should use when sending events on the MessageBus. Defaults to AKHILAFLEX_ID_DISPLAY.
      *
      * @param map The mapping information that relates pin inputs/outputs to physical screen coordinates.
      *            Defaults to AKHILAFLEXMatrixMap, defined in AKHILAFLEXMatrixMaps.h.
      *
      * @code
      * AKHILAFLEXDisplay display;
      * @endcode
      */
    AKHILAFLEXDisplay(uint16_t id = AKHILAFLEX_ID_DISPLAY, const MatrixMap &map = AKHILAFLEXMatrixMap);

    /**
      * Stops any currently running animation, and any that are waiting to be displayed.
      */
    void stopAnimation();

    /**
      * Frame update method, invoked periodically to strobe the display.
      */
    virtual void systemTick();

    /**
      * Prints the given character to the display, if it is not in use.
      *
      * @param c The character to display.
      *
      * @param delay Optional parameter - the time for which to show the character. Zero displays the character forever,
      *              or until the Displays next use.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_BUSY is the screen is in use, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.printAsync('p');
      * display.printAsync('p',100);
      * @endcode
      */
    int printCharAsync(char c, int delay = 0);

    /**
      * Prints the given ManagedString to the display, one character at a time.
      * Returns immediately, and executes the animation asynchronously.
      *
      * @param s The string to display.
      *
      * @param delay The time to delay between characters, in milliseconds. Must be > 0.
      *              Defaults to: AKHILAFLEX_DEFAULT_PRINT_SPEED.
      *
      * @return AKHILAFLEX_OK, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.printAsync("abc123",400);
      * @endcode
      */
    int printAsync(ManagedString s, int delay = AKHILAFLEX_DEFAULT_PRINT_SPEED);

    /**
      * Prints the given image to the display, if the display is not in use.
      * Returns immediately, and executes the animation asynchronously.
      *
      * @param i The image to display.
      *
      * @param x The horizontal position on the screen to display the image. Defaults to 0.
      *
      * @param y The vertical position on the screen to display the image. Defaults to 0.
      *
      * @param alpha Treats the brightness level '0' as transparent. Defaults to 0.
      *
      * @param delay The time to delay between characters, in milliseconds. Defaults to 0.
      *
      * @code
      * AKHILAFLEXImage i("1,1,1,1,1\n1,1,1,1,1\n");
      * display.print(i,400);
      * @endcode
      */
    int printAsync(AKHILAFLEXImage i, int x = 0, int y = 0, int alpha = 0, int delay = 0);

    /**
      * Prints the given character to the display.
      *
      * @param c The character to display.
      *
      * @param delay Optional parameter - the time for which to show the character. Zero displays the character forever,
      *              or until the Displays next use.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_CANCELLED or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.printAsync('p');
      * display.printAsync('p',100);
      * @endcode
      */
    int printChar(char c, int delay = 0);

    /**
      * Prints the given string to the display, one character at a time.
      *
      * Blocks the calling thread until all the text has been displayed.
      *
      * @param s The string to display.
      *
      * @param delay The time to delay between characters, in milliseconds. Defaults
      *              to: AKHILAFLEX_DEFAULT_PRINT_SPEED.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_CANCELLED or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.print("abc123",400);
      * @endcode
      */
    int print(ManagedString s, int delay = AKHILAFLEX_DEFAULT_PRINT_SPEED);

    /**
      * Prints the given image to the display.
      * Blocks the calling thread until all the image has been displayed.
      *
      * @param i The image to display.
      *
      * @param x The horizontal position on the screen to display the image. Defaults to 0.
      *
      * @param y The vertical position on the screen to display the image. Defaults to 0.
      *
      * @param alpha Treats the brightness level '0' as transparent. Defaults to 0.
      *
      * @param delay The time to display the image for, or zero to show the image forever. Defaults to 0.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_BUSY if the display is already in use, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * AKHILAFLEXImage i("1,1,1,1,1\n1,1,1,1,1\n");
      * display.print(i,400);
      * @endcode
      */
    int print(AKHILAFLEXImage i, int x = 0, int y = 0, int alpha = 0, int delay = 0);

    /**
      * Scrolls the given string to the display, from right to left.
      * Returns immediately, and executes the animation asynchronously.
      *
      * @param s The string to display.
      *
      * @param delay The time to delay between characters, in milliseconds. Defaults
      *              to: AKHILAFLEX_DEFAULT_SCROLL_SPEED.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_BUSY if the display is already in use, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.scrollAsync("abc123",100);
      * @endcode
      */
    int scrollAsync(ManagedString s, int delay = AKHILAFLEX_DEFAULT_SCROLL_SPEED);

    /**
      * Scrolls the given image across the display, from right to left.
      * Returns immediately, and executes the animation asynchronously.
      *
      * @param image The image to display.
      *
      * @param delay The time between updates, in milliseconds. Defaults
      *              to: AKHILAFLEX_DEFAULT_SCROLL_SPEED.
      *
      * @param stride The number of pixels to shift by in each update. Defaults to AKHILAFLEX_DEFAULT_SCROLL_STRIDE.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_BUSY if the display is already in use, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * AKHILAFLEXImage i("1,1,1,1,1\n1,1,1,1,1\n");
      * display.scrollAsync(i,100,1);
      * @endcode
      */
    int scrollAsync(AKHILAFLEXImage image, int delay = AKHILAFLEX_DEFAULT_SCROLL_SPEED, int stride = AKHILAFLEX_DEFAULT_SCROLL_STRIDE);

    /**
      * Scrolls the given string across the display, from right to left.
      * Blocks the calling thread until all text has been displayed.
      *
      * @param s The string to display.
      *
      * @param delay The time to delay between characters, in milliseconds. Defaults
      *              to: AKHILAFLEX_DEFAULT_SCROLL_SPEED.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_CANCELLED or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * display.scroll("abc123",100);
      * @endcode
      */
    int scroll(ManagedString s, int delay = AKHILAFLEX_DEFAULT_SCROLL_SPEED);

    /**
      * Scrolls the given image across the display, from right to left.
      * Blocks the calling thread until all the text has been displayed.
      *
      * @param image The image to display.
      *
      * @param delay The time between updates, in milliseconds. Defaults
      *              to: AKHILAFLEX_DEFAULT_SCROLL_SPEED.
      *
      * @param stride The number of pixels to shift by in each update. Defaults to AKHILAFLEX_DEFAULT_SCROLL_STRIDE.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_CANCELLED or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * AKHILAFLEXImage i("1,1,1,1,1\n1,1,1,1,1\n");
      * display.scroll(i,100,1);
      * @endcode
      */
    int scroll(AKHILAFLEXImage image, int delay = AKHILAFLEX_DEFAULT_SCROLL_SPEED, int stride = AKHILAFLEX_DEFAULT_SCROLL_STRIDE);

    /**
      * "Animates" the current image across the display with a given stride, finishing on the last frame of the animation.
      * Returns immediately.
      *
      * @param image The image to display.
      *
      * @param delay The time to delay between each update of the display, in milliseconds.
      *
      * @param stride The number of pixels to shift by in each update.
      *
      * @param startingPosition the starting position on the display for the animation
      *                         to begin at. Defaults to AKHILAFLEX_DISPLAY_ANIMATE_DEFAULT_POS.
      *
      * @param autoClear defines whether or not the display is automatically cleared once the animation is complete. By default, the display is cleared. Set this parameter to zero to disable the autoClear operation.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_BUSY if the screen is in use, or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * const int heart_w = 10;
      * const int heart_h = 5;
      * const uint8_t heart[] = { 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, };
      *
      * AKHILAFLEXImage i(heart_w,heart_h,heart);
      * display.animateAsync(i,100,5);
      * @endcode
      */
    int animateAsync(AKHILAFLEXImage image, int delay, int stride, int startingPosition = AKHILAFLEX_DISPLAY_ANIMATE_DEFAULT_POS, int autoClear = AKHILAFLEX_DISPLAY_DEFAULT_AUTOCLEAR);

    /**
      * "Animates" the current image across the display with a given stride, finishing on the last frame of the animation.
      * Blocks the calling thread until the animation is complete.
      *
      *
      * @param delay The time to delay between each update of the display, in milliseconds.
      *
      * @param stride The number of pixels to shift by in each update.
      *
      * @param startingPosition the starting position on the display for the animation
      *                         to begin at. Defaults to AKHILAFLEX_DISPLAY_ANIMATE_DEFAULT_POS.
      *
      * @param autoClear defines whether or not the display is automatically cleared once the animation is complete. By default, the display is cleared. Set this parameter to zero to disable the autoClear operation.
      *
      * @return AKHILAFLEX_OK, AKHILAFLEX_CANCELLED or AKHILAFLEX_INVALID_PARAMETER.
      *
      * @code
      * const int heart_w = 10;
      * const int heart_h = 5;
      * const uint8_t heart[] = { 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, };
      *
      * AKHILAFLEXImage i(heart_w,heart_h,heart);
      * display.animate(i,100,5);
      * @endcode
      */
    int animate(AKHILAFLEXImage image, int delay, int stride, int startingPosition = AKHILAFLEX_DISPLAY_ANIMATE_DEFAULT_POS, int autoClear = AKHILAFLEX_DISPLAY_DEFAULT_AUTOCLEAR);

    /**
      * Configures the brightness of the display.
      *
      * @param b The brightness to set the brightness to, in the range 0 - 255.
      *
      * @return AKHILAFLEX_OK, or AKHILAFLEX_INVALID_PARAMETER
      *
      * @code
      * display.setBrightness(255); //max brightness
      * @endcode
      */
    int setBrightness(int b);

    /**
      * Configures the mode of the display.
      *
      * @param mode The mode to swap the display into. One of: DISPLAY_MODE_GREYSCALE,
      *             DISPLAY_MODE_BLACK_AND_WHITE, DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE
      *
      * @code
      * display.setDisplayMode(DISPLAY_MODE_GREYSCALE); //per pixel brightness
      * @endcode
      */
    void setDisplayMode(DisplayMode mode);

    /**
      * Retrieves the mode of the display.
      *
      * @return the current mode of the display
      */
    int getDisplayMode();

    /**
      * Fetches the current brightness of this display.
      *
      * @return the brightness of this display, in the range 0..255.
      *
      * @code
      * display.getBrightness(); //the current brightness
      * @endcode
      */
    int getBrightness();

    /**
      * Rotates the display to the given position.
      *
      * Axis aligned values only.
      *
      * @code
      * display.rotateTo(AKHILAFLEX_DISPLAY_ROTATION_180); //rotates 180 degrees from original orientation
      * @endcode
      */
    void rotateTo(DisplayRotation position);

    /**
      * Enables the display, should only be called if the display is disabled.
      *
      * @code
      * display.enable(); //Enables the display mechanics
      * @endcode
      *
      * @note Only enables the display if the display is currently disabled.
      */
    void enable();

    /**
      * Disables the display, which releases control of the GPIO pins used by the display,
      * which are exposed on the edge connector.
      *
      * @code
      * display.disable(); //disables the display
      * @endcode
      *
      * @note Only disables the display if the display is currently enabled.
      */
    void disable();

    /**
      * Clears the display of any remaining pixels.
      *
      * `display.image.clear()` can also be used!
      *
      * @code
      * display.clear(); //clears the display
      * @endcode
      */
    void clear();

    /**
      * Updates the font that will be used for display operations.
	  *
      * @param font the new font that will be used to render characters.
      *
      * @note DEPRECATED! Please use AKHILAFLEXFont::setSystemFont() instead.
      */
    void setFont(AKHILAFLEXFont font);

    /**
      * Retrieves the font object used for rendering characters on the display.
	  *
      * @note DEPRECATED! Please use AKHILAFLEXFont::getSystemFont() instead.
      */
    AKHILAFLEXFont getFont();

    /**
      * Captures the bitmap currently being rendered on the display.
      *
      * @return a AKHILAFLEXImage containing the captured data.
      */
    AKHILAFLEXImage screenShot();

    /**
      * Gives a representative figure of the light level in the current environment
      * where are AKHILAFLEX is situated.
      *
      * Internally, it constructs an instance of a AKHILAFLEXLightSensor if not already configured
      * and sets the display mode to DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE.
      *
      * This also changes the tickPeriod to AKHILAFLEX_LIGHT_SENSOR_TICK_SPEED so
      * that the display does not suffer from artifacts.
      *
      * @return an indicative light level in the range 0 - 255.
      *
      * @note this will return 0 on the first call to this method, a light reading
      * will be available after the display has activated the light sensor for the
      * first time.
      */
    int readLightLevel();

    /**
      * Destructor for AKHILAFLEXDisplay, where we deregister this instance from the array of system components.
      */
    ~AKHILAFLEXDisplay();
};

#endif
