/** \file usonic.cpp
 * Interface for using the ultrasonic sensor
 *
 * \author cmellauner, mplankensteiner
 * \version 1.0
 * \date 2017
 */
#include "usonic.h"
#include <util/delay.h>
#include "../usart/usart.h"
#include "../configuration.h"

SnowHeight::Interface::Usart usart;
usonic uSonic;

usonic::usonic() :
	_ready(true),
	_overflows(0),
	_distance(0)
{
	
	// Pin Definitions
	USONIC_DIR |= (1 << USONIC_TRIGGER_PIN);
	USONIC_PORT &= ~(1 << USONIC_TRIGGER_PIN);
	
	// init Timer
	USONIC_TIMSK |= (1 << USONIC_TOVF);
	USONIC_TRIGGER_IMPULS = 180;
	
	// init Ext. Interrupts
	USONIC_ECHO_EICR |= (1 << USONIC_INTERRUPT); // rising and falling edge sense

	// AF- USart zum debuggen
	
}

void usonic::bubbleSort(uint16_t (&distanceArray)[10])
{
	bool bSorted = false;
	while (bSorted == false) 
	{
		bSorted = true;
		for (int i = 0; i < 10; ++i)
		{
			uint16_t distance1 = distanceArray[i];
			uint16_t distance2 = distanceArray[i + 1];
			if (distance1 > distance2)
			{
				distanceArray[i] = distance2;
				distanceArray[i + 1] = distance1;
				bSorted = false;
			}
		}
	}
}

uint16_t usonic::getAverageDistance()
{	
	USONIC_TIMSK |= (1 << USONIC_OCRA);
	
	//Take 10 measurements and save them into an Array
	uint16_t measureDistances[10];
	for (uint8_t i = 0; i < 10; i++)
	{			
		
		startMeasurement();		
		while(!_ready) {	
			_delay_us(10);
		}	
		int distance = getDistance();
		measureDistances[i] = distance;
	}
	USONIC_TIMSK &= ~(1 << USONIC_OCRA);
	
	for (uint8_t i = 0; i < 10; i++)
	{
		usart.Transmit("|start Mesurment|=>");
		usart.Transmit((long)measureDistances[i]);
	}
	
	//Sort the Values
	bubbleSort(measureDistances);
	
		for (uint8_t i = 0; i < 10; i++)
		{
			usart.Transmit("|bubbleosrt|=>");
			usart.Transmit((long)measureDistances[i]);
		}
			
	//Remove all the values that vary 5% of the compare Value
	uint16_t compareValue = (measureDistances[4] + measureDistances[5]) / 2;
	uint8_t lowIndex = 0, highIndex = 9;
	for (uint8_t i = 0; i < 10; ++i)
	{
		if (measureDistances[i] < (uint32_t)compareValue * 95 / 100)
		{
			lowIndex = i;
		}
		else if (measureDistances[i] > (uint32_t)compareValue * 105 / 100)
		{
			highIndex = i + 1;
			break;
		}
	}
	usart.Transmit("|Compare|=>");
	usart.Transmit((long)compareValue);
	
	usart.Transmit("|low|=>");
	usart.Transmit(lowIndex);
	
	usart.Transmit("|high|=>");	
	usart.Transmit(highIndex);
	
	for (uint8_t i = lowIndex; i < highIndex; i++)
	{
		usart.Transmit("|mesureD|==>");
		usart.Transmit((long)measureDistances[i]);
	}
		
	uint32_t averageValue = 0;
	for (uint8_t i = lowIndex; i < highIndex; ++i)
	{
		averageValue += measureDistances[i];
	}
	usart.Transmit("|averageValue|=>");
	usart.Transmit((long)averageValue / (highIndex - lowIndex));
	
	for (uint8_t i = lowIndex; i < highIndex; i++)
	{
		usart.Transmit("|mesureD|=>");
		usart.Transmit((long)measureDistances[i]);
	}
	
	return averageValue / (highIndex - lowIndex);

}

/**
 * Converts Time and Temperature to distance
 *
 * \code
 * usonic::convertdistance(uSonic.getDistance(), tempsen.getTemperature());
 * \endcode
 *
 * \param[in] The time it took in 62.5 Nanosecond ticks
 * \param[in] The temperature next to the Ultrasonic Module
 * \return The distance in cm
 */
uint16_t usonic::convertDistance(uint32_t ticks, uint16_t tempAir)
{
	//ultraSonicSpeed * 10000 = (Speed at 0 Degree * 10000) + (1 Degree Difference * 1000) * (Degrees * 10))
	uint32_t ultraSonicSpeed = 3313010 + 606 * tempAir;

	//ultraSonicSpeed converted to microsecond/centimeter
	uint16_t ultraSonicConstant = (uint16_t)((uint64_t)600000 * 10000 * 100 / (uint64_t)(ultraSonicSpeed * 60));
	
	//(ticks * (Nanoseconds per clock * 1000) / 100 / 2) / 29.1;
	//2 -> due to the sonic signal traveling back and forth	
	return (ticks * 6250 / 100 / 2) / ultraSonicConstant;
}

/**
 * Sends the Ultrasonic module the request to start a new measurement
 *
 * To measure it first starts a timer to ensure that an asynchronous signal is sent long enough for the
 * Ultrasonic module to recognize. After that we wait for the echo signal to arrive. Once a rising edge
 * is detected it starts the timer again. This time however it measures how long it takes for the echo
 * signal to return to a falling edge. This time then is put into the _distance parameter which can be
 * retrieved through uSonic.getDistance();
 *
 * \code
 * uSonic.startMeasurement();
 * while (!isReady());
 * uSonic.getDistance();
 * \endcode
 *
 * \note Doesn't work if there is currently an ongoing Measurement
 */
void usonic::startMeasurement()
{
	//Starts a new measurement if last one finished
	if (_ready)
	{
		_ready = false;
		uSonic.setOverflows(0);
			
		//Start a timer so that the Trigger signal is at least 10�s long
		USONIC_PORT |= (1 << USONIC_TRIGGER_PIN);
		USONIC_PRESCALER1();
	}
}

