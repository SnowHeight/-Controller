/* 
* Servo.h
* Include file
* Created: 06.03.2017 09:00:49
* Author: Baric / Sternbach
* 
*/

#include <avr/io.h>
#include "../general.h"	

#ifndef __SERVO_H__
#define __SERVO_H__	

//List of states
enum class ServoState
{
	 busy,
	 ready,
	 error
};

//List of drive States
enum class ServoDrive
{
	off,
	drive,
	extradrive
};

//List of Error States
enum class ErrorState
{
	noError,		//0
	errorDrive,		//1
	errorBattery,	//2
	errorVoltage	//3
};

class Servo
{
	//variables
	public:
			uint16_t rservo1;				//read servo OCR1n value
			uint16_t rservo2;				//read servo OCR1n value
			uint16_t rADC1;					//read servo ADC1 value
			uint16_t rADC2;					//read servo ADC2 value
			char* rCM;						//USART
			char* rstate;					//USART
			char* rdrivestate;				//USART
			char* rErr;						//USART
			ServoState	_state;				//Servo State enum value
			uint16_t	_batteryMeasure1;	//Battery measure 1 value
			uint16_t	_batteryMeasure2;	//Battery measure 2 value
			uint16_t	_minBatteryVoltage;	//min voltage value (delivered from the EEPROM)
			uint16_t	_batteryADC;		//Battery voltage value
			ErrorState	_errorlog;			//Error Log enum value
			char*		_ErrorTxt;			//Error Log File message
			uint16_t	_batterySteps;
			bool LaserMeasure();
		
	protected:
	private:
		ServoDrive	_drivesstate;			//Servo drive state
		//ServoState	_state;				//Servo state
		uint8_t		_duration;				//Comparison Value from the EEPROM
		uint8_t		_actDuration;			//Count Value
		///uint16_t	_batteryADC;			//battery analog digitac converter value
		uint16_t	_currentS1;				//Servo 1 angle value
		uint16_t	_currentS2;				//Servo 2 angle value


	//functions
	public:
		Servo();
		~Servo();
		void activateADC();					//turn the analog digital conver on
		void move (uint16_t angle1, uint16_t angle2);	//servo move function, with the transfer parameter angle1 (servo1) & angle2 (servo2)
		bool isReady();						//servo ready check, returns true or false
		void Interrupt_Servo();				//interrupt sequence
		bool isError();						//servo drive error check, returns true or false
		bool isBusy();						//servo busy check, returns true or false
		void clearError();					//error log clear
		void ErrorLog();					//defines the error chars, based on the error
		bool BatteryOK();					//battery check 

	protected:

	private:
		Servo( const Servo &c );			
		Servo& operator=( const Servo &c );
		void startPWM();					//start the pulse wide modulation
		void stopPWM();						//stop pulse wide modulation
		bool CurrentVoltageMeassure();		//voltage measure for servo1 & servo2

}; extern Servo servo; //Servo

#endif //__SERVO_H__
