/*
Name:		Components.h
Created:	29/07/2018 10:30:51
Author:	buho29
*/

#ifndef _COMPONENTS_h
#define _COMPONENTS_h

#define CONTAINER_MAX_SIZE 10
#define VIEW_MAX_SIZE 30
#define TEXT_MAX_SIZE 30
#define TEXTSCROLL_MAX_SIZE 400

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <U8g2lib.h>


#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define FLT_MAX 340282346638528859811704183484516925440.0

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

struct Dimension
{
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
	void copy(Dimension dim);
};

struct Gap
{
	int8_t top;
	int8_t left;
	int8_t rigth;
	int8_t button;
	void copy(Gap gap);
};

enum StatusButton { onRelease,onPress};
enum Align { ALeft, ACenter, ARigth };

enum Font
{
	verySmall = 0,
	small = 1,
	medium = 2,
	large = 3,
	veryLarge = 4,
	icon = 5,
	icon1 = 6,
	icon2 = 7
};

extern char charSymbol[];

enum FontGlyph {
	sun = 0,
	sun_cloud = 1,
	cloud = 2,
	rain = 3,
	thunder = 4,
	snow = 5,
	mist = 6,
	moon = 7,
	wifi_on = 8,
	wifi_off = 9
};


extern const uint8_t *listFonts[];
const uint8_t *getFont(uint8_t font);


class Tween
{
public:

	float begin, duration, change, time;
	bool isRunning = false;

	Tween() {};

	float(*ease)(float, float, float, float) = easeOut;
	void setEase(float(*ease)(float, float, float, float));

	void play(float begin, float finish, float duration = 10);
	float get();
	//Robert penner easing equations
	static float easeLinear(float t, float b, float c, float d);
	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);

};

// base class
class Ui
{
public:
	//constructor
	Ui();
	Ui(Dimension dim);
	// override method
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual void measure();
	virtual char * nameClass();

	//prop
	Dimension dim;
	Gap gap;
	bool selected;
	bool visible = true;
};

class Text : public Ui
{
public:
	Text() :Ui(),align(ALeft) {};
	Text(Dimension dim, const char * txt, Align align = ALeft,Font font = Font::small);
	Text(Dimension dim, Align align = ALeft,Font font = Font::small);
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual void measure();
	virtual char * nameClass();
	virtual void setText(const char * txt);
	virtual char * getText();
	Align align;
	uint8_t  font = 1;
	
protected:
	Dimension textDim;
	char text[TEXT_MAX_SIZE];
};

class Button : public Text
{
public:
	Button() :Text() {};

	Button(Dimension dim, const char * txt, 
		Align align = ACenter, Font font = Font::medium) :
		Text(dim,txt,align,font) {};

	Button(Dimension dim, 
		Align align = ACenter, Font font = Font::medium) :
		Text(dim,align,font) {};

	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
	StatusButton status = onRelease;
};


class Container : public Ui
{
public:
	Container() :Ui() {};
	Container(Dimension dim) :Ui(dim) {};
	void addChild(Ui *child);
	Ui *getChild(int index);
	virtual uint8_t size();
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual void measure();
	virtual char * nameClass();
protected:
	Ui * childs[CONTAINER_MAX_SIZE];
	uint8_t childCount;
};

// abstract class
class View : public Container
{
protected:
	View * viewChilds[VIEW_MAX_SIZE];
	uint8_t viewCount = 0;
	View * currentView = nullptr;
	View * previousView = nullptr;
	uint8_t currentIndex = 0;
	Tween tween;
	bool verticalAnim = true;
	void play(float begin, float finish,bool vertical = true);
public:

	enum Motion { Left, Rigth, Up, Bottom, In, Out};

	View * parentView = nullptr;
	virtual void begin();
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
	void home();
	void next();
	void previous();
	//Views

	// agrega una view a una view
	void addView(View * view);
	View * getView(uint8_t id);
	int16_t getViewId(View * view);
	void removeView(uint8_t id);
	bool existView(View * view);
	void setCurrent(View *view);
	View * getCurrent();
	//input
	virtual bool acceptClickButton(uint8_t id);
	//tween
	bool isPlaying();
	void play(Motion motion, Motion dir);
	void measure();
};

class ManagerView
{
public:
	ManagerView(){};
	void next();
	void previous();
	void begin();
	void add(View *view);
	View * get(uint8_t index);
	View *  getCurrent();
	void setCurrent(View * view);
	uint8_t size();

