#include "usart.h"


SnowHeight::Interface::Usart::Usart()
{
	this->Initialize();
}

SnowHeight::Interface::Usart::~Usart()
{
		
}

void SnowHeight::Interface::Usart::Initialize()
{
	// set baudrate
	UBRR0H = (this->prescale >> 8);
	UBRR0L = this->prescale;// load lower 8-bits into the low byte of the UBRR register
		
	// enable transmitter
	UCSR0B = (1<<TXEN0);
}

// transmit 1 byte
void SnowHeight::Interface::Usart::Transmit(uint8_t byte)
{
	// wait until last byte has been transmitted
	while ((UCSR0A & (1 << UDRE0)) == 0)
	{
	}
		
	// transmit 1 byte
	UDR0 = byte;
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
	// wait until last byte has been transmitted
	while ((UCSR0A & (1 << UDRE0)) == 0)
	{
	}
	
	// transmit 1 ASCII-character
	UDR0 = character;
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
	/* TODO
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
	
	
	// TODO
	// OLD VERSION!
	long restzahl = number;
	int8_t rest = 0;
	char strZahl[11];
	uint8_t stelle = 0;
	
	if (restzahl == 0)
	{
		strZahl[stelle] = '0';
		stelle = 1;
	}
	else
	{
		if(restzahl<0)	{
			strZahl[stelle] = '-';
			stelle++;
		}
		
		while(restzahl != 0){
			rest = restzahl % 10;
			restzahl /= 10;
			strZahl[stelle] = (rest + 48);
			stelle++;
		}
	}
	
	for (int i = stelle-1; i >= 0; i--)	{
		this->Transmit((uint8_t)(strZahl[i]));
	}
}

// TODO: rewrite
void SnowHeight::Interface::Usart::Transmit(float number, uint8_t decimals)
{
	long integer = (long)number;
	long DezFaktor = 1;
	long nachkomma = 0;
	
	// float has per definition 7 decimals
	if (decimals < 8)
	{
		for (uint8_t i  = 0; i < decimals; i++)
		{
			DezFaktor *= 10;
		}
		
		nachkomma = (long)((number - (float)integer) * (float)DezFaktor);
		
		this->Transmit(integer);
		this->Transmit('.');
		this->Transmit(nachkomma);
	}
}

// TODO: rewrite
void SnowHeight::Interface::Usart::Transmit(double number, uint8_t decimals)
{
	long integer = (long)number;
	long DezFaktor = 1;
	long nachkomma = 0;
	
	// float has per definition 15 decimals
	if (decimals < 16)
	{
		for (uint8_t i  = 0; i < decimals; i++)
		{
			DezFaktor *= 10;
		}
		
		nachkomma = (long)((number - (double)integer) * (double)DezFaktor);
		
		this->Transmit(integer);
		this->Transmit('.');
		this->Transmit(nachkomma);
	}
}

uint8_t SnowHeight::Interface::Usart::ReceiveByte()
{
	while((UCSR0A &(1<<RXC0)) == 0)
	{
		// wait until USART-interface is ready to use
		// - datatransmission could be ongoing from other modules -
	}
	
	// UDR0 is the recive pin (register with recived byte)
	return UDR0;
}