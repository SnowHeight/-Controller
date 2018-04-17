/*
 * SnowHeight.cpp
 *
 * Created: 19.02.2018 15:37:52
 * Author : gert
  *	\Edit bbruestle
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <string.h>
#include <avr/wdt.h>

#include "configuration.h"
#include "general.h"
#include "usart/usart.h"
#include "usonic/usonic.h"


int main(void)
{	
	sei();
	char trUltrasonicData[] = "|Ultrasonic|=>";
	uint16_t nUsonicDistance = uSonic.getAverageDistance();
	
	SnowHeight::Interface::Usart usart0;
    usart0.Transmit((double)nUsonicDistance,1);
	
    while (1) 
    {
    }
}

	ISR(USONIC_OVF_ISR)
	{
		//Too far -> Increase Overflow
		uSonic.setOverflows(uSonic.getOverflows() + 1);
	}
	
	ISR(USONIC_OCRA_ISR)
	{
		//Restarts the 16bit Timer when the TriggerPin is active
		if (USONIC_PIN & (1 << USONIC_TRIGGER_PIN))
		{
			USONIC_PRESCALER0();
			USONIC_PORT &= ~(1 << USONIC_TRIGGER_PIN);
			EIMSK |= (1 << USONIC_ECHO_INT);				// ext. INT 3
		}
	}
	
	ISR(USONIC_ECHO_ISR)
	
	{	SnowHeight::Interface::Usart usartEcho;
		//Check if Interrupt was caused by raising edge or falling edge
		if (USONIC_PIN & (1 << USONIC_ECHO_PIN))
		{
			// raising edge detection
			USONIC_TCNT = 0;
			USONIC_PRESCALER1();
		}
		else
		{
			// falling edge detection
			USONIC_PRESCALER0();
			EIMSK &= ~(1 << USONIC_ECHO_INT);
			cli();
	
			//TODO: Get the real Temperature * 10
			uint16_t convertedDistance = uSonic.convertDistance((uSonic.getOverflows() * UINT16_MAX) + USONIC_TCNT, 200);
			uSonic.setDistance(convertedDistance);
			sei();
			uSonic.setReady(true);
			USONIC_TCNT = 0;
		}
	}