	//input
	bool acceptClickButton(uint8_t id);

	void draw();
	void start();
	void enableDisplay(bool value);
	bool isEnabledDisplay();
	void stop();
	bool isStarted();
	void setLoopscroll(bool value);
	uint8_t FPS = 1000 / 15;

private:
	View * views[CONTAINER_MAX_SIZE];
	uint8_t currentIndex;
	View * currentView = nullptr;
	View * previousView = nullptr;
	uint8_t viewCount;
	bool anim = false;
	uint16_t animInterval = 5000;
	void resetViews();
	bool isEnabled = true;
	bool loopScroll = true;

};

// rest UI

class Glyph : public Ui
{
private:
	uint8_t getFontGlyph(uint8_t font);
public:
	Glyph() :Ui() {};
	Glyph(Dimension dim, uint8_t fontGlyph = FontGlyph::sun);
	virtual void draw(int16_t x = 0, int16_t y = 0);
	uint8_t fontGlyph;
};

class Color : public Ui
{
private:
	uint8_t color;
public:
	Color(int8_t c = 1) :color(c) {};
	virtual void draw(int16_t x = 0, int16_t y = 0);
};


class ItemList : public Text
{
public:
	ItemList() :Text() {};

	ItemList(Dimension dim, const char * txt,
		Align align = ACenter, Font font = Font::medium) :
		Text(dim, txt, align, font) {};

	ItemList(Dimension dim,
		Align align = ACenter, Font font = Font::medium) :
		Text(dim, align, font) {};

	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
	StatusButton status = onRelease;
};

class List : public Container
{
private:
	ItemList txt1, txt2 ,txt3,txt4;
	char items[CONTAINER_MAX_SIZE][TEXT_MAX_SIZE];
	bool press[CONTAINER_MAX_SIZE];
	uint8_t currentSelected = 0;
	uint8_t itemCount;
	uint8_t selectedId = 0;
	uint8_t posSelected;
	ItemList * uis[CONTAINER_MAX_SIZE] = {
		&txt1, &txt2 ,&txt3,&txt4
	};
public:
	List() :Container() {};
	List(Dimension dim);
	virtual void measure();
	virtual char * nameClass();
	Align align;
	uint8_t size();
	void reset();
	uint8_t addItem(const char * str);
	bool selectItem(uint8_t index,bool press = false);
	int8_t selectedItem();
	bool up();
	bool down();
};

class TextScroll : public Text
{
public:
	TextScroll():Text() {};
	TextScroll(Dimension dim);

	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual void measure();
	virtual char * nameClass();
	virtual void setText(const char * txt);
	virtual char * getText();
	uint8_t vel;
private:
	char text1[TEXTSCROLL_MAX_SIZE];
	int16_t offset;
	uint8_t fontWidth;
};

class Rectangle: public Ui
{
private:
	bool fill;
public:
	Rectangle() :Ui() {};
	Rectangle(Dimension dim,bool fill = false) :Ui(dim),fill(fill) {};
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
};

class RectangleR : public Ui
{
private:
	uint8_t radius;
	bool fill;
public:
	RectangleR() :Ui() {};
	RectangleR(Dimension dim,uint8_t radius =5,bool fill = false) :Ui(dim),
		radius(radius),fill(fill) {};
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
};

class InputNumber : public Text
{
public:
	InputNumber() : Text() {};
	InputNumber(Dimension dim,const char * format);
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
	void plus();
	void minus();
	void setNumber(int8_t n);
	int8_t getNumber();
	void setFormat(const char * format);

	int8_t max;
	int8_t min = 0;
private:
	int8_t number;
	char format[10];
};

class GraphXY :public Ui
{
public:
	GraphXY() :Ui() {};
	GraphXY(Dimension dim);
	void setData(float * data, uint8_t size);
	void setData(float * data,uint8_t size, float maxValue,float minValue);
	virtual void draw(int16_t x = 0, int16_t y = 0);
	virtual char * nameClass();
private:
	float * data;
	uint8_t size = 2;
	float maxValue, minValue;
	static void drawLine(int16_t x, int16_t y,int16_t x1, int16_t y1);
};

class Math
{
public:
	static float map(float x, float in_min, float in_max, float out_min, float out_max);

};

#endif