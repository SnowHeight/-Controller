/*
	Project:				SnowHeight
	File:					usart.cpp
	
	Created:				02/19/2018
	Last modified:			03/19/2018
	
	***
	
	Authors:				Project leading:
								ING. BIRKNER GERT
					
							Programming & fine-tuning:
								ROBERT JA?KOWSKI
								RAFAEL KNIPPITSCH
								FABIO WITTING
								DAVID OBERHOFER
								BJÖRN BRÜSTLE
							
							Concept & prototype development:
								FUETSCH ARTHUR
								BARIC ALEN
								MATHIAS WICHENTH.-STERNBACH
								NIKOLAS PACIK
								MARKUS PLANKENSTEINER
								CHRISTOPH MELLAUNER
								ALEXANDER VUKOVIC
								STEFAN SCHWARZENBERGER
			
	License:				
	
	***
	
	Project description:	See readme.txt
	
	***
	
	File description:		Implementation of the USART interface.
	
*/

#include "usart.h"


SnowHeight::Interface::Usart::Usart(uint8_t port)
{
	this->port = port;
	this->Initialize();
}

void SnowHeight::Interface::Usart::Initialize()
{
	switch (this->port)
	{
		// USART port 0
		case 0:
			// set baudrate
			UBRR0H = (this->prescale >> 8);
			UBRR0L = this->prescale;
			
			// enable transmitter & receiver
			UCSR0B = ((1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0));
			break;
			
		// USART port 1
		case 1:
			// set baudrate
			UBRR1H = (this->prescale >> 8);
			UBRR1L = this->prescale;
			
			// enable transmitter & receiver
			UCSR1B = ((1<<TXEN1) | (1<<RXEN1) | (1<<RXCIE1));
			break;
			
		// USART port 2
		case 2:
			// set baudrate
			UBRR2H = (this->prescale >> 8);
			UBRR2L = this->prescale;
			
			// enable transmitter & receiver
			UCSR2B = ((1<<TXEN2) | (1<<RXEN2) | (1<<RXCIE2));
			break;
			
		// USART port 3
		case 3:
			// set baudrate
			UBRR3H = (this->prescale >> 8);
			UBRR3L = this->prescale;
			
			// enable transmitter & receiver
			UCSR3B = ((1<<TXEN3) | (1<<RXEN3) | (1<<RXCIE3));
			break;
	}
}

