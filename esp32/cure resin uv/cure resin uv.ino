

#include "Arduino.h"
#include <Ticker.h> // interupcciones
#include <OneWire.h>
#include <DallasTemperature.h>

#include "lib/Esp32Stepper.h"
#include "lib/AiEsp32RotaryEncoder.h"
#include "lib/Components.h"



//pinout esp32 
//input 36? 37? 38 39 27
//output 2
//in/out 15 0 4 14 12 13 21 23 17 25 33 32
//not work 26

//motor
const int8_t motorPin1 = 14;
const int8_t motorPin2 = 12;
const int8_t motorPin3 = 13;
const int8_t motorPin4 = 21;

//encoder
const int8_t encoderPinA = 18;
const int8_t encoderPinB = 23;
const int8_t encoderPinBtn = 17;

//dac (leds IRLZ44N)
const int8_t dacPin = 25;
//temp (DS18B20)
const int8_t tempPin = 32;

enum Rotary {
	btn,
	rigth,
	left
};

enum Status {
	running,
	stoped,
	paused
};

//motor
StepperEsp stepper1(4076, motorPin1, motorPin2, motorPin3, motorPin4);
//encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(encoderPinA, encoderPinB, encoderPinBtn, -1);
// temp
OneWire oneWireBus(tempPin);
DallasTemperature sensor(&oneWireBus);

//"delays timer"
long tempTime;
long startTime;
long runTime;
long elapsed;

Status status;
// si estamos "dentro" de una pagina
bool enterPage = false;

// variables q se usaran para el run
uint32_t timeValue = 60 * 60;
uint8_t powerValue = 4;
uint8_t rpmValue = 2;

//UI
ManagerView manager;

Font fontTitle = Font::large;
Font fontValue = Font::veryLarge;

Rectangle rect = Rectangle(Dimension{ 0,0,128,64 }, false);

Color c1 = Color(0), c2;
Rectangle rectTitle = Rectangle(Dimension{ 0,0,128,18 }, true);


View pageTime;
Text txtTimeTitle = Text(Dimension{ 0,-3,80,-1 }, "Time", ACenter, fontTitle);
Text txtTimeValue = Text(Dimension{ 0,24,128,-1 },"60 min", ACenter, fontValue);

List timeList = List(Dimension{ 0,0,128,64 });


View pagePower;
Text txtPowerTitle = Text(Dimension{ 0,-3,80,-1 }, "Power", ACenter, fontTitle);
Text txtPowerValue = Text(Dimension{ 0,24,128,-1 },"100%", ACenter, fontValue);

List powerList = List(Dimension{ 0,0,128,64 });


View pageRpm;
Text txtRpmTitle = Text(Dimension{ 0,-3,80,-1 }, "Speed", ACenter, fontTitle);
Text txtRpmValue = Text(Dimension{ 0,24,128,-1 },"2 rpm", ACenter, fontValue);

List rpmList = List(Dimension{ 0,0,128,64 });


View pageReady;
Text txtReadyTitle = Text(Dimension{ 0,-3,80,-1 }, "Ready?", ACenter, fontTitle);
Text txtTemp = Text(Dimension{ 105,-3,30,-1 }, "35°", Align::ARigth, Font::large);
Button btReady = Button(Dimension{32 ,30,62,-1 },"Start", ACenter,Font::large);

Text txtReadyElapsed = Text(Dimension{ 0,20,128,-1 }, "-00:59", Align::ACenter, Font::large);
Button btReadyCancel = Button(Dimension{ 67,42,53,-1 },"Cancel");
Button btReadyPause = Button(Dimension{ 7,42,53,-1 },"Pause");

void updatePageList(Text * txt, List *list, Text * title, uint8_t rot) {

	list->visible = enterPage;
	txt->visible = !enterPage;

	txtTemp.visible = !enterPage;
	c1.visible = !enterPage;
	rectTitle.visible = !enterPage;
	title->visible = !enterPage;

	if (rot == Rotary::rigth && enterPage) {
		list->down();
	}
	else if (rot == Rotary::left && enterPage) {
		list->up();
	}
}

void updatePageReady() {

	if (enterPage) txtReadyTitle.setText("Running");
	else txtReadyTitle.setText("Ready?");

	btReady.visible = !enterPage;
	txtReadyElapsed.visible = enterPage;
	btReadyPause.visible = enterPage;
	btReadyCancel.visible = enterPage;
}

