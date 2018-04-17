/* 
* usonic.h
*
* Created: 07.03.2017 08:51:07
* Author: cmellauner
*/


#ifndef __USONIC_H__
#define __USONIC_H__

#include <avr/interrupt.h>
#include "../general.h"
#include "../configuration.h"

class usonic
{
//variables
public:
protected:
private:
	bool _ready;
	uint8_t _overflows;
	uint16_t _distance;

//functions
public:
	usonic();

	static uint16_t convertDistance(uint32_t ticks, uint16_t tempAir);

	void startMeasurement();

	//Getter and Setter functions
	bool isReady() { return _ready; }
	void setReady(bool ready) { _ready = ready; }

	void setDistance(uint16_t millimeter) { _distance = millimeter; }
	uint16_t getAverageDistance();
	uint16_t getDistance() { return _distance; }
		
	void setOverflows(uint8_t overflows) { _overflows = overflows; }
	uint8_t getOverflows() { return _overflows; }

private:
	void bubbleSort(uint16_t (&distanceArray)[10]);
}; //usonic

extern usonic uSonic;

#endif //__USONIC_H__

