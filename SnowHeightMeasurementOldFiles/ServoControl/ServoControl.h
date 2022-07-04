/*
 * ServoControl.h
 *
 * Created: 27.02.2017 09:58:07
 *  Author: alen.baric
 */ 


#ifndef SERVOCONTROL_H_
#define SERVOCONTROL_H_
#endif /*SERVOCONTROL_H_ */


//including libraries
#include <avr/io.h>

class ServoControl {

	public: 
		ServoControl();
		uint16_t getAngle1(){return m_angle1;}
		uint16_t getAngle2(){return m_angle2;}
		void setAngle(uint16_t angle1, uint16_t angle2); 

	private:
		uint16_t m_angle1, m_angle2;
		
}; extern ServoControl servoControl;