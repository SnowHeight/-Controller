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
			
				Usart();
				~Usart();
				
				void Initialize();
				
				void Transmit(uint8_t byte);
				void Transmit(uint8_t* bytes);
				void Transmit(char byte);
				void Transmit(char* bytes);
				void Transmit(long number);
				void Transmit(double number, uint8_t decimals);
				void Transmit(float number, uint8_t decimals);
				
				uint8_t ReceiveByte();

			private:
				
				static const uint32_t baudrate = 115200;
				static const uint8_t  prescale = 8;
				
				
		}; // class Usart
	} // namespace Interfaces
} // namespace SnowHeight
#endif // _USART_H_