#include "GUI.h"
#include "Engine.h"

LabelStyle gDefaultLabelStyle(TEXT("Consolas"), 7, 15, 100, 0xffe3b706, DT_CENTER | DT_VCENTER);
ButtonStyle gDefaultButtonStyle(TEXT("./Assets/Textures/GUI/buttonNormal3.tga"), TEXT("./Assets/Textures/GUI/buttonHover3.tga"), TEXT("./Assets/Textures/GUI/buttonActive3.tga"));
SliderStyle gDefaultSliderStyle(TEXT("./Assets/Textures/GUI/sliderBar3.tga"), TEXT("./Assets/Textures/GUI/sliderThumbNormal3.tga"), 
                               TEXT("./Assets/Textures/GUI/sliderThumbHover3.tga"), TEXT("./Assets/Textures/GUI/sliderThumbActive3.tga"), 280.0f/360.0f);
ToggleStyle gDefaultToggleStyle(TEXT("./Assets/Textures/GUI/toggleOffNormal4.tga"), TEXT("./Assets/Textures/GUI/toggleOffHover4.tga"), 
                               TEXT("./Assets/Textures/GUI/toggleOnNormal4.tga"), TEXT("./Assets/Textures/GUI/toggleOnHover4.tga"));
ListBoxStyle gDefaultListBoxStyle(&gDefaultLabelStyle, TEXT("./Assets/Textures/GUI/listBoxButtonNormal2.tga"), TEXT("./Assets/Textures/GUI/listBoxButtonHover2.tga"), 
                                 TEXT("./Assets/Textures/GUI/listBoxItemNormal2.tga"), TEXT("./Assets/Textures/GUI/listBoxItemHover2.tga"));
GUI gGUISystem;

LabelStyle::LabelStyle(wchar_t* font_name, int font_width, int font_height, int font_weight, D3DXCOLOR font_color, DWORD align)
{
	wcscpy_s(fontName, _countof(fontName), font_name);
	fontWidth = font_width;
	fontHeight = font_height;
	fontWeight = font_weight;

	color = font_color;
	alignFlag = align;

	font = NULL;
}

//bool LabelStyle::CreateFont()
//{
//	Assert(Driver::D3DDevice);
//	{
//		SAFE_RELEASE(font);
//
//		D3DXFONT_DESC fontDesc;
//		ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESC));
//		wcscpy_s(fontDesc.FaceName, _countof(fontDesc.FaceName), fontName);
//		fontDesc.Height = fontHeight;
//		fontDesc.Width = fontWidth;
//		fontDesc.Weight = fontWeight;
//		fontDesc.MipLevels = D3DX_DEFAULT;
//		fontDesc.Quality = DEFAULT_QUALITY ;
//		fontDesc.OutputPrecision = OUT_OUTLINE_PRECIS;
//
//		Assert(SUCCEEDED(D3DXCreateFontIndirect(Driver::D3DDevice, &fontDesc, &font)));
//	}
//
//	return true;
//Exit:
//	return false;
//}

Label::Label(wchar_t* label_text, Rect label_rect, LabelStyle* label_style)
{
	if(label_style == NULL)
		style = &gDefaultLabelStyle;
	else
		style = label_style;

	rect = label_rect;
	screenRect = rect;

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	screenRect.TransLate(viewPort.X, viewPort.Y);

    wcscpy_s(text, _countof(text), label_text);
}

ButtonStyle::ButtonStyle(wchar_t *_normalTexFilePath, wchar_t *_hoverTexFilePath, wchar_t *_activeTexFilePath)
{
	wcscpy_s(normalTexFilePath, _countof(normalTexFilePath), _normalTexFilePath);
	wcscpy_s(hoverTexFilePath, _countof(hoverTexFilePath), _hoverTexFilePath);
	wcscpy_s(activeTexFilePath, _countof(activeTexFilePath), _activeTexFilePath);

	normalTex = NULL;
	hoverTex = NULL;
	activeTex = NULL;
}

