/* 
* DS3231.h
*
* Created: 26.03.2017 18:25:56
* Author: ARTHUR
*/
#ifndef __DS3231_H__
#define __DS3231_H__
#include <avr/interrupt.h>
#include "../general.h"
class DS3231
{
//variables
public:
//AF - Test int für interrupt
int DS3231_Alarmcounter;
protected:
private:



//functions
public:
	DS3231();
	//functions
	//extern void DS3231_init();
	static uint16_t DS3231_date2days(uint8_t y, uint8_t m, uint8_t d);
	uint8_t DS3231_getdayofweek(uint8_t y, uint8_t m, uint8_t d);
	uint8_t DS3231_setdate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
	void DS3231_getdate(uint8_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second);
	uint8_t DS3231_setalarm(uint8_t minutes);
	//AF - Test Alarm
	void DS3231_getalarm(uint8_t &hour, uint8_t &minute, uint8_t &second);
	void DS3231_enablealarm();
	void DS3231_disablealarm();
	void DS3231_clearalarmflag();
	char* DS3231_getdatetimenow();
protected:
private:
	DS3231( const DS3231 &c );
	DS3231& operator=( const DS3231 &c );

}; //DS3231
extern DS3231 rtc;

#endif //__DS3231_H__
