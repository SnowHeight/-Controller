/* 
* uart.h
* Simple UART class for uart0 on atmega32
* Overloaded methods write for different datatypes
* no string needed, but every char array must be '\0' terminated
* to ensure correct char array length
* reading can be done by reading only one char or a char array handed over as char* and size
*
* uart must be enabled and can be disabled to start/stop sensing uart port and bind/unbind the rx and tx pins 
*
* with every write method a linefeed can be carried out with the second parameter (boolean), default is no linefeed
* Linefeed and ClearScreen can be executed with extra methods and these are VT100 and above compatible
*
* TODO: insert parameters to contructor (Baud-rate)
* TODO: use different uarts on different controllers
*
* Created: 06.12.2017
* Author: gert birkner bSd 
* use this switch for the uart modules
	switch(module) {
		case uartModule::uart0:
		break;
		#if UARTs_MAX > 0
		case uartModule::uart1:
		break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
		break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
		break;
		#endif
	}

*/


#ifndef __MYUART_H__
#define __MYUART_H__

#include <avr/io.h>
#include <stdlib.h>

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#  define UARTs_MAX		0
#elif (defined __AVR_ATmega2560__)
#  define UARTs_MAX		3
#endif

#define UART_LF	'\n'
#define UART_CR '\r'

enum class UartBaudRates : uint16_t {
	br4800 = 48,
	br9600 = 96,
	br14400 = 144,
	br19200 = 192,
	br28800 = 288,
	br38400 = 384,
	br57600 = 576,
	br115200 = 1152,
	br230400 = 2304,
	br460800 = 4608,
	br500000 = 5000,
	br1000000 = 10000
};

enum class UartDataBits {
	db5 = 0,
	db6 = 1,
	db7 = 2,
	db8 = 3,
	db9 = 7
};

enum class UartParity {
	none = 0,
	even = 2,
	odd = 3
};

enum class UartStopBit {
	sb1 = 0,
	sb2 = 1
};

enum class uartModule {
	uart0 = 0,
	#if UARTs_MAX > 0
	uart1,
	#endif
	#if UARTs_MAX > 1
	uart2,
	#endif
	#if UARTs_MAX > 2
	uart3
	#endif
};

struct uartSettings {
	uartModule module = uartModule::uart0;
	UartBaudRates baud = UartBaudRates::br9600;
	UartDataBits databits = UartDataBits::db8;
	UartParity parity = UartParity::none;
	UartStopBit stopbits = UartStopBit::sb1;
	bool doubleSpeed = false;
	bool enRx = true;
	bool enTx = true;
};



/*
*\brief
* UART standard class to write char, char*, integers, float and double
* and to read char and char*
*/
class myUart {

//variables
public:
	
private:
	uartModule module = uartModule::uart0;
	bool enRx = true;
	bool enTx = true;

//functions
public:
	myUart(uartSettings settings);
	~myUart();

	void enable();
	void disable();
	void write(unsigned char c, bool linefeed = false);
	void write(char *s, bool linefeed = false);
	void write(uint16_t i, bool linefeed = false);
	void write(int16_t i, bool linefeed = false);
	void write(uint32_t i, bool linefeed = false);
	void write(int32_t i, bool linefeed = false);
	void write(float f, uint8_t precision = 2, bool linefeed = false);
	void write(double d, uint8_t precision = 2, bool linefeed = false);
	void cls();
	void LfCr();
	uint8_t read(unsigned char *x, uint8_t size);
	unsigned char read();


private:
	//myUart( const myUart &c );
	//myUart& operator=( const myUart &c );

};   //myUart


#endif //__MYUART_H__
