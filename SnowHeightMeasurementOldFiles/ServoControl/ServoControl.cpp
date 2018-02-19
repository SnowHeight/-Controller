/*
 * ServoControl.cpp
 *
 * Created: 27.02.2017 10:06:41
 *  Author: alen.baric
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include "ServoControl.h"

ServoControl::ServoControl() 
{

}



void ServoControl::setAngle(uint16_t angle1, uint16_t angle2)
{		
		//Wait Function
		uint16_t i = 0;		
		_delay_loop_2(i);

		//Configure TIMER1
		TCCR1A|=(1<<COM1A1)|(1<<5)|(1<<WGM11);        //NON Inverted PWM
		TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)

		ICR1=39999;  //fPWM=50Hz (Period = 20ms Standard).

		DDRD|=(1<<PD4)|(1<<PD5);   //PWM Pins as Out	
}


