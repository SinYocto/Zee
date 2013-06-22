#ifndef GUI_H
#define GUI_H

#include"D3DUtility.h"
#include"Math.h"
#include<vector>
#include<list>
#include<map>

enum ButtonState { Normal, Hover, Active };

class LabelStyle
{
public:
	LabelStyle(wchar_t* font_name, int font_width, int font_height, int font_weight, D3DXCOLOR font_color, DWORD align);
	~LabelStyle()
	{
		SAFE_RELEASE(font);
	}

	void OnLostDevice()
	{
		font->OnLostDevice();
	}

	void OnResetDevice()
	{
		font->OnResetDevice();
	}

	bool CreateFont();

public:
	ID3DXFont* font;
	D3DXCOLOR color;
	DWORD alignFlag;

private:
	wchar_t fontName[MAX_STR_LEN];

	int fontWidth;
	int fontHeight;
	int fontWeight;
};

class Label
{
public:
	Label(wchar_t* label_text, Rect label_rect, LabelStyle* label_style = NULL);
public:
	wchar_t text[MAX_STR_LEN];
	Rect rect;
	LabelStyle* style; 
};

class ButtonStyle
{
public:
	ButtonStyle(wchar_t* _normalTexFilePath, wchar_t* _hoverTexFilePath, wchar_t* _activeTexFilePath);
	~ButtonStyle()
	{
		SAFE_RELEASE(normalTex);
		SAFE_RELEASE(hoverTex);
		SAFE_RELEASE(activeTex);
	}

	void CreateTextures();
public:
	IDirect3DTexture9 *normalTex;	// texture资源创建是在D3DPOOL_MANAGED中, 不需要处理设备丢失
	IDirect3DTexture9 *hoverTex;
	IDirect3DTexture9 *activeTex;
private:
	wchar_t normalTexFilePath[MAX_PATH_LEN];
	wchar_t hoverTexFilePath[MAX_PATH_LEN];
	wchar_t activeTexFilePath[MAX_PATH_LEN];
};

class Button
{
public:
	Button(LPWSTR button_text, Rect button_rect, ButtonStyle* button_style = NULL, LabelStyle* label_style = NULL);
	~Button()
	{
		SAFE_RELEASE(vertexBuffer);
	}

	// 只处理了自身成员的设备丢失, 对于其使用的LabelStyle和ButtonStyle需要自己进行设备丢失处理
	void OnLostDevice()
	{
		SAFE_RELEASE(vertexBuffer);
	}

	void OnResetDevice()
	{
		CreateVertexBuffer();
	}

	void GetButtonState();
private:
	void CreateVertexBuffer();
public:
	LPWSTR text;
	Rect rect;
	ButtonStyle* buttonStyle;
	LabelStyle* labelStyle;
	IDirect3DVertexBuffer9 *vertexBuffer;

	ButtonState state;
};

class ToggleStyle
{
public:
	ToggleStyle(wchar_t* _offNormalTexFile, wchar_t* _offHoverTexFile, wchar_t* _onNormalTexFile, wchar_t* _onHoverTexFile);
	~ToggleStyle()
	{
		SAFE_RELEASE(offNormalTex);
		SAFE_RELEASE(offHoverTex);
		SAFE_RELEASE(onNormalTex);
		SAFE_RELEASE(onHoverTex);
	}

	void CreateTextures();
public:
	IDirect3DTexture9 *offNormalTex;
	IDirect3DTexture9 *offHoverTex;
	IDirect3DTexture9 *onNormalTex;
	IDirect3DTexture9 *onHoverTex;
private:
	wchar_t offNormalTexFilePath[MAX_PATH_LEN];
	wchar_t offHoverTexFilePath[MAX_PATH_LEN];
	wchar_t onNormalTexFilePath[MAX_PATH_LEN];
	wchar_t onHoverTexFilePath[MAX_PATH_LEN];
};


class Toggle
{
public:
	Toggle(Rect toggle_rect, bool _value, ToggleStyle* toggle_style = NULL);
	~Toggle()
	{
		SAFE_RELEASE(vertexBuffer);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(vertexBuffer);
	}