void ButtonStyle::CreateTextures()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	if(normalTex)
		SAFE_RELEASE(normalTex);
	D3DXCreateTextureFromFile(d3dDevice, normalTexFilePath, &normalTex);
	
	if(hoverTex)
		SAFE_RELEASE(hoverTex);
	D3DXCreateTextureFromFile(d3dDevice, hoverTexFilePath, &hoverTex);
	
	if(activeTex)
		SAFE_RELEASE(activeTex);
	D3DXCreateTextureFromFile(d3dDevice, activeTexFilePath, &activeTex);
}

void Button::CreateVertexBuffer()
{
	VertexXYZRHWUV vertexData[4];
	vertexData[0] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.top, 0, 1, 0, 0);
	vertexData[1] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.top, 0, 1, 1, 0);
	vertexData[2] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.bottom, 0, 1, 0, 1);
	vertexData[3] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &vertexBuffer, vertexData, 4, XYZRHW_UV);
}

Button::Button(LPWSTR button_text, Rect button_rect, ButtonStyle *button_style, LabelStyle* label_style)
{
	if(button_style == NULL)
		buttonStyle = &gDefaultButtonStyle;
	else
		buttonStyle = button_style;
	
	if(label_style == NULL)
		labelStyle = &gDefaultLabelStyle;
	else
		labelStyle = label_style;

	text = button_text;

	rect = button_rect;
	screenRect = rect;

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	screenRect.TransLate(viewPort.X, viewPort.Y);

	CreateVertexBuffer();
}

void Button::GetButtonState()
{
	Input* input = gEngine->GetInput();

	POINT cursorPos = input->GetCursorPos();
	if(cursorPos.x > rect.left && cursorPos.x < rect.right &&
		cursorPos.y > rect.top && cursorPos.y < rect.bottom)
	{
		if(input->GetLeftButton())
			state = Active;
		else
			state = Hover;
	}
	else
		state = Normal;
}


ToggleStyle::ToggleStyle(wchar_t* _offNormalTexFilePath, wchar_t* _offHoverTexFilePath, 
						 wchar_t* _onNormalTexFilePath, wchar_t* _onHoverTexFilePath)
{
	wcscpy_s(offNormalTexFilePath, _countof(offNormalTexFilePath), _offNormalTexFilePath);
	wcscpy_s(offHoverTexFilePath, _countof(offHoverTexFilePath), _offHoverTexFilePath);
	wcscpy_s(onNormalTexFilePath, _countof(onNormalTexFilePath), _onNormalTexFilePath);
	wcscpy_s(onHoverTexFilePath, _countof(onHoverTexFilePath), _onHoverTexFilePath);

	offNormalTex = NULL;
	offHoverTex = NULL;
	onNormalTex = NULL;
	onHoverTex = NULL;
}

void ToggleStyle::CreateTextures()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	if(offNormalTex)
		SAFE_RELEASE(offNormalTex);
	D3DXCreateTextureFromFile(d3dDevice, offNormalTexFilePath, &offNormalTex);
	
	if(offHoverTex)
		SAFE_RELEASE(offHoverTex);
	D3DXCreateTextureFromFile(d3dDevice, offHoverTexFilePath, &offHoverTex);
	
	if(onNormalTex)
		SAFE_RELEASE(onNormalTex);
	D3DXCreateTextureFromFile(d3dDevice, onNormalTexFilePath, &onNormalTex);
	
	if(onHoverTex)
		SAFE_RELEASE(onHoverTex);
	D3DXCreateTextureFromFile(d3dDevice, onHoverTexFilePath, &onHoverTex);
}

Toggle::Toggle(Rect toggle_rect, bool _value, ToggleStyle *toggle_style)
{
	if(toggle_style == NULL)
		style = &gDefaultToggleStyle;
	else
		style = toggle_style;

	rect = toggle_rect;
	screenRect = rect;

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	screenRect.TransLate(viewPort.X, viewPort.Y);

	value = _value;

	CreateVertexBuffer();
}

void Toggle::CreateVertexBuffer()
{
	VertexXYZRHWUV vertexData[4];
	vertexData[0] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.top, 0, 1, 0, 0);
	vertexData[1] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.top, 0, 1, 1, 0);
	vertexData[2] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.bottom, 0, 1, 0, 1);
	vertexData[3] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &vertexBuffer, vertexData, 4, XYZRHW_UV);
}

