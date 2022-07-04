/*
 * USART.h
 *
 * Created: 04.02.2015 08:12:11
 *  Author: gert
 */ 

#define F_CPU 16000000UL
#include <avr\interrupt.h>


#ifndef USART_H_
#define USART_H_
//#include <stdlib.h>

#define USART_BAUDRATE 115200
#define BAUD_PRESCALE  8

volatile unsigned char value;

enum USART_Control {
	USART_LF  = 10,
	USART_Tab = 11,
	USART_CR  = 13
	};


ISR(USART0_RX_vect)
{
	value = UDR0;
	PORTC = ~value;
}

void USART_Init(void){
   // Set baud rate
   //UBRR0H = (BAUD_PRESCALE >> 8);    
   //UBRR0L = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
	UBRR0 = 8;
	 /* Load upper 8-bits into the high byte of the UBRR register
    Default frame format is 8 data bits, no parity, 1 stop bit
  to change use UCSRC, see AVR datasheet*/ 
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
  // Enable receiver and transmitter and receive complete interrupt 
  UCSR0B |= ((1<<TXEN0)|(1<<RXEN0) | (1<<RXCIE0));
}


void USART_SendByte(uint8_t u8Data) {

	// Wait until last byte has been transmitted
	while((UCSR0A &(1<<UDRE0)) == 0);
	// Transmit data
	UDR0 = u8Data;
	
}


void USART_sendBytes(char* text, uint8_t usartControl = 0)
{
	while(*text != '\0')
	{
		USART_SendByte((uint8_t)*text);
		text++;
	}
	if(usartControl)
		USART_SendByte(usartControl);
	
}


// not being used but here for completeness
// Wait until a byte has been received and return received data
uint8_t USART_ReceiveByte(){
	while((UCSR0A &(1<<RXC0)) == 0);
	return UDR0;
}


void USART_sendInteger(long zahl, uint8_t usartControl = 0)
{

	char strZahl[11];	

	ltoa(zahl, strZahl, 10);
	USART_sendBytes(strZahl);
	
}


void USART_sendDezimal(double zahl, uint8_t Kommastellen=1, uint8_t usartControl = 0)
{
	long vorkomma = (long)zahl;
	long DezFaktor = 1;
	long nachkomma = 0;
	
	if (Kommastellen < 10) {
		for (uint8_t i  = 0; i < Kommastellen; i++)
		{
			DezFaktor *= 10;
		}
		
		nachkomma = (long)((zahl - (double)vorkomma) * (double)DezFaktor);
	}
	USART_sendInteger(vorkomma);
	USART_SendByte('.');
	USART_sendInteger(nachkomma);
	if(usartControl)
		USART_SendByte(usartControl);
}


#endif /* USART_H_ */