	void OnResetDevice()
	{
		CreateVertexBuffer();
	}

	void GetButtonState();
private:
	void CreateVertexBuffer();
public:
	Rect rect;
	ToggleStyle* style;
	IDirect3DVertexBuffer9 *vertexBuffer;

	ButtonState state;
	bool value;
}; 

class SliderStyle
{
public:
	SliderStyle(wchar_t* _sliderBarTexFilePath, wchar_t* _normalTexFilePath, wchar_t* _hoverTexFilePath, 
		wchar_t* _activeTexFilePath, float _sliderBarSize);

	~SliderStyle()
	{
		SAFE_RELEASE(sliderBarTex);
		SAFE_RELEASE(normalTex);
		SAFE_RELEASE(hoverTex);
		SAFE_RELEASE(activeTex);
	}

	void CreateTextures();
public:
	IDirect3DTexture9 *sliderBarTex;
	IDirect3DTexture9 *normalTex;
	IDirect3DTexture9 *hoverTex;
	IDirect3DTexture9 *activeTex;
	float sliderBarSize;

private:
	wchar_t sliderBarTexFilePath[MAX_PATH_LEN];
	wchar_t normalTexFilePath[MAX_PATH_LEN];
	wchar_t hoverTexFilePath[MAX_PATH_LEN];
	wchar_t activeTexFilePath[MAX_PATH_LEN];
};


class Slider
{
public:
	Slider(Rect slider_rect, float _minValue, float _maxValue, float _value, char* slider_id, SliderStyle* slider_style = NULL);
	~Slider()
	{
		SAFE_RELEASE(sliderBarVB);
		SAFE_RELEASE(sliderThumbVB);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(sliderBarVB);
		SAFE_RELEASE(sliderThumbVB);
	}

	void OnResetDevice()
	{
		CreateVertexBuffer();
	}

	void GetButtonState();
private:
	void CreateVertexBuffer();
public:
	Rect rect;
	SliderStyle* style;
	IDirect3DVertexBuffer9 *sliderBarVB;
	IDirect3DVertexBuffer9 *sliderThumbVB;

	float minValue;
	float maxValue;
	float value;

	char* id;

	ButtonState state;
};


class ListBoxStyle
{
public:
	ListBoxStyle(LabelStyle* _labelStyle, wchar_t* _buttonNormalTexFile, wchar_t* _buttonHoverTexFile, 
		wchar_t* _itemNormalTexFile, wchar_t* _itemHoverTexFile);

	~ListBoxStyle()
	{
		SAFE_RELEASE(buttonNormalTex);
		SAFE_RELEASE(buttonHoverTex);
		SAFE_RELEASE(itemNormalTex);
		SAFE_RELEASE(itemHoverTex);
	}

	void CreateTextures();
public:
	IDirect3DTexture9 *buttonNormalTex;
	IDirect3DTexture9 *buttonHoverTex;
	IDirect3DTexture9 *itemNormalTex;
	IDirect3DTexture9 *itemHoverTex;
	LabelStyle* labelStyle;
private:
	wchar_t buttonNormalTexFilePath[MAX_PATH_LEN];
	wchar_t buttonHoverTexFilePath[MAX_PATH_LEN];
	wchar_t itemNormalTexFilePath[MAX_PATH_LEN];
	wchar_t itemHoverTexFilePath[MAX_PATH_LEN];
};

struct ListBoxItem
{
	ListBoxItem(wchar_t* _text, int _ix)
		:ix(0)
		,vertexBuffer(NULL)
	{
		wcscpy_s(text, _countof(text), _text);
	}

	~ListBoxItem()
	{
		SAFE_RELEASE(vertexBuffer);
	}

	wchar_t text[MAX_STR_LEN];
	IDirect3DVertexBuffer9 *vertexBuffer;
	int ix;
};