void Toggle::GetButtonState()
{
	Input* input = gEngine->GetInput();

	POINT cursorPos = input->GetCursorPos();
	if(cursorPos.x > screenRect.left && cursorPos.x < screenRect.right &&
		cursorPos.y > screenRect.top && cursorPos.y < screenRect.bottom)
	{
		if(input->GetLeftButton())
			state = Active;
		else
			state = Hover;
	}
	else
		state = Normal;
}


SliderStyle::SliderStyle(wchar_t *_sliderBarTexFilePath, wchar_t *_normalTexFilePath, 
						 wchar_t *_hoverTexFilePath, wchar_t *_activeTexFilePath, float _sliderBarSize)
{
	wcscpy_s(sliderBarTexFilePath, _countof(sliderBarTexFilePath), _sliderBarTexFilePath);
	wcscpy_s(normalTexFilePath, _countof(normalTexFilePath), _normalTexFilePath);
	wcscpy_s(hoverTexFilePath, _countof(hoverTexFilePath), _hoverTexFilePath);
	wcscpy_s(activeTexFilePath, _countof(activeTexFilePath), _activeTexFilePath);
	sliderBarSize = _sliderBarSize;

	sliderBarTex = NULL;
	normalTex = NULL;
	hoverTex = NULL;
	activeTex = NULL;
}

void SliderStyle::CreateTextures()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	if(sliderBarTex)
		SAFE_RELEASE(sliderBarTex);
	D3DXCreateTextureFromFile(d3dDevice, sliderBarTexFilePath, &sliderBarTex);

	if(normalTex)
		SAFE_RELEASE(normalTex);
	D3DXCreateTextureFromFile(d3dDevice, normalTexFilePath, &normalTex);
	
	if(hoverTex)
		SAFE_RELEASE(hoverTex);
	D3DXCreateTextureFromFile(d3dDevice, hoverTexFilePath, &hoverTex);
	
	if(activeTex)
		SAFE_RELEASE(activeTex);
	D3DXCreateTextureFromFile(d3dDevice, activeTexFilePath, &activeTex);
}

Slider::Slider(Rect slider_rect, float _minValue, float _maxValue, float _value, char* slider_id, SliderStyle *slider_style)
{
	if(slider_style == NULL)
		style = &gDefaultSliderStyle;
	else
		style = slider_style;

	id = slider_id;

	rect = slider_rect;
	screenRect = rect;

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	screenRect.TransLate(viewPort.X, viewPort.Y);

	minValue = _minValue;
	maxValue = _maxValue;
	value = _value;
	if(value < minValue)
		value = minValue;
	if(value > maxValue)
		value = maxValue;

	CreateVertexBuffer();
}

void Slider::CreateVertexBuffer()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	VertexXYZRHWUV barVertexData[4];
	barVertexData[0] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.top, 0, 1, 0, 0);
	barVertexData[1] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.top, 0, 1, 1, 0);
	barVertexData[2] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.bottom, 0, 1, 0, 1);
	barVertexData[3] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(d3dDevice, &sliderBarVB, barVertexData, 4, XYZRHW_UV);

	VertexXYZRHWUV thumbVertexData[4];
	float offset = ((value - minValue)/(maxValue - minValue)) * 
		(screenRect.right - style->sliderBarSize*(screenRect.bottom - screenRect.top) - screenRect.left);

	float width = style->sliderBarSize * (screenRect.bottom - screenRect.top);
	thumbVertexData[0] = VertexXYZRHWUV(screenRect.left + offset, (float)screenRect.top, 0, 1, 0, 0);
	thumbVertexData[1] = VertexXYZRHWUV(screenRect.left + offset + width, (float)screenRect.top, 0, 1, 1, 0);
	thumbVertexData[2] = VertexXYZRHWUV(screenRect.left + offset, (float)screenRect.bottom, 0, 1, 0, 1);
	thumbVertexData[3] = VertexXYZRHWUV(screenRect.left + offset + width, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(d3dDevice, &sliderThumbVB, thumbVertexData, 4, XYZRHW_UV);
}

