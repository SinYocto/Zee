#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include"dinput.h"

class Input
{
public:

	static bool Init(HINSTANCE hInst, HWND hWnd);
	static void Destroy();

	static void GetDeviceState(HWND hWnd);
	static bool GetKey(BYTE keyCode);
	static bool GetKeyDown(BYTE keyCode);
	static bool GetKeyUp(BYTE keyCode);
	static bool GetLeftButton();
	static bool GetLeftButtonDown();
	static bool GetLeftButtonUp();
	static bool GetRightButton();
	static bool GetRightButtonDown();
	static bool GetRightButtonUp();

public:
	static DIMOUSESTATE mouseState;
	static POINT cursorPos;

	static IDirectInputDevice8 *DIDKeyboard;
	static IDirectInputDevice8 *DIDMouse;

private:
	static char keyState[256];

	static POINT lastCursorPos;
	static char lastKeyState[256];
	static DIMOUSESTATE lastMouseState;
};

#endif