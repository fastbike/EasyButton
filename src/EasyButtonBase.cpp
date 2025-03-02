/**
 * EasyButtonBase.h
 * @author Evert Arias
 * @version 2.0.0
 * @license MIT
 */

#include "EasyButtonBase.h"
#include "Defines.h"

void EasyButtonBase::onButtonDown(EasyButtonBase::callback_t callback)
{
	_button_down_callback = callback;
}

void EasyButtonBase::onButtonClick(EasyButtonBase::callback_t callback)
{
	_click_callback = callback;
}

void EasyButtonBase::onPressedFor(uint32_t duration, EasyButtonBase::callback_t callback)
{
	_held_threshold = duration;
	_pressed_for_callback = callback;
}

void EasyButtonBase::onPressedContinues(uint32_t period, EasyButtonBase::callback_t callback)
{
	_held_continues_period = period;
	_pressed_continues_callback = callback;
}

#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
void EasyButtonBase::onSequence(uint8_t sequences, uint32_t duration, EasyButtonBase::callback_handled_t callback)
{
	if (_sequences_count < 5)
	{
		Sequence sequence(sequences, duration);
		sequence.enable();
		_sequences[_sequences_count] = sequence;
		_pressed_sequence_callbacks[_sequences_count++] = callback;
	}
}
#endif

bool EasyButtonBase::isPressed()
{
	return _current_state;
}

bool EasyButtonBase::isReleased()
{
	return !_current_state;
}

bool EasyButtonBase::wasPressed()
{
	return _current_state && _changed;
}

bool EasyButtonBase::wasReleased()
{
	return !_current_state && _changed;
}

bool EasyButtonBase::pressedFor(uint32_t duration)
{
	return _current_state && _time - _last_change >= duration;
}

bool EasyButtonBase::releasedFor(uint32_t duration)
{
	return !_current_state && _time - _last_change >= duration;
}

void EasyButtonBase::_checkPressedTime()
{
	uint32_t read_started_ms = millis();
	if (_current_state && read_started_ms - _last_change >= _held_threshold && _pressed_for_callback)
	{
		// Button has been pressed for at least the given time.
		_was_btn_held = true;

#ifndef EASYBUTTON_DO_NOT_USE_SEQUENCES
		// Reset short presses counters.
		for (Sequence seq : _sequences)
		{
			seq.reset();
		}
#endif

		// Call the callback function for a long press event if it exist and if it has not been called yet.
		if (_pressed_for_callback && !_held_callback_called)
		{
			// Set as called.
			_held_callback_called = true;
#ifndef EASYBUTTON_ALLOW_INTERRUPTS
			_pressed_for_callback(*this);
#else
			_pressed_for_callback();
#endif
			_last_continues = read_started_ms;
		}
	}

	if (_current_state && (read_started_ms - _last_continues) >= _held_continues_period && _pressed_continues_callback)
	{
		// Call the callback function for a long press continues event if it exist and the pressedFor has been called
		if (_pressed_continues_callback && _held_callback_called)
		{
#ifndef EASYBUTTON_ALLOW_INTERRUPTS
			_pressed_continues_callback(*this);
#else
			_pressed_continues_callback();
#endif
			_last_continues = read_started_ms;
		}

	}

}