void Slider::GetButtonState()
{
	Input* input = gEngine->GetInput();
	POINT cursorPos = input->GetCursorPos();

	if(state == Active && input->GetLeftButton())
		return;

	float offset = ((value - minValue)/(maxValue - minValue)) * 
		(screenRect.right - style->sliderBarSize*(screenRect.bottom - screenRect.top) - screenRect.left);
	float width = style->sliderBarSize * (screenRect.bottom - screenRect.top);

	if(cursorPos.x > screenRect.left && cursorPos.x < screenRect.right &&
		cursorPos.y > screenRect.top && cursorPos.y < screenRect.bottom && input->GetLeftButton())
	{
		state = Active;
		return;
	}

	if(cursorPos.x > screenRect.left + offset && cursorPos.x < screenRect.left + offset + width &&
		cursorPos.y > screenRect.top && cursorPos.y < screenRect.bottom)
	{
		state = Hover;
	}
	else
		state = Normal;
}


ListBoxStyle::ListBoxStyle(LabelStyle* _labelStyle, wchar_t* _buttonNormalTexFilePath, wchar_t* _buttonHoverTexFilePath, 
						   wchar_t* _itemNormalTexFilePath, wchar_t* _itemHoverTexFilePath)
{
	labelStyle = _labelStyle;

	wcscpy_s(buttonNormalTexFilePath, _countof(buttonNormalTexFilePath), _buttonNormalTexFilePath);
	wcscpy_s(buttonHoverTexFilePath, _countof(buttonHoverTexFilePath), _buttonHoverTexFilePath);
	wcscpy_s(itemNormalTexFilePath, _countof(itemNormalTexFilePath), _itemNormalTexFilePath);
	wcscpy_s(itemHoverTexFilePath, _countof(itemHoverTexFilePath), _itemHoverTexFilePath);

	buttonNormalTex = NULL;
	buttonHoverTex = NULL;
	itemNormalTex = NULL;
	itemHoverTex = NULL;
}

void ListBoxStyle::CreateTextures()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	if(buttonNormalTex)
		SAFE_RELEASE(buttonNormalTex);
	D3DXCreateTextureFromFile(d3dDevice, buttonNormalTexFilePath, &buttonNormalTex);
	
	if(buttonHoverTex)
		SAFE_RELEASE(buttonHoverTex);
	D3DXCreateTextureFromFile(d3dDevice, buttonHoverTexFilePath, &buttonHoverTex);
	
	if(itemNormalTex)
		SAFE_RELEASE(itemNormalTex);
	D3DXCreateTextureFromFile(d3dDevice, itemNormalTexFilePath, &itemNormalTex);
	
	if(itemHoverTex)
		SAFE_RELEASE(itemHoverTex);
	D3DXCreateTextureFromFile(d3dDevice, itemHoverTexFilePath, &itemHoverTex);
}

void ListBox::CreateBodyVB()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	int height= rect.bottom - rect.top;

	VertexXYZRHWUV labelVertexData[4];
	labelVertexData[0] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.top, 0, 1, 0, 0);
	labelVertexData[1] = VertexXYZRHWUV((float)screenRect.right - height, (float)screenRect.top, 0, 1, 1, 0);
	labelVertexData[2] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.bottom, 0, 1, 0, 1);
	labelVertexData[3] = VertexXYZRHWUV((float)screenRect.right - height, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(d3dDevice, &labelVB, labelVertexData, 4, XYZRHW_UV);
	
	VertexXYZRHWUV buttonVertexData[4];
	buttonVertexData[0] = VertexXYZRHWUV((float)screenRect.right - height, (float)screenRect.top, 0, 1, 0, 0);
	buttonVertexData[1] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.top, 0, 1, 1, 0);
	buttonVertexData[2] = VertexXYZRHWUV((float)screenRect.right - height, (float)screenRect.bottom, 0, 1, 0, 1);
	buttonVertexData[3] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.bottom, 0, 1, 1, 1);

	CreateVB(d3dDevice, &buttonVB, buttonVertexData, 4, XYZRHW_UV);
}

