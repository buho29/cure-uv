#include "SampleClass.h"                                                                                                                                                                                                                                                                                                         

void SampleClass::onSetInterval(Task * t)
{
	char buff[9];
	Tasker::tickTimeToChar(buff, t->start);
	Serial.printf("classe :%d %s %dsc %d\n",t->id ,buff,seconds ,t->runing);
}

void SampleClass::setup(uint16_t s)
{
	seconds = s;

	using namespace std::placeholders;

	task = tasker.setInterval(
		std::bind(&SampleClass::onSetInterval, this, _1), s);
}

void SampleClass::setup(uint16_t h, uint16_t m, uint16_t s)
{
	hours = h;
	minutes = m;
	seconds = s;


	using namespace std::placeholders;

	task = tasker.setInterval(
		std::bind(&SampleClass::onSetInterval, this, _1), h,m,s);

}
