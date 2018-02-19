/* 
* DS3231.cpp
*
* Created: 26.03.2017 18:25:56
* Author: ARTHUR
*/

#include "DS3231.h"
//AF - Includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../general.h"
#include "util/delay.h"
#include <stdio.h>

extern "C" {
	#include "../I2C_MASTER/i2c_master.h"
};

DS3231 rtc;

/**
 * \brief 
 * initialize the accellerometer
 * enable the external interrupt
 * \return void
 */
// default constructor
DS3231::DS3231()
{
	#if DS3231_I2CINIT == 1
		//init i2c
		i2c_init();
		_delay_us(10);
	#endif
	//AF - PullUp
	PORTD |= (1 << PD2);
	//AF  - Turn on interrupt
	DS3231_EnableRisingEdge();
	DS3231_EnableExtInterrupt();
} //DS3231

/*
 * days per month
 */
int DS3231_itest = 0;
const uint8_t DS3231_daysinmonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
/**
 * \brief 
 * transform decimal value to bcd
 * \param val
 * val is an uint8_t number, range = 0 - 255
 * \return uint8_t
 */
uint8_t DS3231_dec2bcd(uint8_t val) {
	return val + 6 * (val / 10);
}
/**
 * \brief 
 * transform bcd value to decimal
 * \param val
 * val is an uint8_t number, range = 0 - 255
 * \return uint8_t
 */
static uint8_t DS3231_bcd2dec(uint8_t val) {
	return val - 6 * (val >> 4);
}

/**
 * \brief 
 * get number of days since 2000/01/01 (valid for 2001..2099)
 * \param y
 * parameter for the year as uint8_t range = 0 - 255
 * \param m
 * parameter for the month as uint8_t range = 0 - 255
 * \param d
 * parameter for the day as uint8_t range = 0 - 255
 * \return uint16_t
 */
uint16_t DS3231::DS3231_date2days(uint8_t y, uint8_t m, uint8_t d) {
	uint16_t days = d;
	for (uint8_t i = 1; i < m; ++i)
		days += pgm_read_byte(DS3231_daysinmonth + i - 1);
	if (m > 2 && y % 4 == 0)
		++days;
	return days + 365 * y + (y + 3) / 4 - 1;
}

/**
 * \brief 
 * get day of a week
 * \param y
 * parameter for the year as uint8_t, range = 0 - 255
 * \param m
 * parameter for the month as uint8_t, range = 0 - 255
 * \param d
 * parameter for the day as uint8_t, range = 0 - 255
 * \return uint8_t
 */
uint8_t DS3231::DS3231_getdayofweek(uint8_t y, uint8_t m, uint8_t d) {
	uint16_t day = DS3231_date2days(y, m, d);
	return (day + 6) % 7;
}
/**
 * \brief 
 * sets the date and the time of the real time clock
 * \param year
 * uint8_t, range = 0 - 255
 * \param month
 * uint8_t, range = 0 - 255
 * \param day
 * uint8_t, range = 0 - 255
 * \param hour
 * uint8_t, range = 0 - 255
 * \param minute
 * uint8_t, range = 0 - 255
 * \param second
 * uint8_t, range = 0 - 255
 * \return uint8_t
 */
uint8_t DS3231::DS3231_setdate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	//sanitize data
	if (second < 0 || second > 59 ||
	minute < 0 || minute > 59 ||
	hour < 0 || hour > 23 ||
	day < 1 || day > 31 ||
	month < 1 || month > 12 ||
	year < 0 || year > 99)
	return 8;
	//sanitize day based on month
	if(day > pgm_read_byte(DS3231_daysinmonth + month - 1))
	return 0;
	//get day of week
	uint8_t dayofweek = DS3231_getdayofweek(year, month, day);
	//write date
	i2c_start(DS3231_ADDR + I2C_WRITE);
	i2c_write(0x00);//stop oscillator
	i2c_write(DS3231_dec2bcd(second));
	i2c_write(DS3231_dec2bcd(minute));
	i2c_write(DS3231_dec2bcd(hour));
	i2c_write(DS3231_dec2bcd(dayofweek));
	i2c_write(DS3231_dec2bcd(day));
	i2c_write(DS3231_dec2bcd(month));
	i2c_write(DS3231_dec2bcd(year));
	i2c_write(0x00); //start oscillator
	i2c_stop();
	return 1;
}
/**
 * \brief 
 * get the current set date and time from the real time clock
 * \param year
 * uint8_t, range = 0 - 255
 * \param month
 * uint8_t, range = 0 - 255
 * \param day
 * uint8_t, range = 0 - 255
 * \param hour
 * uint8_t, range = 0 - 255
 * \param minute
 * uint8_t, range = 0 - 255
 * \param second
 * uint8_t, range = 0 - 255
 * \return void
 */
void DS3231::DS3231_getdate(uint8_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second) {
	i2c_start(DS3231_ADDR + I2C_WRITE);
	i2c_write(0x00);//stop oscillator
	i2c_stop();
	i2c_start(DS3231_ADDR + I2C_READ); //| I2C_READ);
	second = DS3231_bcd2dec(i2c_read_ack() & 0x7F);
	minute = DS3231_bcd2dec(i2c_read_ack());
	hour = DS3231_bcd2dec(i2c_read_ack());
	i2c_read_ack();
	day = DS3231_bcd2dec(i2c_read_ack());
	month = DS3231_bcd2dec(i2c_read_ack());
	year = DS3231_bcd2dec(i2c_read_nack());
	i2c_stop();	
}

