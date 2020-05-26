/*
Name:		Components.cpp
Created:	29/07/2018 10:30:51
Author:	Buho29
*/

#include "Components.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,/*rst*/ 16, /*scl*/ 15, /*sda*/ 4);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// Font "Factory"
//const uint8_t font = getFont(Font::small);

const uint8_t *listFonts[] = {
	u8g2_font_profont10_mf,//verySmall 6pxl
	u8g2_font_t0_12_me ,//small default
	u8g2_font_t0_13b_me,//medium
	u8g2_font_t0_17b_me,//Large
	u8g2_font_logisoso18_tf,//veryLarge
	u8g2_font_open_iconic_weather_4x_t,//icon
	u8g2_font_open_iconic_embedded_4x_t,//icon1
	u8g2_font_open_iconic_text_4x_t//icon2
};

char charSymbol[] = {
	69,//sun
	65,//sun_cloud
	64,//cloud
	67,//rain
	67,//thunder
	68,//snow
	64,//mist
	66,//moon
	80,//wifi_on
	71//wifi_off
};

const uint8_t * getFont(uint8_t font)
{
	return listFonts[font];
}

/******************************
			UI
*******************************/

Ui::Ui()
{	
	dim.copy(Dimension{ 0,0,128,64 });
	gap.copy(Gap{ 2,4,4,4 });
}

Ui::Ui(Dimension dim)
{
	this->dim.copy(dim);
	gap.copy(Gap{ 2,4,4,4 });
}

void Ui::draw(int16_t x, int16_t y) {
	Serial.println("Error metodo draw() no implementado ");
}

void Ui::measure() {
	Serial.println("Error metodo measure() no implementado");
}

char * Ui::nameClass()
{
	return "Ui";
}

/******************************
			Text
*******************************/

Text::Text(Dimension dim, const char * txt, Align align, Font font)
{
	this->dim.copy(dim);
	gap.copy(Gap{ 2,4,4,4 });
	this->align = align;
	this->font = font;
	setText(txt);
}

Text::Text(Dimension dim, Align align, Font font)
{
	this->dim.copy(dim);
	gap.copy(Gap{ 2,4,4,4 });
	this->align = align;
	this->font = font;
}

void Text::measure()
{
	//
	const uint8_t * f = getFont(font);
	u8g2.setFont(f);

	textDim.w =  u8g2.getStrWidth(text);
	textDim.h = u8g2.getAscent()+u8g2.getDescent()*-1;

	if (dim.h < 0)
		dim.h = gap.top + textDim.h + gap.button;
	if (dim.w < 0)
		dim.w = gap.left + textDim.w + gap.rigth;
	
	switch (align)
	{
	case ALeft:
		textDim.x = dim.x + gap.left;
		break;
	case ACenter:
		textDim.x = dim.x + (dim.w - textDim.w) / 2  ;
		break;
	case ARigth:
		textDim.x = dim.x + (dim.w - textDim.w);
		break;
	}
	textDim.y = dim.y + textDim.h + gap.top;

}

void Text::draw(int16_t x, int16_t y)
{

	if (!visible)
		return;

	measure();

	u8g2.setCursor(textDim.x + x, textDim.y + y);
	u8g2.print(text);
	
	//Serial.println(selected);
	if (selected) u8g2.drawFrame(dim.x + x, dim.y + y, dim.w, dim.h);
}

void Text::setText(const char * txt)
{
	strncpy(text, txt,TEXT_MAX_SIZE);
}

char * Text::getText()
{
	return text;
}

char * Text::nameClass()
{
	return "Text";
}

/******************************
			Button
*******************************/

void Button::draw(int16_t x, int16_t y)
{

	if (!visible)
		return;

	measure();

	if (selected) {
		u8g2.drawRBox(dim.x + x, dim.y + y, dim.w, dim.h, 4);

		u8g2.setDrawColor(0);
		u8g2.setCursor(textDim.x + x, textDim.y + y);
		u8g2.print(text);
		u8g2.setDrawColor(1);
		return;
	}

	switch (status)
	{
		case onPress:

			u8g2.setCursor(textDim.x + 2 + x, textDim.y + y);
			u8g2.print(text);

			u8g2.drawRFrame(dim.x + 2 + x, dim.y + 2 + y, dim.w - 4, dim.h - 4, 4);
			break;

		default: //onRealease

			u8g2.setCursor(textDim.x + x, textDim.y + y);
			u8g2.print(text);

			u8g2.drawRFrame(dim.x + x, dim.y + y, dim.w, dim.h, 4);
			break;
	}

}