void ListBox::CreateItemVB(int itemIx)
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	VertexXYZRHWUV vertexData[4];
	int height= rect.bottom - rect.top;
	vertexData[0] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.top + (itemIx+1)*height, 0, 1, 0, 0);
	vertexData[1] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.top + (itemIx+1)*height, 0, 1, 1, 0);
	vertexData[2] = VertexXYZRHWUV((float)screenRect.left, (float)screenRect.bottom + (itemIx+1)*height, 0, 1, 0, 1);
	vertexData[3] = VertexXYZRHWUV((float)screenRect.right, (float)screenRect.bottom + (itemIx+1)*height, 0, 1, 1, 1);

	CreateVB(d3dDevice, &(items[itemIx].vertexBuffer), vertexData, 4, XYZRHW_UV);
}

ListBox::ListBox(std::vector<wchar_t*> _texts, Rect _rect, int _ix, char* _id, ListBoxStyle* listBox_style)
{
	if(listBox_style == NULL)
		style = &gDefaultListBoxStyle;
	else
		style = listBox_style;

	rect = _rect;
	screenRect = rect;

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	screenRect.TransLate(viewPort.X, viewPort.Y);

	selectedIx = _ix;
	strcpy_s(id, _countof(id), _id);
	isSelecting = false;
	cursorState = 0;
	CreateBodyVB();

    for(std::vector<wchar_t*>::iterator iter = _texts.begin(); iter != _texts.end(); ++iter){
		AddItem((*iter));
	}
}

void ListBox::AddItem(wchar_t* _text)
{
	ListBoxItem item(_text, (int)items.size());

	items.push_back(item);
	CreateItemVB(item.ix);
}

void ListBox::GetState()
{
	Input* input = gEngine->GetInput();
	POINT cursorPos = input->GetCursorPos();

	int height = screenRect.bottom - screenRect.top;

	if(cursorPos.x < (int)screenRect.left || cursorPos.x > (int)screenRect.right ||
		cursorPos.y < (int)screenRect.top || cursorPos.y > (int)screenRect.bottom + (int)items.size() * height)
	{
		cursorState = 0;
		return;
	}

	cursorState = (int)(cursorPos.y - screenRect.top) / height + 1;

	if(cursorState == 1)
	{
		if(input->GetLeftButtonUp())
			isSelecting = !isSelecting;
	}
	else
	{
		if(input->GetLeftButtonUp() && isSelecting == true)
		{
			selectedIx = cursorState - 2;
			isSelecting = false;
		}
	}
}


void GUI::AddLabel(Label *label)
{
	labels.push_back(label);
}

void GUI::AddButton(Button* button)
{
	buttons.push_back(button);
}

void GUI::AddSlider(Slider* slider)
{
	sliders.push_back(slider);
}

void GUI::AddToggle(Toggle* toggle)
{
	toggles.push_back(toggle);
}

void GUI::AddListBox(ListBox* listBox)
{
	listBoxes.push_back(listBox);
}

void GUI::clear()
{
	for(std::list<Label*>::iterator iter = labels.begin(); iter != labels.end(); ++iter)
	{
		delete (*iter);
	}
	labels.clear();

	for(std::list<Button*>::iterator iter = buttons.begin(); iter != buttons.end(); ++iter)
	{
		delete (*iter);
	}
	buttons.clear();
	
	for(std::list<Toggle*>::iterator iter = toggles.begin(); iter != toggles.end(); ++iter)
	{
		delete (*iter);
	}
	toggles.clear();

	for(std::list<Slider*>::iterator iter = sliders.begin(); iter != sliders.end(); ++iter)
	{
		delete (*iter);
	}
	sliders.clear();
	
	for(std::list<ListBox*>::iterator iter = listBoxes.begin(); iter != listBoxes.end(); ++iter)
	{
		delete (*iter);
	}
	listBoxes.clear();
}

