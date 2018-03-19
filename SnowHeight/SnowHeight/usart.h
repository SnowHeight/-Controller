/*
	Project:				SnowHeight
	File:					usart.h
	
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
	
	File description:		Definition of the USART interface.
	
*/

#ifndef _USART_H_
#define _USART_H_

#include <avr/interrupt.h>

namespace SnowHeight // project namespace
{
	namespace Interface
	{
		class Usart
		{
			public:
			
				Usart(uint8_t port = 0);
				void Initialize();
				
				// this functions are transmitting there value as binary bytes
				void Transmit(uint8_t byte);
				void Transmit(uint8_t* bytes);
				
				// this functions are transmitting there value as its ASCII-string representation
				void Transmit(char byte);
				void Transmit(char* bytes);
				void Transmit(long number);
				void Transmit(double number, uint8_t decimals);
				void Transmit(float number, uint8_t decimals);
				
				uint8_t ReceiveByte();

			private:
				
				static const uint32_t baudrate = 115200;
				static const uint8_t  prescale = 8;
				
				uint8_t port;
		}; // class Usart
	} // namespace Interfaces
} // namespace SnowHeight
#endif // _USART_H_



/* USART Ports with there hardware-addresses 

// Port 0				in decimal
UBRR0L  _SFR_MEM8(0xC4) 196
UBRR0H  _SFR_MEM8(0xC5) 197
UCSR0B  _SFR_MEM8(0XC1) 193
TXEN0   3
UCSR0A  _SFR_MEM8(0xC0) 192
UDRE0   5
UDR0    _SFR_MEM8(0XC6) 198
RXC0    7

// Port 1
UBRR1L  _SFR_MEM8(0xCC) 204
UBRR1H  _SFR_MEM8(0xCD) 205
UCSR1B  _SFR_MEM8(0XC9) 201
TXEN1   3
UCSR1A  _SFR_MEM8(0xC8) 200
UDRE1   5
UDR1    _SFR_MEM8(0XCE) 206
RXC1    7

// Port 2
UBRR2L _SFR_MEM8(0xD4) 212
UBRR2H _SFR_MEM8(0xD5) 213
UCSR2B _SFR_MEM8(0XD1) 209
TXEN2   3
UCSR2A _SFR_MEM8(0xD0) 208
UDRE2   5
UDR2   _SFR_MEM8(0XD6) 214
RXC2    7

// Port 3
UBRR3L _SFR_MEM8(0x134) 308
UBRR3H _SFR_MEM8(0x135) 309
UCSR3B _SFR_MEM8(0X131) 305
TXEN3   3
UCSR3A _SFR_MEM8(0x130) 304
UDRE3   5
UDR3   _SFR_MEM8(0X136) 310
RXC3    7

*/