// transmit 1 byte
void SnowHeight::Interface::Usart::Transmit(uint8_t byte)
{
	switch (this->port)
	{
		// USART port 0
		case 0:
			while ((UCSR0A & (1 << UDRE0)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
			
			// transmit 1 byte
			UDR0 = byte;
			break;
			
		// USART port 1
		case 1:
			while ((UCSR1A & (1 << UDRE1)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
		
			// transmit 1 byte on port 1
			UDR1 = byte;
			break;
			
		// USART port 2
		case 2:
			while ((UCSR2A & (1 << UDRE2)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
		
			// transmit 1 byte on port 1
			UDR2 = byte;
			break;
			
		// USART port 3
		case 3:
			while ((UCSR3A & (1 << UDRE3)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
		
			// transmit 1 byte on port 1
			UDR3 = byte;
			break;
	}
}

// transmit array of bytes until '\0' (0x00) is reached in array
void SnowHeight::Interface::Usart::Transmit(uint8_t* bytes)
{
	// transmit each byte by byte
	while (*bytes != '\0')
	{
		this->Transmit(*bytes);
		bytes++;
	}
}

// transmit 1 byte as ASCII-character
// - for easier use (without casting) of the class -
void SnowHeight::Interface::Usart::Transmit(char character)
{
	//// wait until last byte has been transmitted
	//while ((UCSR0A & (1 << UDRE0)) == 0)
	//{
	//}
	//
	//// transmit 1 ASCII-character
	//UDR0 = character;
	
	this->Transmit((uint8_t)character);
}

// transmit array of bytes until '\0' (0x00) is reached in array
void SnowHeight::Interface::Usart::Transmit(char* string)
{
	// transmit each ASCII-character of the string byte by byte
	while (*string != '\0')
	{
		this->Transmit(*string);
		string++;
	}
}

// transmit number as its corresponding ASCII representation
// - each digit as a ASCII-character-byte
void SnowHeight::Interface::Usart::Transmit(long number)
{
	/* TODO: get a better algorithm for value in ASCII-string conversion
	
	// range of "unsigned long" type = 4 * 10^9
	// we need 10 digits for that range + 1 digit used for the sign
	char valueAsString[11];

	// is it a negative number?
	if (number < 0)
	{
		// add minus sign
		*valueAsString = '-';
		//valueAsString++;
	}

	while (number % 10)
	{
		return;
	}
	*/
	
	
	// TODO: maybe replace algorithm with a better one
	// OLD VERSION!
	long remainder = number;
	int8_t rest = 0;
	char valueAsASCII[11];	// value as ASCII-string representation
	uint8_t decimalPlace = 0;
	
	if (remainder == 0)
	{
		valueAsASCII[decimalPlace] = '0';
		decimalPlace = 1;
	}
	else
	{
		if(remainder < 0)	{
			valueAsASCII[decimalPlace] = '-';
			decimalPlace++;
		}
		
		while(remainder != 0){
			rest = remainder % 10;
			remainder /= 10;
			valueAsASCII[decimalPlace] = (rest + 48);
			decimalPlace++;
		}
	}
	
	for (int i = decimalPlace-1; i >= 0; i--)	{
		this->Transmit((uint8_t)(valueAsASCII[i]));
	}
}

// transmit float as its ASCII-string representation
void SnowHeight::Interface::Usart::Transmit(float number, uint8_t decimals)
{
	long integer = (long)number;
	long decimalFactor = 1;
	long fractionalDigits = 0;
	
	// float has per definition 7 decimals
	if (decimals < 8)
	{
		for (uint8_t i  = 0; i < decimals; i++)
		{
			decimalFactor *= 10;
		}
		
		// get the fractional digits from the "float" value
		fractionalDigits = (long)((number - (float)integer) * (float)decimalFactor);
		
		this->Transmit(integer);
		this->Transmit('.');
		this->Transmit(fractionalDigits);
	}
}

// transmit double as its ASCII-string representation
void SnowHeight::Interface::Usart::Transmit(double number, uint8_t decimals)
{
	long integer = (long)number;
	long decimalFactor = 1;
	long fractionalDigits = 0;
	
	// float has per definition 15 decimals
	if (decimals < 16)
	{
		for (uint8_t i  = 0; i < decimals; i++)
		{
			decimalFactor *= 10;
		}
		
		fractionalDigits = (long)((number - (double)integer) * (double)decimalFactor);
		
		this->Transmit(integer);
		this->Transmit('.');
		this->Transmit(fractionalDigits);
	}
}

uint8_t SnowHeight::Interface::Usart::ReceiveByte()
{
	switch (this->port)
	{
		// USART port 0
		case 0:
			while((UCSR0A &(1<<RXC0)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
			
			// UDR0 is the receive pin (register with received byte)
			return UDR0;
			
		// USART port 1
		case 1:
			while((UCSR1A &(1<<RXC1)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
			
			// UDR1 is the receive pin (register with received byte)
			return UDR1;
			
		// USART port 2
		case 2:
			while((UCSR2A &(1<<RXC2)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
			
			// UDR0 is the receive pin (register with received byte)
			return UDR2;
			
		// USART port 3
		case 3:
			while((UCSR3A &(1<<RXC3)) == 0)
			{
				// wait until USART-interface is ready to use
				// - data transmission could be ongoing from other modules
			}
			
			// UDR0 is the receive pin (register with received byte)
			return UDR3;
	}
	
	return 0;
}