char * Button::nameClass()
{
	return "Button";
}

/******************************
			Container
*******************************/

void Container::addChild(Ui * child)
{
	childs[childCount++] = child;
}

Ui *Container::getChild(int index)
{
	return childs[index];
}

uint8_t Container::size()
{
	return childCount;
}

void Container::draw(int16_t x , int16_t y)
{
	if (!visible)
		return;

	measure();

	for (size_t i = 0; i < childCount; i++)
	{
		Ui *ui = childs[i];
		ui->draw(x , y);
	}
}

void Container::measure(){}

char * Container::nameClass()
{
	return "Container";
}

/******************************
			View
******************************/

void View::begin(){
	dim.w = u8g2.getDisplayWidth();
	dim.h = u8g2.getDisplayHeight();
}
bool View::acceptClickButton(uint8_t id)
{
	return false;
}
//motion
bool View::isPlaying()
{
	return tween.isRunning;
}
void View::play(float begin, float finish, bool vertical)
{
	verticalAnim = vertical;
	tween.play(begin, finish,vertical ? 15:10);
}
void View::play(Motion motion, Motion dir)
{
	float begin, finish;
	bool vertical = true;

	switch (motion)
	{
	case View::Left:
		vertical = false;
		if (dir == View::In) {
			begin = -dim.w;
			finish = 0;
		}
		else {
			begin = 0;
			finish = -dim.w;
		}
		break;
	case View::Rigth:
		vertical = false;
		if (dir == View::In) {
			begin = dim.w;
			finish = 0;
		}
		else {
			begin = 0;
			finish = dim.w;
		}
		break;
	case View::Up:
		if (dir == View::In) {
			begin = -dim.h;
			finish = 0;
		}
		else {
			begin = 0;
			finish = -dim.h;
		}
		break;
	case View::Bottom:
		if (dir == View::In) {
			begin = dim.h;
			finish = 0;
		}
		else {
			begin = 0;
			finish = dim.h;
		}
		break;
	}

	play(begin, finish, vertical);
}
//draw
void View::measure()
{
	if (tween.isRunning && verticalAnim) {
		dim.y = tween.get();
	}
	else if (tween.isRunning) {
		dim.x = tween.get();
	}
	else {
		dim.x = 0;
		dim.y = 0;
	}
}
void View::draw(int16_t x , int16_t y)
{

	measure();
	if (dim.x != 0 || dim.y != 0) {
		x = dim.x;
		y = dim.y;
	}

	uint32_t c = millis();
	if (currentView == nullptr) {
		for (size_t i = 0; i < childCount; i++)
		{
			Ui *ui = childs[i];
			ui->draw(x , y);
		}
	}
	else {
		if (currentView->isPlaying())
			previousView->draw(x, y);
		currentView->draw(x, y);
	}
	if (millis() - c > 50)
		Serial.printf("View::draw measure %d child%d\n", millis() - c, currentView != nullptr);
}
char * View::nameClass()
{
	return "View";
}
void View::home()
{
	setCurrent(nullptr);
}
void View::next()
{
	if (currentIndex < viewCount - 1)
		currentIndex++;
	else currentIndex = 0;

	previousView = currentView;
	currentView = viewChilds[currentIndex];

	if(previousView != nullptr)
		previousView->play(View::Up, View::Out);
	currentView->play(View::Bottom, View::In);

	Serial.println("view next");
}
void View::previous()
{
	if (currentIndex > 0)
		currentIndex--;
	else currentIndex = viewCount - 1;

	previousView = currentView;
	currentView = viewChilds[currentIndex];

	if (previousView != nullptr)
		previousView->play(View::Bottom, View::Out);
	currentView->play(View::Up, View::In);

}
// agrega una view a una view
// si la vista ya existe no se agrega
void View::addView(View * view)
{
	view->parentView = this;
	viewChilds[viewCount++] = view;
	view->begin();
}
View * View::getView(uint8_t id)
{
	return viewChilds[id];
}
int16_t View::getViewId(View * view)
{
	for (int16_t i = 0; i < viewCount; i++)
	{
		View * v = viewChilds[i];
		if (v == view) return i;
	}
	return -1;
}
void View::removeView(uint8_t id)
{
	//desplazamos los de delante para rellenar el hueco
	for (uint8_t i = id; i < viewCount - 1; i++)
	{
		viewChilds[i] = viewChilds[i+1];
	}
	//borramos el ultimo
	viewChilds[viewCount - 1] = nullptr;
	// restamos 1 a count
	viewCount--;
}
bool View::existView(View * view)
{

	return getViewId(view) > -1;
}
void View::setCurrent(View * view)
{
	if (view == nullptr) {
		currentView = nullptr;
		return;
	}

	for (size_t i = 0; i < viewCount; i++)
	{
		View * v = viewChilds[i];
		if (v == view ){
			currentIndex = i;
			currentView = view;
			return;
		}
	}
}
View * View::getCurrent()
{
	return currentView;
}

