/**
 * EasyButton.cpp
 * @author Evert Arias
 * @version 2.0.0
 * @license MIT
 */

#include "EasyButton.h"

void EasyButton::begin()
{
	pinMode(_pin, _pu_enabled ? INPUT_PULLUP : INPUT);
	_current_state = _readPin();
	if (_active_low)
	{
		_current_state = !_current_state;
	}
	_time = millis();
	_last_state = _current_state;
	_changed = false;
	_last_change = _time;
}

bool EasyButton::read()
{
	uint32_t read_started_ms = millis();

	bool pinVal = _readPin();

	if (_active_low)
	{
		pinVal = !pinVal;
	}

	if (read_started_ms - _last_change < _db_time)
	{
		// Debounce time has not ellapsed.
		_changed = false;
	}
	else
	{
		// Debounce time ellapsed.
		_last_state = _current_state;				// Save last state.
		_current_state = pinVal;					// Assign new state as current state from pin's value.
		_changed = (_current_state != _last_state); // Report state change if current state vary from last state.
		// If state has changed since last read.
		if (_changed)
		{
			// State changed.
			// Save current millis as last change time.
			_last_change = read_started_ms;
		}
	}

	if (wasPressed())
	{
        if (!_was_btn_down)
		{
			if (_button_down_callback)
			{
	#ifndef EASYBUTTON_ALLOW_INTERRUPTS
				_button_down_callback(*this);
	#else
				_button_down_callback();
	#endif
			}
			_was_btn_down = true;
		}

	}

// check to see if we are coming out of a multi click event
	if (wasReleased())
	{
		if (!_was_btn_held)
		{

		// run the sequence callbacks first - the order they are added does matter
			boolean handled = false;

#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
			for (size_t i = 0; i < MAX_SEQUENCES; i++)
			{
				if (_sequences[i].newPress(read_started_ms))
				{
					callback_handled_t function = _pressed_sequence_callbacks[i];
#ifndef EASYBUTTON_ALLOW_INTERRUPTS
					function(*this, handled);
#else
					function();
#endif
				}
				if (handled) {
					break;
				}
			}
#endif

			if (_click_callback)
			{
#ifndef EASYBUTTON_ALLOW_INTERRUPTS
				_click_callback(*this);
#else
				_click_callback();
#endif
			}

		}
		// Button was not held.
		else
		{
			_was_btn_held = false;
		}

		// Since button released, reset _pressed_for_callbackCalled value.
		_held_callback_called = false;
		_was_btn_down = false;
	}
	else if (isPressed() && _read_type == EASYBUTTON_READ_TYPE_POLL)
	{
		_checkPressedTime();
	}

	_time = read_started_ms;

	return _current_state;
}

bool EasyButton::_readPin()
{
	return digitalRead(_pin);
}

bool EasyButton::supportsInterrupt()
{
#ifdef EASYBUTTON_ALLOW_INTERRUPTS
	return (digitalPinToInterrupt(_pin) != NOT_AN_INTERRUPT);
#else
	return false;
#endif
}

void EasyButton::enableInterrupt(EasyButton::callback_t callback)
{
#ifdef EASYBUTTON_ALLOW_INTERRUPTS
	attachInterrupt(digitalPinToInterrupt(_pin), callback, CHANGE);
	_read_type = EASYBUTTON_READ_TYPE_INTERRUPT;
#endif
}

void EasyButton::disableInterrupt()
{
	detachInterrupt(digitalPinToInterrupt(_pin));
	_read_type = EASYBUTTON_READ_TYPE_POLL;
}

void EasyButton::update()
{
	if (!_was_btn_held)
	{
		_checkPressedTime();
	}
}
