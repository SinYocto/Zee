#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

class Input
{
public:
	Input();

	bool Init(HINSTANCE hInst, HWND hWnd);
	void Destroy();

	void GetDeviceState(HWND hWnd);
	bool GetKey(BYTE keyCode);
	bool GetKeyDown(BYTE keyCode);
	bool GetKeyUp(BYTE keyCode);
	bool GetLeftButton();
	bool GetLeftButtonDown();
	bool GetLeftButtonUp();
	bool GetRightButton();
	bool GetRightButtonDown();
	bool GetRightButtonUp();

	DIMOUSESTATE GetMouseState();
	POINT GetCursorPos();

private:
	IDirectInputDevice8 *DIDKeyboard;
	IDirectInputDevice8 *DIDMouse;

	DIMOUSESTATE mouseState;
	POINT cursorPos;

	char keyState[256];

	POINT lastCursorPos;
	char lastKeyState[256];
	DIMOUSESTATE lastMouseState;
};

#endif