void GUI::GUILabel(wchar_t* label_text, Rect label_rect, LabelStyle* label_style)
{
	Label* label = New Label(label_text, label_rect, label_style);
	AddLabel(label);
}

bool GUI::GUIButton(wchar_t* button_text, Rect button_rect, ButtonStyle *button_style, LabelStyle *label_style)
{
	Button* button = New Button(button_text, button_rect, button_style, label_style);
	AddButton(button);

	button->GetButtonState();
	if(gEngine->GetInput()->GetLeftButtonUp() && button->state == Hover)
		return true;
	return false;
}

bool GUI::GUIToggle(Rect toggle_rect, bool _value, ToggleStyle *toggle_style)
{
	Toggle* toggle = New Toggle(toggle_rect, _value, toggle_style);
	AddToggle(toggle);

	toggle->GetButtonState();
	if(toggle->state != Normal && gEngine->GetInput()->GetLeftButtonUp())
		toggle->value = !toggle->value;

	return toggle->value;
}

float GUI::GUISlider(Rect slider_rect, float _minValue, float _maxValue, float _value, char* slider_id, SliderStyle* slider_style)
{ 
	Slider* slider = New Slider(slider_rect, _minValue, _maxValue, _value, slider_id, slider_style);
	AddSlider(slider);

	if(sliderState.count(slider_id))
		slider->state = sliderState[slider_id];
	else
		slider->state = Normal;

	slider->GetButtonState();
	sliderState[slider_id] = slider->state;

	if(slider->state == Active){
		float width = slider->style->sliderBarSize*(slider->rect.bottom - slider->rect.top);

		POINT cursorPos = gEngine->GetInput()->GetCursorPos();
		slider->value = slider->minValue + (slider->maxValue - slider->minValue)*(cursorPos.x - width/2 - slider->rect.left)/
			(slider->rect.right - slider->rect.left - width);

		if(slider->value < slider->minValue)
			slider->value = slider->minValue;
		if(slider->value > slider->maxValue)
			slider->value = slider->maxValue;
	}
	return slider->value;
}

int GUI::GUIListBox(std::vector<LPWSTR> _texts, Rect _rect, int _ix, char* _id, ListBoxStyle* listBox_style)
{
	ListBox* listBox = New ListBox(_texts, _rect, _ix, _id, listBox_style);
	AddListBox(listBox);

	if(listBoxState.count(_id))
		listBox->isSelecting = listBoxState[_id];
	else
		listBox->isSelecting = false;

	listBox->GetState();
	listBoxState[_id] = listBox->isSelecting;
	
	return listBox->selectedIx;
}