/*			ManagerView			*/



void ManagerView::next()
{
	if (currentIndex < viewCount-1)
		currentIndex++;
	else if(loopScroll) currentIndex = 0;
	else return;//

	previousView = currentView;
	currentView = views[currentIndex];

	if (previousView != nullptr)
		previousView->play(View::Left, View::Out);
	currentView->play(View::Rigth, View::In);
}

void ManagerView::previous()
{
	if (currentIndex > 0)
		currentIndex--;
	else if(loopScroll) currentIndex = viewCount-1;
	else return;//


	previousView = currentView;
	currentView = views[currentIndex];

	if (previousView != nullptr)
		previousView->play(View::Rigth, View::Out);
	currentView->play(View::Left, View::In);
}

void ManagerView::begin()
{
	// Inicio display
	u8g2.begin();
	u8g2.enableUTF8Print();

	for (size_t i = 0; i < viewCount; i++)
	{
		views[i]->begin();
	}
}

void ManagerView::add(View *view)
{
	views[viewCount++] = view;
	//setCurrent(view);
}

View *ManagerView::get(uint8_t index)
{
	return views[index];
}

View * ManagerView::getCurrent()
{
	return currentView;
}

void ManagerView::setCurrent(View * view)
{
	for (size_t i = 0; i < viewCount; i++)
	{
		if (view == views[i]) {
			currentIndex = i;
			currentView = view;
		}
	}
}

uint8_t ManagerView::size()
{
	return viewCount;
}

bool ManagerView::acceptClickButton(uint8_t id)
{
	return currentView->acceptClickButton(id);
}

void ManagerView::draw()
{
	static uint32_t currentTime = 0;
	static uint32_t currentTimeInterval = 0;
	
	if (!isEnabled) return;

	uint32_t c = millis();
	if (anim && millis() - currentTime > animInterval) {
		previous();
		currentTime = millis();
	}

	if (millis() - c > 60)
			Serial.printf("ManagerView::draw next %dms %s\n", millis() - c, getCurrent()->nameClass());


	if (millis() - currentTimeInterval > FPS) {
	
		c = millis();	
		u8g2.firstPage();
		do {
			if (currentView->isPlaying())
				previousView->draw();

			currentView->draw();
		} while (u8g2.nextPage());

		if (millis() - c > 60)
			Serial.printf("ManagerView::draw end %dms %s\n", millis() - c, getCurrent()->nameClass());

		//u8g2.clearBuffer();					// clear the internal memory
		//getCurrent()->draw();

		//u8g2.sendBuffer();

		//if (millis() - c > 50)
		//		Serial.printf("ManagerView::draw current %dms %s\n", millis() - c, getCurrent()->nameClass());
	
		currentTimeInterval = millis();
	}

}

void ManagerView::start()
{	
	anim = true;
	resetViews();
}

void ManagerView::enableDisplay(bool value)
{
	if (!value) {
		u8g2.clearBuffer();
		u8g2.sendBuffer();
	}
	isEnabled = value;
}

bool ManagerView::isEnabledDisplay()
{
	return isEnabled;
}

void ManagerView::stop()
{
	anim = false;
}

bool ManagerView::isStarted()
{
	return anim;
}

