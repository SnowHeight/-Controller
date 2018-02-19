/* 
* Servo.cpp
*
* Created: 06.03.2017 09:00:49
* Author: Baric / Sternbach
* 
*/


#include "Servo.h"
#include "../general.h"
#include "../SDCard/sdcard.h"

#include <avr/wdt.h>
#include "../Eeprom/eeprom.h"
#include "../DistanceLaser/DistanceLaser.h"

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


Servo servo;

// default constructor
/**
 * \brief
 * set the motor duration
 * 
 * \return 
 */
Servo::Servo()
{	
	_drivesstate = ServoDrive::off;

	//ToDo: Wartezeit mit der APP einstellen (NP)
	uint16_t latency = 500;
	
	//duration is set in ms
	_duration = ( uint16_t) ( latency / 20);
	if (_duration < 5) {
		_duration = 5;
	}
	_actDuration = 0;

			//PWM timer
			//Configure TIMER1
			TCCR1A |= (1 << WGM11);										//set fast PWM mode (14) 
			TCCR1B |= (1 << WGM13) | (1 <<WGM12);	  
			TCCR1A |= (1 << COM1A1) | (1 << COM1B1);					//set output behavior for OCN1A and OCN1B (non-inverted mode)

			ICR1 = 39999;												//frequency 50Hz, 16MHZ, F_CPU, prescaler 8 (F_CPU/8/50 - 1)

			ServoPWMDir |= (1 << Servo1PWMOut) | (1 << Servo2PWMOut);	//set PWM out pins

			//initiate the delay timer
			OCR1C = (uint16_t)(((F_CPU/8/50) / 20 * 3) - 1);			//value greater than OCR1A and OCR1B e.g. 3ms = (ICR / 20 * 3) - 1 
			TIMSK1 |= (1 << OCIE1C),
			_state = ServoState::ready;
} //Servo


/**
 * \brief 
 * set drive state to drive and prescaler 8, to start the timer
 * 
 * \return void
 */
void Servo::startPWM()
{
	_drivesstate = ServoDrive::drive;	//set servo state driving (in use)
	TCNT1 = 0;					//sets the counter on 0
	TCCR1B |= (1 << CS11);		//set prescaler to 8 and start timer
}

/**
 * \brief 
 * stops PWM Timer (prescaler off)
 * 
 * \return void
 */
void Servo::stopPWM()
{
	TCCR1B &= ~(( 1 << CS12) | (1 << CS11) | (1 << CS10));			//deletes the bits in the prescaler register
	_drivesstate = ServoDrive::off;									//set servo state off
}

// default destructor
Servo::~Servo()
{
} //~Servo


/**
 * \brief 
 * calculates the correct angles for both servos
 * and lets the servos drive to the calculated positions
 *
 * \param angle1
 * angle * 10 (10° ==> 100) to avoid floats for servo 1
 *
 * \param angle2
 *
 * angle * 10 (34,5° ==> 100) to avoid floats for servo 2
 * \return void
 */
void Servo::move(uint16_t angle1, uint16_t angle2)
{
	//Servo state 
	_state = ServoState::busy;

	//the servo has an angle from 0° to 120°
	//if it goes under 30° we will set the minimum angle to 30°
	//if it goes above 120° we will set the maximum angle to 120°
	//the absolute angle is 150°, because the start value is 0°

	//servo1 angle
	if (angle1 < 300) angle1 = 300;
	else if (angle1 > 1500) angle1 = 1500;

	//servo2 angle
	if (angle2 < 300) angle2 = 300;
	else if (angle2 > 1500) angle2 = 1500;
	
	//Calculation for Servo 1 & 2
	uint16_t _servo1 = ((1200 + angle1 * 2) - 1);
	uint16_t _servo2 = ((1200 + angle2 * 2) - 1);
	
	OCR1A = _servo1;
	rservo1 = _servo1;		//read value for USART

	OCR1B = _servo2;
	rservo2 = _servo2;		//read value for USART
	
	startPWM();
}


/**
 * \brief 
 * Request if the servo is not moving
 * 
 * \return bool
 */
bool Servo::isReady()
{
	if (_drivesstate == ServoDrive::off)
	{
		return true;
	} 
	else 
	{
		return false;
	}
}

/**
 * \brief 
 * Servo state state busy
 * 
 * \return bool if the _state is busy or not
 */
bool Servo::isBusy()
{
	if (_state == ServoState::busy)
	{
		return true;
	} 
	else 
	{
		return false;
	}
}

/**
 * \brief 
 * Interrupt for the Servo-Movement to Measure the Servo Voltage, and try it once again if the voltage level was to high.
 * 
 * \return void
 */