void setText(Text * txt, const char * msg, uint8_t value) {
	char buff[20];
	sprintf(buff, msg, value);
	txt->setText(buff);
}

void setRpm(uint8_t rpm){
	stepper1.SetSpeedRpm(rpm*3);//(72t/24t)
}
void setPower(uint8_t power) {
	// 80 -> 170 (mofset saturation 2.1v)

	dacWrite(dacPin, 150 + (4 * power));
	Serial.printf("dac : %d \n", 150 + (4 * power));
}
void stop() {
	//stop led;
	stepper1.Stop();
	dacWrite(dacPin, 0);
}

void start() {
	setPower(powerValue);
	setRpm(rpmValue);
	stepper1.RotateContinuos(StepperEsp::ccw);
}

void update(uint8_t rot) {

	View * current = manager.getCurrent();

	Serial.println(rot);

	if (!enterPage && rot == Rotary::rigth) {
		manager.next();
		return;
	}
	else if (!enterPage && rot == Rotary::left) {
		manager.previous();
		return;
	}
	else if (!enterPage && rot == Rotary::btn){
		enterPage = true;
		Serial.println("in page");
	}
	else if (enterPage && rot == Rotary::btn){
		enterPage = false;
		Serial.println("out page");
	}
	
	if (current == &pageTime) {
		Serial.println("page Time");
		
		updatePageList(&txtTimeValue, &timeList, &txtTimeTitle, rot);

		if (rot == Rotary::btn && !enterPage) {
			uint8_t index = timeList.selectedItem() +1;
			timeValue = index * 15 * 60;
			setText(&txtTimeValue, "%d min", timeValue/60);
		}
	}
	else if (current == &pagePower) {
		Serial.println("page power");

		updatePageList(&txtPowerValue, &powerList, &txtPowerTitle, rot);

		uint8_t index = powerList.selectedItem() + 1;
		setPower(index);
		if (rot == Rotary::btn && !enterPage) {
			powerValue = 
				index ;
			setText(&txtPowerValue, "%d%%", powerValue * 20);
			stop();
		}

	}
	else if (current == &pageRpm) {
		Serial.println("page rpm");
		
		updatePageList(&txtRpmValue, &rpmList, &txtRpmTitle, rot);
		
		rpmValue = rpmList.selectedItem() + 1;
		setRpm(rpmValue);
		stepper1.RotateContinuos(StepperEsp::ccw);
		if (rot == Rotary::btn && !enterPage) {
			setText(&txtRpmValue, "%d rpm", rpmValue);
			stop();
		}
	}
	else if (current == &pageReady) {
		
		Serial.println("page ready ");

		if (rot == Rotary::rigth || rot == Rotary::left  && enterPage ) {
			btReadyCancel.selected = !btReadyCancel.selected;
			btReadyPause.selected = !btReadyPause.selected;
		}
		else if (rot == Rotary::btn && enterPage) {
			status = Status::running;
			start();
			startTime = millis();
			Serial.println("init");
		}
		else if (rot == Rotary::btn && !enterPage && btReadyCancel.selected) {
			Serial.println("bt canceled");
			status = Status::stoped;
			stop();
		}else if (rot == Rotary::btn && !enterPage && btReadyPause.selected) {
			Serial.println("bt paused");
			
			enterPage = true;

			if (status == Status::paused) {
				status = Status::running;
				start();
				startTime = millis() - elapsed;
			}
			else {
				status = Status::paused;
				stop();

			}

			return;
		}

		updatePageReady();
	}

}


void rotary_loop() {

	ButtonState bt = rotaryEncoder.currentButtonState();
	//first lets handle rotary encoder button click
	if (bt == BUT_RELEASED) {
		update(Rotary::btn);
		//Serial.println("bt");
		return;
	}

	//lets see if anything changed
	int16_t encoderDelta = rotaryEncoder.encoderChanged();

	//optionally we can ignore whenever there is no change
	if (encoderDelta == 0) return;

	//for some cases we only want to know if value is increased or decreased (typically for menu items)
	if (encoderDelta > 0) {
		update(Rotary::rigth);
		//Serial.println("+");
	}
	if (encoderDelta < 0) {
		update(Rotary::left);
		//Serial.println("-");
	}

}

void buildTitle(View * page,Text * title) {
	page->addChild(&rect);
	page->addChild(&rectTitle);
	page->addChild(&c1);
	page->addChild(title);
	page->addChild(&txtTemp);
	page->addChild(&c2);
}