void ManagerView::setLoopscroll(bool value)
{
	loopScroll = value;
}

void ManagerView::resetViews()
{
	for (size_t i = 0; i < viewCount; i++)
	{
		views[i]->home();
	}
}


/******************************
			List
*******************************/

void ItemList::draw(int16_t x , int16_t y)
{
	if (!visible)
		return;

	measure();


	switch (status)
	{
	case onPress:
		
		u8g2.drawFrame(dim.x + x, dim.y + y, dim.w, dim.h);
		
		if (selected) {
			u8g2.drawBox(dim.x + x, dim.y + y, dim.w, dim.h);
			u8g2.setDrawColor(0);
		}
		u8g2.setCursor(textDim.x + x, textDim.y + y);
		u8g2.print(text);
		u8g2.setDrawColor(1);
		break;

	default: //onRealease
		if (selected) {
			u8g2.drawBox(dim.x + x, dim.y + y, dim.w, dim.h);
			u8g2.setDrawColor(0);
		}
		u8g2.setCursor(textDim.x + x, textDim.y + y);
		u8g2.print(text);
		u8g2.setDrawColor(1);
		break;
	}
}

char * ItemList::nameClass()
{
	return "ItemList";
}


List::List(Dimension dim)
{
	this->dim.copy(dim);
	this->align = Align::ACenter;
	this->posSelected = 2;

	for (uint8_t i = 0; i < CONTAINER_MAX_SIZE; i++)
	{
		Text * t = uis[i];
		if (t == nullptr) break;
		addChild(t);

		t->dim.w = dim.w;
		t->dim.h = -1;
		t->align = align;
	}
}

void List::measure()
{
	uint8_t init = 0, tcount = 0;


	if (selectedId - posSelected < 0)
		posSelected = 0;
		

	if(posSelected > 3)
		posSelected = 3;

	init = selectedId - posSelected;

	for (uint8_t i = init; i < init + 4 ;i++)
	{
		ItemList * t = (ItemList *) getChild(tcount);
		t->selected = false;
		t->dim.y = 16 * tcount;

		if (i < itemCount) {

			if(i==selectedId) 
				t->selected = true;

			t->setText(items[i]);
			t->status = (StatusButton) press[i];
		} 
		else {
			t->setText("");
			t->status = StatusButton::onRelease;
		}
		
		tcount++;

	}
}

char * List::nameClass()
{
	return "List";
}

uint8_t List::size()
{
	return itemCount;
}

void List::reset()
{
	for (uint8_t i = 0; i < CONTAINER_MAX_SIZE; i++)
	{
		strncpy(items[i], "", TEXT_MAX_SIZE);
	}
	itemCount = 0;
	currentSelected = 0;
	selectedId = 0;
	posSelected = 2;
}

uint8_t List::addItem(const char * str)
{
	if (itemCount < CONTAINER_MAX_SIZE) {

		strncpy(items[itemCount++], str,TEXT_MAX_SIZE);
	}

	return itemCount - 1;
}

bool List::selectItem(uint8_t index, bool press)
{
	if (index < itemCount) {
		selectedId = index;
		this->press[index] = press;
		return true;
	}
	return false;
}

int8_t List::selectedItem()
{
	return selectedId;
}

bool List::up()
{
	if (selectedId - 1  >= 0) {
		selectedId--;
		posSelected--;
		return true;
	}
	return false;
}

bool List::down()
{
	if (selectedId + 1  < itemCount) {
		selectedId++;
		posSelected++;
		return true;
	}
	return false;
}

/******************************
		TextScroll
*******************************/

TextScroll::TextScroll(Dimension dim)
{
	this->dim.copy(dim);
	offset = -dim.w;
	vel = 3;
}