void GUI::Draw()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	for(std::list<Label*>::iterator iter = labels.begin(); iter != labels.end(); ++iter){
		RECT rect;
		rect.left = (*iter)->screenRect.left;
		rect.top = (*iter)->screenRect.top;
		rect.right = (*iter)->screenRect.right;
		rect.bottom = (*iter)->screenRect.bottom;

		(*iter)->style->font->DrawTextW(NULL, (*iter)->text, -1, &rect, (*iter)->style->alignFlag, (*iter)->style->color);
	}

	for(std::list<Button*>::iterator iter = buttons.begin(); iter != buttons.end(); ++iter){
		d3dDevice->SetStreamSource(0, (*iter)->vertexBuffer, 0, sizeof(VertexXYZRHWUV));
		d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

		switch((*iter)->state){
			case Normal:
				d3dDevice->SetTexture(0, (*iter)->buttonStyle->normalTex);
				break;
			case Hover:
				d3dDevice->SetTexture(0, (*iter)->buttonStyle->hoverTex);
				break;
			case Active:
				d3dDevice->SetTexture(0, (*iter)->buttonStyle->activeTex);
				break;
		}

		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		RECT rect;
		rect.left = (*iter)->screenRect.left;
		rect.top = (*iter)->screenRect.top;
		rect.right = (*iter)->screenRect.right;
		rect.bottom = (*iter)->screenRect.bottom;

		(*iter)->labelStyle->font->DrawTextW(NULL, (*iter)->text, -1, &rect, (*iter)->labelStyle->alignFlag, (*iter)->labelStyle->color);
	}

	for(std::list<Toggle*>::iterator iter = toggles.begin(); iter != toggles.end(); ++iter){
		d3dDevice->SetStreamSource(0, (*iter)->vertexBuffer, 0, sizeof(VertexXYZRHWUV));
		d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

		if((*iter)->value == false){
			if((*iter)->state == Normal)
				d3dDevice->SetTexture(0, (*iter)->style->offNormalTex);
			else
				d3dDevice->SetTexture(0, (*iter)->style->offHoverTex);
		}
		else{
			if((*iter)->state == Normal)
				d3dDevice->SetTexture(0, (*iter)->style->onNormalTex);
			else
				d3dDevice->SetTexture(0, (*iter)->style->onHoverTex);
		}

		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	for(std::list<Slider*>::iterator iter = sliders.begin(); iter != sliders.end(); ++iter){
		d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

		d3dDevice->SetStreamSource(0, (*iter)->sliderBarVB, 0, sizeof(VertexXYZRHWUV));
		d3dDevice->SetTexture(0, (*iter)->style->sliderBarTex);
		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		d3dDevice->SetStreamSource(0, (*iter)->sliderThumbVB, 0, sizeof(VertexXYZRHWUV));
		switch((*iter)->state){
			case Normal:
				d3dDevice->SetTexture(0, (*iter)->style->normalTex);
				break;
			case Hover:
				d3dDevice->SetTexture(0, (*iter)->style->hoverTex);
				break;
			case Active:
				d3dDevice->SetTexture(0, (*iter)->style->activeTex);
				break;
		}
		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	
	for(std::list<ListBox*>::iterator iter = listBoxes.begin(); iter != listBoxes.end(); ++iter){
		d3dDevice->SetStreamSource(0, (*iter)->labelVB, 0, sizeof(VertexXYZRHWUV));
		d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

		if((*iter)->cursorState == 1)
			d3dDevice->SetTexture(0, (*iter)->style->itemHoverTex);
		else
			d3dDevice->SetTexture(0, (*iter)->style->itemNormalTex);

		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		
		d3dDevice->SetStreamSource(0, (*iter)->buttonVB, 0, sizeof(VertexXYZRHWUV));
		if((*iter)->cursorState == 1)
			d3dDevice->SetTexture(0, (*iter)->style->buttonHoverTex);
		else
			d3dDevice->SetTexture(0, (*iter)->style->buttonNormalTex);
		d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		RECT rect;
		rect.left = (*iter)->screenRect.left;
		rect.top = (*iter)->screenRect.top;
		rect.right = (*iter)->screenRect.right - ((*iter)->screenRect.bottom - (*iter)->screenRect.top);
		rect.bottom = (*iter)->screenRect.bottom;

		(*iter)->style->labelStyle->font->DrawTextW(NULL, (*iter)->items[(*iter)->selectedIx].text, -1, &rect, (*iter)->style->labelStyle->alignFlag, (*iter)->style->labelStyle->color);

		if((*iter)->isSelecting){
			for(size_t i = 0; i < (*iter)->items.size(); ++i){
				d3dDevice->SetStreamSource(0, (*iter)->items[i].vertexBuffer, 0, sizeof(VertexXYZRHWUV));

				if((*iter)->cursorState == (*iter)->items[i].ix + 2)
					d3dDevice->SetTexture(0, (*iter)->style->itemHoverTex);
				else
					d3dDevice->SetTexture(0, (*iter)->style->itemNormalTex);

				d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				
				RECT rect;
				int height = (*iter)->screenRect.bottom - (*iter)->screenRect.top;
				rect.left = (*iter)->screenRect.left;
				rect.top = (*iter)->screenRect.top + (i+1)*height;
				rect.right = (*iter)->screenRect.right;
				rect.bottom = (*iter)->screenRect.bottom + (i+1)*height;

				(*iter)->style->labelStyle->font->DrawTextW(NULL, (*iter)->items[i].text, -1, &rect, (*iter)->style->labelStyle->alignFlag, (*iter)->style->labelStyle->color);
			}
		}
	}
	
	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}