void Servo::Interrupt_Servo()
{
	_actDuration++;
	if (_actDuration >= _duration) 
	{
		_actDuration = 0;
		if (CurrentVoltageMeassure()) 
		{
				
			if (_drivesstate == ServoDrive::extradrive) 
			{
			
				_state = ServoState::error;
				stopPWM();
			}
			else 
			{

				_drivesstate = ServoDrive::extradrive;
				rdrivestate = "extradrive";
			}
		}
		else 
		{
			stopPWM();
			_state = ServoState::ready;
		}		
	}

	//Error Log
	_errorlog = ErrorState::noError;
	ErrorLog();
}

//error if extradrive is triggered
bool Servo::isError()
{
	if (_state == ServoState::error) {

		//ErrorLog Servo
		_errorlog = ErrorState::errorDrive;
		ErrorLog();

		return true;
	} 
	else
	{

		_errorlog = ErrorState::noError;
		ErrorLog();

		return false;
	}
}

/**
 * \brief 
 * activate the analog digital converter and set the prescaler
 * ADEN = activate ADC
 * ADPS0-2 = set prescaler to 128
 * ADMUX = reference voltage AVCC with external capacitor at AREF pin
 * 
 * \return void
 */
void Servo::activateADC()
{
	ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = (1 << REFS0);
	ADCSRA |= (1<<ADSC);				//start the dummy conversion
	while(ADCSRA & (1<<ADSC));
	(void)ADC;
}

/**
 * \brief 
 * measure the power via analog to digital converter for the Servo1 & Servo2
 * 
 * \return bool
 */
bool Servo::CurrentVoltageMeassure()
{		
	/*ADCSRA start the dummy conversion
	* wait until the conversion has finished (is the Bit set?)
	* read the value and commit it to the _currentS1/_currentS2 variable
	*/

	//set ADC2: mux1 is set --> delete mux0 to activate mux1
	ADMUX &= ~((1 << MUX1) | (1 << MUX0));
		
	//start the conversion
	ADCSRA |= (1<<ADSC); 

	//wait for the result, than read it and set the Servo1 value
	while(ADCSRA & (1<<ADSC));
	_currentS1 = ADC;

	//set the adc2 channel
	ADMUX |= (1 << MUX0);

	//start the conversation
	ADCSRA |= (1<<ADSC);

	//wait for the result, than read it and set the Servo2 value
	while(ADCSRA & (1<<ADSC));
	_currentS2 = ADC;

	//USARt output values
	rADC1 = _currentS1;
	rADC2 = _currentS2;

	
	//Servo1 and Servo2 have to be under our max value, otherwise they won't move further
	if (_currentS1 >= maxCurrentADC || _currentS2 >= maxCurrentADC)
	{
		//ErrorLog Voltage
		_errorlog = ErrorState::errorVoltage;
		ErrorLog();

		return true;

	} else { 

		//no Error
		_errorlog = ErrorState::noError;
		ErrorLog();

		return false; 
	}
}

/**
 * \brief 
 * measure the battery value, so that we only drain energy for the servos, when it has enough power.
 * otherwise set a break
 * \return bool
 * if
 */
bool Servo::BatteryOK()
{
	/*ADCSRA start the dummy conversion
	* wait until the conversion has finished (is the Bit set?)
	* read the value and commit it to the _currentS1/_currentS2 variable
	*/

	ServoPWMDir |= (1 << BatteryMeasureOut);	//set PWM out pin for the battery measure

	//switch adc2 mux 0
	ADMUX &= ~(1 << MUX0);

	//set ADC2 Port
	ADMUX |= (1 << MUX1);

	ADCSRA |= (1<<ADSC);

	//wait for the result, than read it.
	while(ADCSRA & (1<<ADSC));

	uint16_t adcResult = ADC;
	uint8_t count = 1;

	//set the first measured ADC value
	_batteryMeasure2 = adcResult;

	//measure the battery until the values battery-voltage-values are equal
	do 
	{
		count++;
		
		//max 10 measures to calculate the average voltage-value 
		if (count >= 10)
		{
			break;	//ends execution of the nearest enclosing loop
		}

		//start the conversation
		ADCSRA |= (1<<ADSC);

		//wait for the result, than read it.
		while(ADCSRA & (1<<ADSC));

		_batteryMeasure1 = _batteryMeasure2;		
		_batteryMeasure2 = ADC;						//set the next measured ADC value

	} while ((_batteryMeasure1 & Mask) != (_batteryMeasure2 & Mask));

	_batterySteps = _batteryMeasure2;

	_batteryMeasure2 = (uint16_t)(((uint32_t)_batteryMeasure2 * (uint32_t)maxBatteryValue + (uint32_t)50000) / 100000);
	_batteryADC = _batteryMeasure2;

	ServoPWMDir &= ~(1 << BatteryMeasureOut);	//delete PWM out pin for the battery measure

	if (_batteryADC <= _minBatteryVoltage)
	{	
		//ErrorLog Battery
		_errorlog = ErrorState::noError;
		ErrorLog();

		return true;
	}
	else
	{	
		_errorlog = ErrorState::errorBattery;
		ErrorLog();

		return false;
	}
}