void TextScroll::draw(int16_t x , int16_t y)
{
	if (!visible)
		return;

	measure();

	int16_t len = strlen(text1);

	offset += vel;
	if (offset > len * fontWidth + 1)
		offset = -dim.w;

	size_t char_offset = 0;
	u8g2_uint_t dx = 0;
	size_t visible = 0;

	//Serial.printf("%d < %d = %d\n", offset, dim.x, offset < dim.x);
	if (offset < dim.x)
	{
		char_offset = (-offset) / fontWidth;
		dx = offset + char_offset * fontWidth - dim.x;
		if (char_offset >= dim.w / fontWidth) {
			draw( x , y);
			return;
		}
		visible = dim.w / fontWidth - char_offset + 1;
		strncpy(text, text1, visible);
		text[visible] = '\0';

		u8g2.setCursor(char_offset * fontWidth - dx + x, dim.y + y);
		u8g2.print(text);
	}
	else
	{
		char_offset = offset / fontWidth;
		if (char_offset >= len)
			return;	// nothing visible
		dx = offset - char_offset * fontWidth - dim.x;
		visible = len - char_offset;
		if (visible > dim.w / fontWidth + 1)
			visible = dim.w / fontWidth + 1;
		strncpy(text, text1 + char_offset, visible);
		text[visible] = '\0';

		u8g2.setCursor(-dx + x, dim.y + y);
		u8g2.print(text);
	}
}

void TextScroll::measure()
{
	u8g2.setFont(getFont(font));
	fontWidth = u8g2.getStrWidth("M");
}

char * TextScroll::nameClass()
{
	return "TextScroll";
}

void TextScroll::setText(const char * txt)
{
	strncpy(text1, txt, 100);
}

char * TextScroll::getText()
{
	return text1;
}


/******************************
			Rectangle
*******************************/

void Rectangle::draw(int16_t x , int16_t y)
{
	if (visible) 
	{
		if(fill) u8g2.drawBox(dim.x + x, dim.y + y, dim.w, dim.h);
		else u8g2.drawFrame(dim.x + x, dim.y + y, dim.w, dim.h);
	}
}

char * Rectangle::nameClass()
{
	return "Rectangle";
}

void RectangleR::draw(int16_t x , int16_t y)
{
	if (visible) 
	{
		if (fill) u8g2.drawRBox(dim.x + x, dim.y + y, dim.w, dim.h, radius);
		else u8g2.drawRFrame(dim.x + x, dim.y + y, dim.w, dim.h, radius);
	}
		
}

char * RectangleR::nameClass()
{
	return "RectangleR";
}


/******************************
		Glyph
*******************************/

Glyph::Glyph(Dimension dim, uint8_t fontGlyph)
{
	this->dim.copy(dim);
	this->fontGlyph = fontGlyph;
}

void Glyph::draw(int16_t x , int16_t y)
{
	if (visible) {
		u8g2.setFont(
			getFont(getFontGlyph(fontGlyph))
		);
		u8g2.drawGlyph(dim.x + x, dim.y + y, charSymbol[fontGlyph]);
	}
}

uint8_t Glyph::getFontGlyph(uint8_t  font)
{
	FontGlyph b = (FontGlyph)font;
	if (b == sun || b == sun_cloud || b == cloud ||
		b == rain || b == snow || b == moon)
		return icon;
	else if (b == mist)
		return icon2;
	else if (b == thunder || b == wifi_on || b == wifi_off)
		return icon1;
}

/******************************
		InputNumber
*******************************/
InputNumber::InputNumber(Dimension dim,const char * format)
{
	this->dim.copy(dim);
	setFormat(format);
}

void InputNumber::draw(int16_t x , int16_t y)
{
	measure();

	if (selected) {
		u8g2.drawBox(dim.x + x, dim.y + y, dim.w, dim.h);
		u8g2.setDrawColor(0);
		u8g2.setCursor(textDim.x + x, textDim.y + y);
		u8g2.print(text);
		u8g2.setDrawColor(1);
	}
	else {
		u8g2.setCursor(textDim.x + x, textDim.y + y);
		u8g2.print(text);
	}
}

char * InputNumber::nameClass()
{
	return "InputNumber";
}

void InputNumber::plus()
{
	if (number + 1 > max)
		setNumber(min);
	else setNumber(number + 1);
}

void InputNumber::minus()
{
	if (number - 1 < min)
		setNumber(max);
	else setNumber(number -1);
}

void InputNumber::setNumber(int8_t n)
{
	number = n;
	char buff[10];
	sprintf(buff, format, number);
	setText(buff);
}

int8_t InputNumber::getNumber()
{
	return number;
}

void InputNumber::setFormat(const char * format)
{
	strncpy(this->format, format, 10);
}

