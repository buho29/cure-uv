// 
// 
// 

#include "Flag.h"


void Flags::testAlarm()
{
	int lunes =		 1;// 0000001
	int martes =	 2;// 0000010
	int miercoles =	 4;// 0000100
	int jueves =	 8;// 0001000
	int viernes =	16;// 0010000
	int sabado =	32;// 0100000
	int domingo =	64;// 1000000

	Flags alarm = Flags(lunes | martes | miercoles | jueves | viernes);

	if (alarm.contain(martes))
		Serial.println("hay alarma el marte");
	if (alarm.check(sabado | domingo))
		Serial.println("hay alarmas el sabado y el domingo");

	if (alarm.contain(sabado | domingo))
		Serial.println("hay alarma el sabado o el domingo o ambas");

	alarm.remove(martes);
	if (!alarm.contain(martes))
		Serial.println("no hay alarma el martes");

	alarm.remove(jueves | domingo);

	if (alarm.check(lunes | miercoles | viernes))
		Serial.println("hay 3 alarmas el lunes|miercoles|viernes");

	alarm.add(domingo);
	if (alarm.contain(domingo))
		Serial.println("toca misa!");
}

Flags::Flags(int flags)
{
	mFlags = flags;
}

void Flags::set(int flags)
{
	mFlags = flags;
}

int Flags::get()
{
	return mFlags;
}

void Flags::reset()
{
	mFlags = 0;
}

void Flags::add(int flags)
{
	mFlags |= flags;
}

void Flags::remove(int flags)
{
	// 1000110 mFlags
	// 0010110 flags
	// 0000110 mFlags & flags , elimina las q no estan encendidas en mFlags 
	// 1000000 0000110 ^ mflags 

	mFlags ^= (mFlags & flags);
}

bool Flags::contain(int flags)
{
	return (mFlags & flags) > 0;
}

bool Flags::check(int flags)
{
	return (mFlags & flags) == flags;
}