/**
 * \brief 
 * sets the alarm of the real time clock
 * gets the current time and adds the minutes parameter to set the next alarm
 * Seconds are always set to 0 
 * \param minutes
 * uint8_t, range = 0 - 255
 * defines in how many minutes the alarm is set
 * \return uint8_t
 */
uint8_t DS3231::DS3231_setalarm(uint8_t minutes) {
	//add alarm to current date and insert next alarm time.
	uint8_t year = 0;
	uint8_t month = 0;
	uint8_t day = 0;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t second = 0;
	DS3231_getdate(year, month, day, hour, minute, second);
	minute += minutes;					//add minutes
	if (minute > 59)					//treatment for overflow
	{
		minute -= 60;		
		hour += 1;
		if (hour > 23)				
		{
			hour -= 24;
			day += 1;					//can be extended at this point
		}
	}
	second = 0;							//set seconds to 0 
	//write date
	i2c_start(DS3231_ADDR + I2C_WRITE);
	i2c_write(0x07);					//stop oscillator
	//Alarm
	i2c_write(DS3231_dec2bcd(second));	//Seconds 
	i2c_write(DS3231_dec2bcd(minute));	//Minutes 
	i2c_write(DS3231_dec2bcd(hour));	//Hour
	i2c_write(0b10000000);				//Day/Date
	//i2c_write(0x07);					//start oscillator
	i2c_stop();
	return 1;
}
/*
 * get alarm 1
 */
/**
 * \brief 
 * returns the time of the alarm
 * \param hour
 * uint8_t, range = 0 - 255
 * \param minute
 * uint8_t, range = 0 - 255
 * \param second
 * uint8_t, range = 0 - 255
 * \return void
 */
void DS3231::DS3231_getalarm(uint8_t &hour, uint8_t &minute, uint8_t &second) {
	i2c_start(DS3231_ADDR + I2C_WRITE);
	i2c_write(0x07);									//stop oscillator
	i2c_stop();
	i2c_start(DS3231_ADDR + I2C_READ);					//| I2C_READ);
	second = DS3231_bcd2dec(i2c_read_ack() & 0x7F);
	minute = DS3231_bcd2dec(i2c_read_ack() & 0x7F);
	hour = DS3231_bcd2dec(i2c_read_nack() & 0x7F);
	i2c_stop();
}

 void DS3231::DS3231_enablealarm() {
	 uint8_t alarm = 0;
	 //Read bits from control register 0Eh
	 i2c_start(DS3231_ADDR + I2C_WRITE);
	 i2c_write(0x0E);									//stop oscillator
	 i2c_stop();
	 i2c_start(DS3231_ADDR + I2C_READ);					//| I2C_READ);
	 alarm = i2c_read_nack();
	 i2c_stop(); 
	 if (alarm > 0)
	 {
		i2c_start(DS3231_ADDR + I2C_WRITE);
		i2c_write(0x0E);//stop oscillator
		i2c_write(alarm |= (1 << DS3231_Alarm1Bit));
		i2c_write(0x0E); //start oscillator
		i2c_stop();
	 } 
 }

 void DS3231::DS3231_disablealarm() {
	 uint8_t alarm = 0;
	 //Read bits from control register 0Eh
	 i2c_start(DS3231_ADDR + I2C_WRITE);
	 i2c_write(0x0E);									//stop oscillator
	 i2c_stop();
	 i2c_start(DS3231_ADDR + I2C_READ);					//| I2C_READ);
	 alarm = i2c_read_nack();
	 i2c_stop();
	 if (alarm > 0)
	 {
		 i2c_start(DS3231_ADDR + I2C_WRITE);
		 i2c_write(0x0E);								//stop oscillator
		 i2c_write(alarm &= ~(1 << DS3231_Alarm1Bit));
		 i2c_write(0x0E);								//start oscillator
		 i2c_stop();
	 }
 }
  void DS3231::DS3231_clearalarmflag() {
	  uint8_t alarm = 0;
	  //Read bits from control register 0Eh
	  i2c_start(DS3231_ADDR + I2C_WRITE);
	  i2c_write(0x0F);									//stop oscillator
	  i2c_stop();
	  i2c_start(DS3231_ADDR + I2C_READ);				//| I2C_READ);
	  alarm = i2c_read_nack();
	  i2c_stop();
	  i2c_start(DS3231_ADDR + I2C_WRITE);
	  i2c_write(0x0F);									//stop oscillator
	  i2c_write(alarm &= ~(1 << DS3231_Alarm1Flag));
	  i2c_write(0x0F);									//start oscillator
	  i2c_stop();
  }

  char* DS3231::DS3231_getdatetimenow()
  {
  //AF - Return the RTC Date as string
	  uint8_t year = 0;
	  uint8_t month = 0;
	  uint8_t day = 0;
	  uint8_t hour = 0;
	  uint8_t minute = 0;
	  uint8_t second = 0;
	  rtc.DS3231_getdate(year, month, day, hour, minute, second);
	  char Currenttime[50];

	  Currenttime[0] = (char)day;
	  Currenttime[1] = '.';
	  Currenttime[2] = (char)month;
	  Currenttime[3] = '.';
	  Currenttime[4] = (char)year;
	  Currenttime[5] = ' ';
	  Currenttime[6] = (char)hour;
	  Currenttime[7] = ':';
	  Currenttime[8] = (char)minute;
	  Currenttime[9] = ':';
	  Currenttime[10] = (char)second;

	  //sprintf(Currenttime,"%d.%d.%d %d:%d:%d", year, month, day, hour, minute, second);
	  return Currenttime;
  }