class ListBox
{
public:
    ListBox(std::vector<wchar_t*> _texts, Rect _rect, int _ix, char* _id, ListBoxStyle* listBox_style = NULL);
	~ListBox()
	{
		SAFE_RELEASE(labelVB);
		SAFE_RELEASE(buttonVB);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(labelVB);
		SAFE_RELEASE(buttonVB);

		for(std::vector<ListBoxItem>::iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			SAFE_RELEASE(iter->vertexBuffer);
		}
	}

	void OnResetDevice()
	{
		CreateBodyVB();

		for(size_t itemIx = 0; itemIx < items.size(); ++itemIx)
		{
			CreateItemVB((int)itemIx);
		}
	}

	void AddItem(wchar_t* _text);
	void GetState();
private:
	void CreateBodyVB();
	void CreateItemVB(int itemIx);
public:
	std::vector<ListBoxItem> items;
	IDirect3DVertexBuffer9 *labelVB;
	IDirect3DVertexBuffer9 *buttonVB;
	Rect rect;
	ListBoxStyle* style;

	bool isSelecting;
	int cursorState;
	int selectedIx;

	char id[MAX_STR_LEN];
};

class GUI
{
public:
	~GUI()
	{
		clear();
	}

	void OnLostDevice()
	{
		for(std::list<Button*>::iterator iter = buttons.begin(); iter != buttons.end(); ++iter)
		{
			(*iter)->OnLostDevice();
		}

		for(std::list<Slider*>::iterator iter = sliders.begin(); iter != sliders.end(); ++iter)
		{
			(*iter)->OnLostDevice();
		}

		for(std::list<Toggle*>::iterator iter = toggles.begin(); iter != toggles.end(); ++iter)
		{
			(*iter)->OnLostDevice();
		}

		for(std::list<ListBox*>::iterator iter = listBoxes.begin(); iter != listBoxes.end(); ++iter)
		{
			(*iter)->OnLostDevice();
		}
	}

	void OnResetDevice()
	{
		for(std::list<Button*>::iterator iter = buttons.begin(); iter != buttons.end(); ++iter)
		{
			(*iter)->OnResetDevice();
		}

		for(std::list<Slider*>::iterator iter = sliders.begin(); iter != sliders.end(); ++iter)
		{
			(*iter)->OnResetDevice();
		}

		for(std::list<Toggle*>::iterator iter = toggles.begin(); iter != toggles.end(); ++iter)
		{
			(*iter)->OnResetDevice();
		}

		for(std::list<ListBox*>::iterator iter = listBoxes.begin(); iter != listBoxes.end(); ++iter)
		{
			(*iter)->OnResetDevice();
		}
	}

	void clear();

	void GUILabel(wchar_t* label_text, Rect label_rect, LabelStyle* label_style = NULL);
	bool GUIButton(wchar_t* button_text, Rect button_rect, ButtonStyle* button_style = NULL, LabelStyle* label_style = NULL);
	float GUISlider(Rect slider_rect, float _minValue, float _maxValue, float _value, char* slider_id, SliderStyle* slider_style = NULL);
	bool GUIToggle(Rect toggle_rect, bool _value, ToggleStyle* toggle_style = NULL);
	int GUIListBox(std::vector<LPWSTR> _texts, Rect _rect, int _ix, char* _id, ListBoxStyle* listBox_style = NULL);

	void AddLabel(Label* label);
	void AddButton(Button* button);
	void AddSlider(Slider* slider);
	void AddToggle(Toggle* toggle);
	void AddListBox(ListBox* listBox);

    void Draw();
public:
	std::map<char*, ButtonState> sliderState; 
	std::map<char*, bool> listBoxState;
private:
	std::list<Label*> labels;
	std::list<Button*> buttons;
	std::list<Slider*> sliders;
	std::list<Toggle*> toggles;
	std::list<ListBox*> listBoxes;
};


extern LabelStyle gDefaultLabelStyle;
extern ButtonStyle gDefaultButtonStyle;
extern ToggleStyle gDefaultToggleStyle;
extern SliderStyle gDefaultSliderStyle;
extern ListBoxStyle gDefaultListBoxStyle;
extern GUI gGUISystem;

#endif