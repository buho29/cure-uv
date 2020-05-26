/*
 Name:		lib_component.ino
 Created:	05/08/2018 9:50:31
 Author:	pp
*/

#include "Components.h"


Text txt = Text(Dimension{ 0,0,128,-1 },"text");
Text txt1 = Text(Dimension{ 0,20,128,-1 },"text 1");
Button btn = Button(Dimension{ 34,40,60,-1 },"button !");
View page;

Text txt2 = Text(Dimension{ 0,32,128,-1 });
Glyph glyph = Glyph(Dimension{95,30,-1,-1 }, FontGlyph::cloud);
View page1;


List list = List(Dimension{ 0,0,128,64});
View page2;

TextScroll ts = TextScroll(Dimension{0,64,128,-1});
TextScroll ts1 = TextScroll(Dimension{ 24,32,80,-1 });
Rectangle b = Rectangle(Dimension{ 24-6,32-12,80+6+6,16});
View page3;

InputNumber h = InputNumber(Dimension{ 20,30,-1,-1 },"%02d");
Text sep = Text(Dimension{20+7*2,30,-1,-1 },":");
InputNumber m = InputNumber(Dimension{20+7*3,30,-1,-1 },"%02d");
Text sep1 = Text(Dimension{ 20+7*5,30,-1,-1 },":");
InputNumber s = InputNumber(Dimension{ 20+7*6,30,-1,-1 },"%02d");
View page4;
GraphXY graf = GraphXY(Dimension{ 30,0,128-31,64 });

const uint8_t length = 24;
float data[length] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
View page5;

ManagerView manager;

const char * msg= "5:00:00 Miercoles 8 Agosto 2018 Tormentas. Nubosidad 63%";

void setup() {

	Serial.begin(115200);

	// Page 1

	btn.selected = true;
	txt1.selected = true;
	
	page.addChild(&txt);
	page.addChild(&txt1);
	page.addChild(&btn);

	// page 2

	txt2.font = Font::veryLarge;
	txt2.setText("Tmp 29/13°");
	page1.addChild(&txt2);
	page1.addChild(&glyph);

	// page 3
	//List
	uint8_t index = list.addItem("item1");
	list.addItem("item2");
	list.addItem("item3");
	list.addItem("item4");
	list.addItem("item5");
	list.addItem("item6");
	list.addItem("item7");
	
	list.selectItem(index);

	page2.addChild(&list);

	//page 4
	//scroll Text
	ts.setText(msg);
	ts1.setText(msg);
	page3.addChild(&b);
	page3.addChild(&ts);
	page3.addChild(&ts1);

	//page5
	h.max = 24;
	m.max = 60;
	s.max = 60;

	h.setNumber(19);
	m.setNumber(5);
	s.setNumber(8);

	s.selected = true;

	page4.addChild(&h);
	page4.addChild(&sep);
	page4.addChild(&m);
	page4.addChild(&sep1);
	page4.addChild(&s);

	//page6
	graf.setData(data, length,23,0);
	page5.addChild(&graf);

	manager.add(&page);
	manager.add(&page1);
	manager.add(&page2);
	manager.add(&page3);
	manager.add(&page4);
	manager.add(&page5);
	
	manager.setCurrent(&page5);

	manager.begin();
	manager.start();
	

	Tween anim;
	float frames = 10;
	anim.play(0, 100, frames);
	for (uint8_t i = 0; i < frames; i++)
	{
		float x = anim.get();
		Serial.printf("x = %f\n", x);
		if (!anim.isRunning) Serial.println("end anim");
	}

	/*
	// borrar test
	const uint8_t maxSize = 10;
	char zoneNames[maxSize][6 + 1];
	//vaciamos
	for (uint8_t i = 0; i < maxSize; i++)
	{
		strncpy(zoneNames[i], "", 6);
	}

	strncpy(zoneNames[0], "teta1", 6);
	strncpy(zoneNames[1], "teta2", 6);
	strncpy(zoneNames[2], "teta3", 6);
	strncpy(zoneNames[3], "teta4", 6);
	strncpy(zoneNames[4], "teta5", 6);


	uint16_t size = -1;
	for (; ++size < maxSize;)
	{
		if (strcmp(zoneNames[size], "") == 0) break;
	}
	
	Serial.println("Print array");
	for (uint8_t i = 0; i < maxSize; i++)
	{
		Serial.printf("i:%d %s ,",i,zoneNames[i]);
	}
	Serial.println("end Print array");


	Serial.printf("size:%d\n",size);
	
	// borrar index = 2
	uint8_t index = 2;
	//desplazamos los de delante
	for (uint8_t i = index; i < size - 1; i++)
	{
		strncpy(zoneNames[i], zoneNames[i+1], 6);
	}
	//borramos el ultimo
	strncpy(zoneNames[size-1], "", 6);

	Serial.println("Print array");
	for (uint8_t i = 0; i < maxSize; i++)
	{
		Serial.printf("i:%d %s ,",i,zoneNames[i]);
	}
	Serial.println("\nend Print array");
	*/
}

int count;
long currentTime;
long currentTime2;
bool dir = false;

void loop() {

	/*if (!page.isPlaying()) 
	{
		if (dir)
			page.play(View::Rigth,View::Out);
		else
			page.play(View::Left,View::In);
		dir = !dir;
	}

	manager.draw();
	
	return;*/

	// "simulate" user interaction 
	if (millis() - currentTime > 1000) {

		View * current = manager.getCurrent();

		if (current == &page)
		{
			txt.setText("pjTPcount: °");

			char buf[6];
			sprintf(buf, "%d", count++);
			txt1.setText(buf);

			if (btn.status == StatusButton::onRelease)
				btn.status = StatusButton::onPress;
			else
				btn.status = StatusButton::onRelease;

			txt.selected = !txt.selected;
			btn.selected = !btn.selected;/**/

			Serial.println("page 1");
		}
		else if (current == &page1) {

			txt2.selected = !txt2.selected;
			Serial.println("page 2");
		}
		else if (current == &page2) {

			static bool dir = true;
			if (dir && !list.down()) dir = false;
			else if (!dir && !list.up()) dir = true;
			Serial.println("page 3");

		}
		else if (current == &page3) {
			//scroll text
		}
		else if(current == &page4){
			s.plus();
		}
		currentTime = millis();
	}

	uint32_t current = millis();
	manager.draw();
	//Serial.printf("draw %d\n", millis() - current);
}