/**
 * \brief 
 * commits the error string to the sd-card-function, depending on the error-state
 * String-Build: "<<Module name>>; <<Error text>>"
 * \return void
 */
void Servo::ErrorLog()
{	
	
	if (_errorlog == ErrorState::errorBattery)
	{
		_ErrorTxt = "ERROR Battery";

		//Write the string to the SD-Card
		ErrorData writeError;
		writeError.writeData("Servo; error battery");
	}
	else if ( _errorlog == ErrorState::errorDrive)
	{
		_ErrorTxt = "ERROR Drive";

		//Write the string to the SD-Card
		ErrorData writeError;
		writeError.writeData("Servo; error drive");
	}
	else if ( _errorlog == ErrorState::errorVoltage)
	{
		_ErrorTxt = "ERROR Voltage";

		//Write the string to the SD-Card
		ErrorData writeError;
		writeError.writeData("Servo; error voltage");
	}
	else
	{
		_ErrorTxt = "no ERROR";
	}
}

bool Servo::LaserMeasure()
{
	//AF - Servocutoff Pin einschalten.
	CutoffPort |= (1 << ServoCutoff);	//turn Servo Cutoff Pin on

	bool returnOK = true;
	//SD-Card-Log-Counter
	int _counterMeasure = 1;

	//initialize analog digital converter
	servo.activateADC();
	
	//measure the battery capacity
	//returns only if the battery is OK or not
	servo.BatteryOK();

	//starts the Servo-Movement only if the battery check is true
	if(servo.BatteryOK())
	{

		//test Start values
		uint16_t servo1 = 1000;
		uint16_t servo2 = 300;
		uint16_t step1 = 20;
		uint16_t step2 = 30;

		/*Servo movement 
		* x = servo 1 coordinates
		* y = servo 2 coodrinates
		*
		*/
		uint16_t x;
		uint16_t y;


		/***************** Servo movement *********************
		*  Starts the movement if coordinates x,y are available and starts the laser measurement if no error occurs
		*******************************************************
		** Parameters: x, y
		**************
		* x = servo1 angle
		* y = servo2 angle
		*/


		//
		EEPROM oEeprom;
		bool bDatastream = oEeprom.GotoFirstValue();


		while(bDatastream)
		{	
			//reset the Watchdog-Timer
			wdt_reset();

			x = oEeprom.GetValueX();
			y = oEeprom.GetValueY();

			if (x > 1500) x = 300;
			if (y > 1500) y = 300;
			//ToDO: Watchdog: f?r die Messungen zur?cksetzen

			//Request if the servo-drive is on 
			while(!servo.isReady());		// ! = not - if the servo is not ready

			//Servo 1 moves to angle x
			//Servo 2 moves to angle y
			servo.move(x, y);

			//is the servo moving?
			while(servo.isBusy());

			/*Error Sequence
			*if no Error occurs, we call the Laser-request function and in return we get the average measured value.
			*After that we transfer the value to the sd-card
			*/
			if (!servo.isError())
			{	
				
				//Turn on the Leasure-Measurement
				//I2C-Address = 0x62
				DistanceLaser distanceLaser;
				uint16_t measuredDistance;

				//build the string. so we can commit the measure number and average laser-distance to the sd-card function
				//memory allocate, 4 byte
				char* strMeasurements = (char*)malloc(4);
				strcat(strMeasurements, (char*) _counterMeasure);
				strcat(strMeasurements, ";");
				strcat(strMeasurements, (char*) measuredDistance);

				//Laser-Measure-Points function
				LaserData laserData;
				laserData.writeData(strMeasurements);

				//free the allocate memory again
				free(strMeasurements);

				//get the next x,y value for the measurement
				measuredDistance = distanceLaser.getDistance();

			}
			else
			{
				//exit the loop when an error occurs in the servo movement
				returnOK = false;
				break;
			}

			//gets the servo x, y angle from the EEPROM
			bDatastream = oEeprom.GotoNextValue();

			//SD-Card-Log-Timer
			_counterMeasure++;

		}//end while servo-movement

	}
	//error has occurred
	else
	{
		returnOK = false;
	}
	//AF - Servocutoff Pin ausschalten.
	CutoffPort &= ~(1 << ServoCutoff);	//turn servo Cutoff Pin off

	return returnOK;
}//end Main

//ToDo: Leerlauf erstellen