void buildUI (){

	///////////////
	// Time page
	///////////////

	buildTitle(&pageTime,&txtTimeTitle);

	//home
	pageTime.addChild(&txtTimeValue);
	//List
	timeList.visible = false;
	timeList.addItem("15");
	timeList.addItem("30");
	timeList.addItem("45");
	timeList.addItem("60");
	timeList.addItem("75");
	timeList.addItem("90");
	timeList.addItem("105");
	timeList.addItem("120");
	timeList.selectItem(3);
	pageTime.addChild(&timeList);                                                                                                    

	///////////////
	// Power page 
	///////////////

	buildTitle(&pagePower,&txtPowerTitle);

	//home
	pagePower.addChild(&txtPowerValue);
	//List
	powerList.visible = false;
	powerList.addItem("20%");
	powerList.addItem("40%");
	powerList.addItem("60%");
	powerList.addItem("80%");
	powerList.addItem("100%");
	powerList.selectItem(4);
	pagePower.addChild(&powerList);

	///////////////
	// Rpm page 
	///////////////

	buildTitle(&pageRpm,&txtRpmTitle);

	//home
	pageRpm.addChild(&txtRpmValue);
	//List
	rpmList.visible = false;
	rpmList.addItem("1");
	rpmList.addItem("2");
	rpmList.addItem("3");
	rpmList.addItem("4");
	rpmList.addItem("5");
	rpmList.selectItem(1);
	pageRpm.addChild(&rpmList);

	///////////////
	// Ready page
	///////////////

	buildTitle(&pageReady,&txtReadyTitle);

	//home
	btReady.selected = true;
	pageReady.addChild(&btReady);
	//run
	txtReadyElapsed.visible = false;
	btReadyPause.visible = false;
	btReadyCancel.visible = false;
	btReadyPause.selected = true;

	pageReady.addChild(&txtReadyElapsed);
	pageReady.addChild(&btReadyPause);
	pageReady.addChild(&btReadyCancel);

	///////////////
	// Manager
	///////////////
	manager.add(&pageTime);
	manager.add(&pagePower);
	manager.add(&pageRpm);
	manager.add(&pageReady);

	manager.setLoopscroll(false);

	manager.setCurrent(&pageTime);
	manager.begin();
	//manager.draw();
	//manager.start();
}

void setup() {

	Serial.begin(115200);

	//we must initialize rorary encoder 
	rotaryEncoder.begin();
	rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR();});
	//optionally we can set boundaries and if values should cycle or not
	rotaryEncoder.setBoundaries(-100, 100, false); //minValue, maxValue, cycle values (when max go to min and vice versa)
	
	
	steppers.Add(&stepper1);
	steppers.init();

	setRpm(rpmValue);

	sensor.begin();

	buildUI();

}

void temp_loop() {
	if (millis() - tempTime > 2000) {

		tempTime = millis();

		sensor.requestTemperatures();

		float r = sensor.getTempCByIndex(0);

		//Serial.print("Temperatura en sensor 0: ");Serial.print(r);Serial.println(" ºC");
		char buff[20];
		sprintf(buff, "%.1f°", r);
		txtTemp.setText(buff);

	}
}

void timeToChar(char* result, uint32_t time)
{

	uint8_t h, m, s;

	s = time % 60;
	time /= 60;
	m = time % 60;
	time /= 60;
	h = time % 24;

	snprintf(result, 11, "- %d:%02d:%02d", h, m, s);
}

void run_loop() {
	if (manager.getCurrent() != &pageReady || !enterPage) return;
	if (millis() - runTime > 100) {
		runTime = millis();

		switch (status)
		{
		case Status::running:
			elapsed = (millis() - startTime) ;
			if (elapsed / 1000 > timeValue) {
				status = Status::stoped;
				stop();
				enterPage = false;
				updatePageReady();
			}
			else {
				// actualizamos texto
				long rest = timeValue - elapsed/1000;
				char buff[11] = "- 24:00:00";
				timeToChar(buff, rest);
				txtReadyElapsed.setText(buff);
				Serial.println(buff);
			}/*
			Serial.print(millis());
			Serial.print(" - ");
			Serial.print(elapsed);
			Serial.print(" - ");
			Serial.print(startTime);
			Serial.print(" - ");
			Serial.println(timeValue);*/
			break;
		case Status::paused:
			//elapsed = (millis() - startTime) / 1000;
			Serial.println("paused");
			break;
		}

	}
}

void loop() {

	rotary_loop();
	temp_loop();
	run_loop();

	manager.draw();


	
}