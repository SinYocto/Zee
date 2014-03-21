#include"Input.h"
#include"D3DUtility.h"

Input::Input()
:DIDKeyboard(NULL)
,DIDMouse(NULL)
{

}

bool Input::Init(HINSTANCE hInst, HWND hWnd)
{
	bool bResult = false;

	IDirectInput8* directInput = NULL;
	{
		Assert(SUCCEEDED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput,NULL)));
		Assert(SUCCEEDED(directInput->CreateDevice(GUID_SysKeyboard, &DIDKeyboard,NULL)));
		Assert(SUCCEEDED(directInput->CreateDevice(GUID_SysMouse, &DIDMouse,NULL)));

		DIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
		DIDKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		DIDKeyboard->Acquire();

		DIDMouse->SetDataFormat(&c_dfDIMouse);
		DIDMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		DIDMouse->Acquire();

	}

	bResult = true;
Exit:
	SAFE_RELEASE(directInput);		// TODO:OK?
	return bResult;
}

void Input::Destroy()
{
	SAFE_RELEASE(DIDKeyboard);
	SAFE_RELEASE(DIDMouse);
}

void Input::GetDeviceState(HWND hWnd)
{
	for(int i = 0; i < 256; ++i)
		lastKeyState[i] = keyState[i];

	lastMouseState = mouseState;

	lastCursorPos = cursorPos;

	HRESULT hr;
	hr = DIDKeyboard->GetDeviceState(sizeof(keyState), (void*)&keyState);
	while(hr == DIERR_INPUTLOST)
	{
		hr = DIDKeyboard->Acquire();
	}

	hr = DIDMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&mouseState);
	while(hr == DIERR_INPUTLOST)
	{
		hr = DIDMouse->Acquire();
	}

	::GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);
}

bool Input::GetKeyDown(BYTE keyCode)
{
	if((keyState[keyCode]&0x80) && !(lastKeyState[keyCode]&0x80))
		return true;
	return false;
}


bool Input::GetKeyUp(BYTE keyCode)
{
	if(!(keyState[keyCode]&0x80) && (lastKeyState[keyCode]&0x80))
		return true;
	return false;
}

bool Input::GetKey(BYTE keyCode)
{
	if(keyState[keyCode] & 0x80)
		return true;
	return false;
}

bool Input::GetLeftButton()
{
	if(mouseState.rgbButtons[0] & 0x80)
		return true;
	return false;
}

bool Input::GetLeftButtonDown()
{
	if((mouseState.rgbButtons[0] & 0x80) && !(lastMouseState.rgbButtons[0] & 0x80))
		return true;
	return false;
}

bool Input::GetLeftButtonUp()
{
	if(!(mouseState.rgbButtons[0] & 0x80) && (lastMouseState.rgbButtons[0] & 0x80))
		return true;
	return false;
}

bool Input::GetRightButton()
{
	if(mouseState.rgbButtons[1] & 0x80)
		return true;
	return false;
}

bool Input::GetRightButtonDown()
{
	if((mouseState.rgbButtons[1] & 0x80) && !(lastMouseState.rgbButtons[1] & 0x80))
		return true;
	return false;
}

bool Input::GetRightButtonUp()
{
	if(!(mouseState.rgbButtons[1] & 0x80) && (lastMouseState.rgbButtons[1] & 0x80))
		return true;
	return false;
}

DIMOUSESTATE Input::GetMouseState()
{
	return mouseState;
}

POINT Input::GetCursorPos()
{
	return cursorPos;
}

POINT Input::GetLastCursorPos()
{
	return lastCursorPos;
}
