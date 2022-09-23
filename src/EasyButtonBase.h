/**
 * EasyButtonBase.h
 * @author Evert Arias
 * @version 2.0.0
 * @license MIT
 */

#ifndef _EasyButtonBase_h
#define _EasyButtonBase_h

#include <Arduino.h>
#include "Sequence.h"
#include "Defines.h"

#if defined(ESP8266) || defined(ESP32)
#define EASYBUTTON_FUNCTIONAL_SUPPORT 1
#endif

#ifdef EASYBUTTON_FUNCTIONAL_SUPPORT
#include <functional>
#include "FunctionalInterrupt.h"
#endif

#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
#define MAX_SEQUENCES 5
#endif

class EasyButtonBase
{
public:
// Common functions.
// Pointer to event handling methods
#ifndef EASYBUTTON_ALLOW_INTERRUPTS
	#ifdef EASYBUTTON_FUNCTIONAL_SUPPORT
	    typedef std::function<void(EasyButtonBase&)> callback_t;
	#else
	    typedef void (*callback_t)(EasyButtonBase&);
	#endif
#else
	#ifdef EASYBUTTON_FUNCTIONAL_SUPPORT
	    typedef std::function<void()> callback_t;
	#else
	    typedef void (*callback_t)();
	#endif
#endif

    EasyButtonBase(bool active_low) : _active_low(active_low)
    {
    }
    virtual void begin() = 0;                                                   // Initialize a button object and the pin it's connected to.
    virtual bool read() = 0;                                                    // Returns the current debounced button state, true for pressed, false for released.
    void onPressed(callback_t callback);                                        // Call a callback function when the button has been pressed and released.
    void onPressedFor(uint32_t duration, callback_t callback);                  // Call a callback function when the button has been held for at least the given number of milliseconds.
    void onPressedContinues(uint32_t period, callback_t callback);           // Call a callback function when the button continues to be pressed
#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
    void onSequence(uint8_t sequences, uint32_t duration, callback_t callback); // Call a callback function when the given sequence has matched.
#endif

    bool isPressed();                    // Returns true if the button state was pressed at the last read.
    bool isReleased();                   // Returns true if the button state was released at the last read.
    bool wasPressed();                   // Returns true if the button state at the last read was pressed.
    bool wasReleased();                  // Returns true if the button state at the last read was released.
    bool pressedFor(uint32_t duration);  // Returns true if the button state at the last read was pressed, and has been in that state for at least the given number of milliseconds.
    bool releasedFor(uint32_t duration); // Returns true if the button state at the last read was released, and has been in that state for at least the given number of milliseconds.
    byte id;                             // useful field for use in an array of buttons
protected:
    // Common variables.
#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
    Sequence _sequences[MAX_SEQUENCES];
    uint16_t _sequences_count;
#endif
    uint32_t _held_threshold; // Held threshold.
    uint32_t _held_continues_period; // Held Continues period.

#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
    callback_t _pressed_sequence_callbacks[MAX_SEQUENCES];
#endif
    callback_t _pressed_callback;     // Callback function for pressed events.
    callback_t _pressed_for_callback; // Callback function for pressedFor events.
    callback_t _pressed_continues_callback; // Callback function for pressedContinues events.

    bool _held_callback_called;       // Indicate if button long press has been notified.
    bool _active_low;                 // Inverts button logic. If true, low = pressed else high = pressed.
    bool _current_state;              // Current button state, true = pressed.
    bool _last_state;                 // Previous button state, true = pressed.
    bool _changed;                    // Has the state change since last read.
    uint32_t _time;                   // Time of current state.
    uint32_t _last_change;            // Time of last state change.
    uint32_t _last_continues;         // Time the last continues was fired
    bool _was_btn_held;               // Indicate if button was held.

    // Common functions
    void _checkPressedTime(); // Verify if pressed_for_callback should be called.
};

#endif
