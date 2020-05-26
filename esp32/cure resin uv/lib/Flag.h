// Flag.h

#ifndef _FLAG_h
#define _FLAG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Flags {

private:
	int mFlags = 0;

public:

	static void testAlarm();


	Flags() {}
	Flags(int flags);
	void set(int flags);
	/**
	 *
	 * @return mFlags
	 */
	int get();

	/**
	 * pone mFlags a 0
	 */
	void reset();

	/**
	 * agrega un flags
	 * @param flags
	 */
	void add(int flags);

	/**
	 * elimina un flags
	 * @param flags
	 */
	void remove(int flags);

	/**
	 * comparacion
	 * @param flags
	 * @return devuelve si cuando existe algunos de los flags
	 */
	bool contain(int flags);
	/**
	 * comparacion estricta
	 * @param flags
	 * @return devuelve si cuando existe todos los flags
	 */
	bool check(int flags);
};

#endif

