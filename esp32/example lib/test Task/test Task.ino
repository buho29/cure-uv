/*
 Name:		lib_Task.ino
 Created:	05/08/2018 9:42:56
 Author:	pp
*/



#include <Arduino.h>
#include <U8g2lib.h>
#include <TimeLib.h>
#include "Task.h"
#include "SampleClass.h"


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// oled draw msg
char msg[9];
SampleClass samples[2];

void setup() {

	// Inicio display
	u8g2.begin();
	u8g2.enableUTF8Print();

	Serial.begin(115200);
	delay(10);

	char buff[9];
	/*
	uint32_t t = Tasker::getTickTime(24, 0, 0);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("24:0:0 %s t %d\n",buff , t);
	//24:0:0  0:00:00 t 0

	t = Tasker::getTickTime(23, 60, 0);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("23:60:0 %s t %d\n", buff, t);
	//23:60:0 0:00:00 t 0

	t = Tasker::getTickTime(24, 6, 0);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("24:6:0 %s t %d\n", buff, t);
	//24:6:0 0:06:00 t 360

	t = Tasker::getTickTime(23, 75, 0);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("23:75:0 %s t %d\n", buff, t);
	//23:75:0 0:15:00 t 900

	t = Tasker::getTickTime(23, 57, 60);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("23:57:60 %s t %d\n\n", buff, t);
	//23:57:60 23:58:00 t 86280

	setTime(23, 58, 50, 4, 8, 2018);// h,mi,sc,d,mo,y

	Tasker::tickTimeToChar(buff, Tasker::getTickNow());
	Serial.printf("now %s\n\n", buff);

	//howTimeLeft

	t = Tasker::getTickTime(0, 1, 0);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("0:1:00 %s falta %dsg\n", buff, Tasker::howTimeLeft(t));


	t = Tasker::getTickTime(23, 59, 30);
	Tasker::tickTimeToChar(buff, t);
	Serial.printf("23:59:30 %s falta %dsg\n\n", buff, Tasker::howTimeLeft(t));

	//tasks
	Task * task;

	task = tasker.add(
		Tasker::getTickTime(23, 59, 0),// inicio 
		Tasker::getTickTime(23, 59, 30),// fin 
		&taskCallback // callback
	);
	Serial.printf("id = %d 23:59:00 -> 23:59:30\n",task->id);

	task = tasker.add(Tasker::getTickTime(23, 59, 30), Tasker::getTickTime(0, 1, 0), &taskCallback);
	Serial.printf("id = %d 23:59:30 -> 0:01:30\n",task->id);

	task = tasker.add(Tasker::getTickTime(23, 59, 40), Tasker::getTickTime(23, 60, 30), &taskCallback);
	Serial.printf("id = %d 23:59:40 -> 23:60:30\n",task->id);

	task = tasker.setTimeout(&onTimeOut, 3);
	Serial.printf("id = %d timeOut 23:58:53\n", task->id);

	task = tasker.setTimeout(&onTimeOut1, 5);
	Serial.printf("id = %d timeOut 23:58:55\n", task->id);

	tasker.setInterval(&onSetInterval, 50);*//**/

	for (uint8_t i = 0; i < 2; i++)
	{
		samples[i].setup(0,0,i+10);
	}
	/*
	
	now 23:58:50
	id = 0 23:59:00 -> 23:59:30
	id = 1 23:59:30 -> 0:01:30
	id = 2 23:59:40 -> 23:60:30
	id = 3 timeOut 23:58:53
	id = 4 timeOut 23:58:55
	onTimeOut 23:58:53 now: 23:58:53 3s
	onTimeOut1 23:58:55 now: 23:58:55 5s
	task id:0 Start 23:59:00 -> 23:59:30 now: 23:59:00
	task id:3 Start 23:59:00 -> 23:59:30 now: 23:59:00
	task id:0 Stoped 23:59:00 -> 23:59:30 now: 23:59:30
	task id:3 Stoped 23:59:00 -> 23:59:30 now: 23:59:30
	task id:1 Start 23:59:30 -> 00:01:00 now: 23:59:31
	setInterval 23:59:40 now: 23:59:40 50s
	task id:2 Start 23:59:40 -> 00:00:30 now: 23:59:41
	setInterval 00:00:30 now: 00:00:30 50s
	task id:2 Stoped 23:59:40 -> 00:00:30 now: 00:00:31
	task id:1 Stoped 23:59:30 -> 00:01:00 now: 00:01:01
	setInterval 00:01:20 now: 00:01:20 50s
	setInterval 00:02:10 now: 00:02:10 50s
	*/
}

void taskCallback(Task *current) {
	char buff[9];

	Serial.printf("task id:%d", current->id);

	if (current->runing)
		Serial.print(" Start ");
	else Serial.print(" Stoped ");

	Tasker::tickTimeToChar(buff, current->start);
	Serial.printf("%s -> ", buff);

	Tasker::tickTimeToChar(buff, current->stop);
	Serial.printf("%s now: %s\n", buff, msg);// msg = hora actual
}


void onTimeOut(Task *current) 
{
	char buff[9];
	Tasker::tickTimeToChar(buff, current->start);
	Serial.printf("onTimeOut %s now: %s %ds\n", 
		buff,msg, current->stop - current->start);

	tasker.add(Tasker::getTickTime(23, 59, 0), Tasker::getTickTime(23, 59, 30), &taskCallback);

}


void onTimeOut1(Task *current) 
{
	char buff[9];
	Tasker::tickTimeToChar(buff, current->start);
	Serial.printf("onTimeOut1 %s now: %s %ds\n", 
		buff, msg, current->stop - current->start);
}

void onSetInterval(Task *current) 
{
	char buff[9];
	Tasker::tickTimeToChar(buff, current->start);
	Serial.printf("setInterval %s now: %s %ds\n",
		buff, msg, current->stop - current->start);
}

void loop() {

	Tasker::tickTimeToChar(msg,Tasker::getTickNow());

	tasker.check();

	draw(msg);
}

void draw(const char* str) {
	u8g2.clearBuffer();					// clear the internal memory
	u8g2.setFont(u8g2_font_10x20_me);	// choose a suitable font
	u8g2.drawStr(0, 20, str);	// write something to the internal memory
	u8g2.sendBuffer();
}