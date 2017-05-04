﻿#include "stdafx.h"
#include "input.h"
#include "gmengine/controllers/gamemachine.h"
#ifdef _WINDOWS

XInputWrapper::XInputWrapper()
	: m_xinputGetState(nullptr)
	, m_xinputSetState(nullptr)
	, m_module(0)
{
#ifdef _WINDOWS
	m_module = LoadLibrary("xinput1_4.dll");
	if (!m_module)
	{
		gm_warning("cannot find xinput dll, xinput disabled.");
		FreeLibrary(m_module);
		return;
	}

	m_xinputGetState = (XInputGetState_Delegate)GetProcAddress(m_module, "XInputGetState");
	m_xinputSetState = (XInputSetState_Delegate)GetProcAddress(m_module, "XInputSetState");
#endif
}

DWORD XInputWrapper::XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if (m_xinputGetState)
		return m_xinputGetState(dwUserIndex, pState);
	else
		gm_warning("cannot invoke XInputGetState");
	return ERROR_DLL_INIT_FAILED;
}

DWORD XInputWrapper::XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	if (m_xinputSetState)
		return m_xinputSetState(dwUserIndex, pVibration);
	else
		gm_warning("cannot invoke XInputGetState");
	return ERROR_DLL_INIT_FAILED;
}

XInputWrapper::~XInputWrapper()
{
	FreeLibrary(m_module);
}

Input_Windows::Input_Windows()
	: m_mouseReady(false)
{
}

Input_Windows::~Input_Windows()
{
}

void Input_Windows::initMouse(IWindow* window)
{
	m_window = window;
	GMRect rect = m_window->getWindowRect();
	::SetCursorPos(rect.x + rect.width / 2, rect.y + rect.height / 2);
	::ShowCursor(false);
	m_mouseReady = true;
}

JoystickState Input_Windows::getJoystickState()
{
	XINPUT_STATE state;
	JoystickState result = { false };

	if (m_xinput.XInputGetState(0, &state) == ERROR_SUCCESS)
	{
		result.valid = true;
		result.leftTrigger = state.Gamepad.bLeftTrigger;
		result.rightTrigger = state.Gamepad.bRightTrigger;
		result.thumbLX = state.Gamepad.sThumbLX;
		result.thumbLY = state.Gamepad.sThumbLY;
		result.thumbRX = state.Gamepad.sThumbRX;
		result.thumbRY = state.Gamepad.sThumbRY;
		result.buttons = state.Gamepad.wButtons;
	}

	return result;
}

void Input_Windows::joystickVibrate(WORD leftMotorSpeed, WORD rightMotorSpeed)
{
	XINPUT_VIBRATION v = { leftMotorSpeed, rightMotorSpeed };
	m_xinput.XInputSetState(0, &v);
}

KeyboardState Input_Windows::getKeyboardState()
{
	KeyboardState state;
	GetKeyboardState(state.keystate);
	return std::move(state);
}

MouseState Input_Windows::getMouseState()
{
	if (!m_mouseReady)
	{
		gm_error("Mouse is not ready. Please call initMouse() first.");
		return MouseState();
	}

	MouseState state;
	GMRect rect = m_window->getWindowRect();
	const GMint centerX = rect.x + rect.width / 2;
	const GMint centerY = rect.y + rect.height / 2;

	POINT pos;
	::GetCursorPos(&pos);
	::SetCursorPos(centerX, centerY);
	state.deltaX = pos.x - centerX;
	state.deltaY = pos.y - centerY;
	return state;
}

#endif