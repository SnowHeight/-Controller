/*
	Project:				SnowHeight
	File:					configuration.h
	
	Created:				02.19.2018
	Last modified:			02.26.2018
	
	***
	
	Authors:				
			
	License:				
	
	***
	
	Project description:	See readme.txt
	
	***
	
	File description:		General definitions for all modules and controller.
	
*/

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

// global constants
#define F_CPU						16000000UL	// CPU Frequency - constant must be defined as "F_CPU" for API compatibilities with API "I2C_MASTER"

//EEPROM
#define EEPROM_MatrixStart			0x00FC
#define EEPROM_ParameterStart		0x0000

//Real Time Clock- DS3231 (Tiny RTC)
#define RTC_ADDR					(0x68<<1) //device address
#define RTC_I2CINIT					1
#define RTC_EICR					EICRA
#define RTC_ISC0					ISC20
#define RTC_ISC1					ISC21
#define RTC_ISR						INT2_vect
#define RTC_INT						INT2
#define RTC_ENABLE_RISING_EDGE()	(RTC_EICR |= (1 << RTC_ISC1))
#define RTC_ENABLE_EXT_INTERRUPT()	(EIMSK |= ( 1 << RTC_INT))
#define RTC_ALARM1_BIT				0
#define RTC_ALARM1_FLAG				0

// weather sensor - Bosch BME280 sensor
#define WEATHER_SENSOR_ADDRESS		0x77

// ultrasonic measuring sensor
#define USONIC_PORT					PORTD
#define USONIC_PIN					PIND
#define USONIC_DIR					DDRD
#define USONIC_ECHO_PIN				PD3
#define USONIC_TRIGGER_PIN			PD7
#define USONIC_ECHO_ISR				INT3_vect
#define USONIC_ECHO_EICR			EICRA
#define USONIC_INTERRUPT			ISC30
#define USONIC_ECHO_INT				INT3

#define USONIC_TIMSK				TIMSK5
#define USONIC_TOVF					TOIE5
#define USONIC_OCRA					OCIE5A
#define USONIC_TRIGGER_IMPULS		OCR5A
#define USONIC_TCNT					TCNT5
#define USONIC_OVF_ISR				TIMER5_OVF_vect
#define USONIC_PRESCALER1()			(TCCR5B |= (1 << CS50))
#define USONIC_PRESCALER0()			(TCCR5B &= ~((1 << CS50) | (1 << CS51) | (1 << CS52)))
#define USONIC_OCRA_ISR				TIMER5_COMPA_vect
// 16bit max unsigned int value
#define UINT16_MAX					65536

// servo motor
#define SERVO_PWM_PORT 				PORTB				//Port B
#define SERVO_PWM_DIRECTION			DDRB				//Data Direction Registry B
#define SERVO1_PWM_OUT 				PB5					//Pin PB5 (Pin 11)
#define SERVO2_PWM_OUT				PB6					//Pin PB6 (Pin 12)
#define BATTERY_MEASURE_OUT			PB7					//Pin PB7 (Pin 13)
// Cutoff Port
#define CUTOFF_PORT					PORTH				//Port H
#define CUTOFF_DIRECTION			DDRH				//Data Direction Registry H
#define SERVO_CUTOFF				PH4					//Pin PH4 (Pin 7)
#define MODULE_CUTOFF				PH3					//Pin PH3 (Pin 6)

#define MAX_CURRENT_ADC				40					//max. ADC-Value (~100 mA)
#define MAX_BATTERY_VALUE			13672				//(14 / 1024) * 1000 000
#define MASK						0b1111111111111000	//Bit-Mask (16 Bit)

#endif // _CONFIGURATION_H_