void Color::draw(int16_t x , int16_t y)
{
	if(visible)
		u8g2.setDrawColor(color);
}

GraphXY::GraphXY(Dimension dim)
{
	this->dim.copy(dim);
}

void GraphXY::setData(float * data, uint8_t size)
{
	float maxValue = -FLT_MAX, minValue = FLT_MAX;

	for (uint8_t i = 0; i < size; i++)
	{
		float value = data[i];
		maxValue = MAX(value, maxValue);
		minValue = MIN(value, minValue);
	}
	setData(data, size, maxValue, minValue);
}

void GraphXY::setData(float * data, uint8_t size, float maxValue, float minValue)
{
	this->data = data;
	this->size = size;
	this->maxValue = maxValue;
	this->minValue = minValue;
	visible = true;
}

void GraphXY::draw(int16_t x , int16_t y)
{
	if (!visible) return;

	// vertical y horizontal
	int16_t yr = dim.y + dim.h -2;
	drawLine(dim.x + x, dim.y + y, dim.x + x, yr + y);
	drawLine(dim.x + x, yr + y, dim.x + dim.w + x, yr + y);

	//medio vertical
	yr = dim.y + (dim.y + dim.h) / 2;
	drawLine(dim.x - 2 + x, yr + y, dim.x + x, yr + y);

	if (size == 0) return;

	uint16_t lastX, lastY, xr;
	uint8_t stepX = dim.w/(size-1);

	for (size_t i = 0; i < size; i++)
	{
		float value = data[i];
		
		yr = Math::map(
			value, 
			maxValue, minValue, 
			dim.y, dim.y + dim.h
		);
		xr = stepX * i + dim.x;

		if (yr == FLT_MAX) Serial.println("error division por 0");
		
		if (i > 0 && yr != FLT_MAX) 
			drawLine(lastX + x, lastY + y, xr + x, yr + y);

		lastX = xr;
		lastY = yr;

		yr = dim.y + dim.h;
		if(i%2 == 0)
			drawLine(xr + x, yr - 3 + y, xr + x, yr + y);
		/**/
	}
}

char * GraphXY::nameClass()
{
	return "GraphXY";
}

void GraphXY::drawLine(int16_t x, int16_t y, int16_t x1, int16_t y1)
{
	uint8_t w = u8g2.getDisplayWidth();
	uint8_t h = u8g2.getDisplayHeight();
	uint8_t inx = 0;
	uint8_t iny = 0;

	if (x < 0) x = 0;
	else if (x > w) x = w;
	else inx++;

	if (x1 < 0) x1 = 0;
	else if (x1 > w) x1 = w;
	else inx++;

	if (y < 0) y = 0;
	else if (y > y) x = h;
	else iny++;

	if (y1 < 0) y1 = 0;
	else if (y1 > y1) x = h;
	else iny++;

	if (inx > 0 && iny >0) 
		u8g2.drawLine(x, y, x1, y1);
}

float Math::map(float x, float in_min, float in_max, float out_min, float out_max)
{

	float divide = (in_max - in_min) + out_min;
	
	if(divide != 0)
		return (x - in_min) * (out_max - out_min) / divide;

	return FLT_MAX;
}


//tween
void Tween::play(float begin, float finish, float duration)
{
	time = 0;
	this->begin = begin;
	this->change = finish - begin;
	this->duration = duration;
	isRunning = true;
}

void Tween::setEase(float(*ease)(float, float, float, float))
{
	this->ease = ease;
}

float Tween::get()
{
	if (time >= duration) {
		isRunning = false;
		return begin + change;
	}

	return ease(time++, begin, change, duration);
}


float Tween::easeLinear(float t, float b, float c, float d){
	return c * t / d + b;
}
//Cubic
float Tween::easeIn(float t, float b, float c, float d) {
	return c * (t /= d)*t*t + b;
}
float Tween::easeOut(float t, float b, float c, float d) {
	return c * ((t = t / d - 1)*t*t + 1) + b;
}

void Dimension::copy(Dimension dim)
{
	x = dim.x;
	y = dim.y;
	h = dim.h;
	w = dim.w;
}

void Gap::copy(Gap gap)
{
	top = gap.top;
	left = gap.left;
	rigth = gap.rigth;
	button = gap.button;
}
