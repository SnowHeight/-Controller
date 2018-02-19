/*
* WeatherSensor.cpp
*
* Created: 03.03.2017 17:04:35
*  Author: Aleksandar V., Stefan S.
*/
#include <avr\io.h>
#include "../general.h"
#include "WeatherSensor.h"
#include <util/delay.h>
//#include "USART.h"

extern "C" {
	#include "../I2C_Master/i2c_master.h"
}
	uint8_t addressS1 = 0x77;
	uint8_t addressS2 = 0x76;



WeatherSensor::WeatherSensor(uint8_t address) :
	temperatureS1(0),
	m_humidityS1(0),
	pressureS1(0)
{
	i2c_init();
	sensorAddress = address;
}
		/*******************************************read Function *****************************/
uint16_t read8(uint8_t reg, uint8_t addressSensor)
{
	uint16_t val;
	
	if (i2c_start(addressSensor<<1 | I2C_WRITE) == 0)
	{
		i2c_write((uint8_t)reg);
		
		i2c_stop();
		
		if (i2c_start(addressSensor<<1 | I2C_READ) == 0)
		{
			val |= ((uint16_t)i2c_read_nack());
			
			i2c_stop();
			
			return val;
		}
	}
}


uint16_t read16(uint8_t reg, uint8_t addressSensor)
{
	uint16_t val;
	
	if (i2c_start(addressSensor<<1 | I2C_WRITE) == 0)
	{
		i2c_write((uint8_t)reg);
		
		i2c_stop();
		
		if (i2c_start(addressSensor<<1 | I2C_READ) == 0)
		{
			val = ((uint16_t)i2c_read_ack());
			val <<= 8;
			val |= ((uint16_t)i2c_read_nack());
			
			i2c_stop();
			
			return val;
		}
	}
}

uint16_t read16_LE(uint8_t reg)
{
	uint16_t temp = read16(reg, addressS1);
	return (temp >> 8) | (temp << 8);
	
}

int16_t readS16(uint8_t reg)
{
	return (int16_t)read16(reg, addressS1);
	
}

int16_t readS16_LE(uint8_t reg)
{
	return (int16_t)read16_LE(reg);
	
}

uint32_t read24(uint8_t reg, uint8_t addressSensor)
{
	uint32_t val;

	if (i2c_start(addressSensor<<1 | I2C_WRITE) == 0)
	{
		i2c_write(reg);

		i2c_stop();

		if (i2c_start(addressSensor<<1 | I2C_READ) == 0)
		{
			val = ((uint16_t)i2c_read_ack());
			val <<= 8;
			val |= ((uint16_t)i2c_read_ack());
			val <<= 8;
			val |= ((uint16_t)i2c_read_nack());
			
			i2c_stop();
			
			return val;
		}
	}
}
					/*********************************END read function *********************/
/**
 * \brief
 * This method reads out the raw values from the weather sensor
 * 
 * 
 */
void WeatherSensor::readData()
{
	//*******************************************************************************READOUT SENSOR 1
	//************************************* Humidity oversampling
	i2c_start((addressS1 << 1) + I2C_WRITE);
	i2c_write(0xF2);
	i2c_stop();	
	i2c_start((addressS1 << 1) + I2C_READ);
	uint8_t F2varS1 = i2c_read_nack();
	i2c_stop();
	F2varS1 &= ~((1<<2) | 1<<1);
	F2varS1 |= (1<<0);
	i2c_start((addressS1 << 1) + I2C_WRITE);
	i2c_write(0xF2);
	i2c_write(F2varS1);
	i2c_stop();
	//************************************** end humidity oversampling
	i2c_start((addressS1 << 1) + I2C_WRITE);
	i2c_write(0xF5);
	i2c_stop();
	i2c_start((addressS1 << 1) + I2C_READ);
	uint8_t VarF5S1 = i2c_read_nack();
	i2c_stop();
	VarF5S1 &= ~((1<<0) | (1<<4) | (1<<7));
	VarF5S1 |= ((1<<2) | (1<<3) | (1<<5) | (1<<6));
	i2c_start((addressS1 << 1) + I2C_WRITE);
	i2c_write(0xF5);
	i2c_write(VarF5S1);
	i2c_stop();
	
	//************************************ temp & pressure oversampling
	i2c_start((addressS1 << 1) + I2C_WRITE);
	i2c_write(0xF4);											// control register
	i2c_write(BME280_TempPressMode1);							// send temperature configuration command
	i2c_stop();
	//************************************ end temp & pressure oversampling

	i2c_start((addressS1 << 1) + I2C_WRITE);						// send I2C register address 
	i2c_write(BME280_TEMPDATA);									// send temperature register address
	i2c_stop();													//Stop I2C 
	_delay_ms(6);												//set delay to convert (6 ms)
	i2c_start((addressS1 << 1) + I2C_READ);						// start I2C and read temperature

	uint8_t tempS1[3];											//create array where temp can be saved
	uint8_t pressS1[3];											//create array where press can be saved
	uint16_t humidS1[2];											//create array where humid can be saved

	pressS1[0] = i2c_read_ack();									//Write readed data in pressure array
	pressS1[1] = i2c_read_ack();
	pressS1[2] = i2c_read_ack();
	tempS1[0] = i2c_read_ack();									//Write readed data in temperature array
	tempS1[1] = i2c_read_ack();
	tempS1[2] = i2c_read_ack();
	humidS1[0] = i2c_read_ack();									//Write readed data in humidity array
	humidS1[1] = i2c_read_nack();
	i2c_stop();													//stop I2C
	
	adc_TS1 = read24(0xFA, addressS1);
	adc_PS1 = read24(0xF7, addressS1);
	adc_HS1 = humidS1[0];

	//********************************************************************SENSOR 2****************************************************************
		//************************************* Humidity oversampling
	i2c_start((addressS2 << 1) + I2C_WRITE);
	i2c_write(0xF2);
	i2c_stop();	
	i2c_start((addressS2 << 1) + I2C_READ);
	uint8_t F2varS2 = i2c_read_nack();
	i2c_stop();
	F2varS2 &= ~((1<<2) | 1<<1);
	F2varS2 |= (1<<0);
	i2c_start((addressS2 << 1) + I2C_WRITE);
	i2c_write(0xF2);
	i2c_write(F2varS2);
	i2c_stop();
	//************************************** end humidity oversampling
	i2c_start((addressS2 << 1) + I2C_WRITE);
	i2c_write(0xF5);
	i2c_stop();
	i2c_start((addressS2 << 1) + I2C_READ);
	uint8_t VarF5S2 = i2c_read_nack();
	i2c_stop();
	VarF5S2 &= ~((1<<0) | (1<<4) | (1<<7));
	VarF5S2 |= ((1<<2) | (1<<3) | (1<<5) | (1<<6));
	i2c_start((addressS2 << 1) + I2C_WRITE);
	i2c_write(0xF5);
	i2c_write(VarF5S2);
	i2c_stop();
	
	//************************************ temp & pressure oversampling
	i2c_start((addressS2 << 1) + I2C_WRITE);
	i2c_write(0xF4);											// control register
	i2c_write(BME280_TempPressMode1);							// send temperature configuration command
	i2c_stop();
	//************************************ end temp & pressure oversampling

	i2c_start((addressS2 << 1) + I2C_WRITE);						// send I2C register address 
	i2c_write(BME280_TEMPDATA);									// send temperature register address
	i2c_stop();													//Stop I2C 
	_delay_ms(6);												//set delay to convert (6 ms)
	i2c_start((addressS2 << 1) + I2C_READ);						// start I2C and read temperature

	uint8_t tempS2[3];											//create array where temp can be saved
	uint8_t pressS2[3];											//create array where press can be saved
	uint16_t humidS2[2];											//create array where humid can be saved

	pressS2[0] = i2c_read_ack();									//Write readed data in pressure array
	pressS2[1] = i2c_read_ack();
	pressS2[2] = i2c_read_ack();
	tempS2[0] = i2c_read_ack();									//Write readed data in temperature array
	tempS2[1] = i2c_read_ack();
	tempS2[2] = i2c_read_ack();
	humidS2[0] = i2c_read_ack();									//Write readed data in humidity array
	humidS2[1] = i2c_read_nack();
	i2c_stop();													//stop I2C
	
	adc_TS2 = read24(0xFA, addressS2);
	adc_PS2 = read24(0xF7, addressS2);
	adc_HS2 = humidS2[0];

	CompensateData();											//start function CompensateData()
}

/**
 * \brief 
 * This method reads out the compensation parameters, calculates the temperature, pressure and humidity and saves the values in global variables 
 * 
 * \return void
 */
void WeatherSensor::CompensateData()
{
	//*******************************************SENSOR 1******************************************
	i2c_start((addressS1 << 1) + I2C_WRITE);	// start I2c in write Mode
	i2c_write(0xF4);											// control register
	i2c_write(BME280_TempPressMode1);							// get temperature config command
	i2c_stop();													//I2C Stop
	_delay_ms(6);												//set delay to convert
	i2c_start((addressS1 << 1) + I2C_WRITE);						//start I2C in write mode
	i2c_write(BME280_TEMPCOMP);									//Write Temp command 
	i2c_stop();													//I2C Stop
	i2c_start((addressS1 << 1) + I2C_READ);						//Red data
	uint16_t dig_tS1[3];
	uint16_t dig_PS1[9];
	uint16_t dig_HS1[5];

	dig_tS1[0] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_tS1[1] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_tS1[2] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[0] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[1] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[2] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[3] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[4] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[5] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[6] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[7] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS1[8] = i2c_read_ack() + (i2c_read_ack() << 8);
	i2c_read_ack();
	dig_HS1[0] = i2c_read_nack();
	i2c_stop();

	i2c_start((addressS1 << 1) + I2C_WRITE);						// start I2c in write Mode
	i2c_write(0xF2);
	i2c_write(BME280_HUMCOMP);
	i2c_stop();
	i2c_start((addressS1 << 1) + I2C_READ);
	dig_HS1[1] = i2c_read_ack() + ((uint16_t)i2c_read_ack() << 8);
	dig_HS1[2] = i2c_read_ack();
	dig_HS1[3] = ((uint16_t)i2c_read_ack() << 4) + (i2c_read_ack() & 0b00001111);
	dig_HS1[4] = (i2c_read_ack() >> 4) + ((uint16_t)i2c_read_ack() <<4);
	dig_HS1[5] = i2c_read_nack();
	i2c_stop();

	//************************ Temperatur berechnen *********************************
	int32_t var1tempS1, var2tempS1;
	adc_TS1 >>= 4;
	var1tempS1  = ((((adc_TS1>>3) - ((int32_t)dig_tS1[0] <<1))) * ((int32_t)dig_tS1[1])) >> 11;
	var2tempS1  = (((((adc_TS1>>4) - ((int32_t)dig_tS1[0])) * ((adc_TS1>>4) - ((int32_t)dig_tS1[0]))) >> 12) *
	((int32_t)dig_tS1[2])) >> 14;
	t_fineS1 = var1tempS1 + var2tempS1;
	float T  = (t_fineS1 * 5 + 128) >> 8;
	temperatureS1 = T;

	//*************************** Pressure Berechnen *********************************
	int64_t var1pressS1, var2pressS1, pS1;
	adc_PS1 >>= 4;
	var1pressS1 = ((int64_t)t_fineS1) - 128000;
	var2pressS1 = var1pressS1 * var1pressS1 * (int64_t)dig_PS1[5];
	var2pressS1 = var2pressS1 + ((var1pressS1*(int64_t)dig_PS1[4])<<17);
	var2pressS1 = var2pressS1 + (((int64_t)dig_PS1[3])<<35);
	var1pressS1 = ((var1pressS1 * var1pressS1 * (int64_t)dig_PS1[2])>>8) +
	((var1pressS1 * (int64_t)dig_PS1[1])<<12);
	var1pressS1 = (((((int64_t)1)<<47)+var1pressS1))*((int64_t)dig_PS1[0])>>33;
	pS1 = 1048576 - adc_PS1;
	pS1 = (((pS1<<31) - var2pressS1)*3125) / var1pressS1;
	var1pressS1 = (((int64_t)dig_PS1[8]) * (pS1>>13) * (pS1>>13)) >> 25;
	var2pressS1 = (((int64_t)dig_PS1[7]) * pS1) >> 19;
	pS1 = ((pS1 + var1pressS1 + var2pressS1) >> 8) + (((int64_t)dig_PS1[6])<<4);
	pressureS1 = pS1/256/100;

	//*********************** Berechnung Humidity*******************
	double var_HS1;
	adc_HS1 = read8(0XFD, addressS1);

	var_HS1 = (((double)t_fineS1) - 76800.0);
	var_HS1 = (adc_HS1 - (((double)dig_HS1[3]) * 64.0 + ((double)dig_HS1[4]) / 16384.0 * var_HS1)) *
	(((double)dig_HS1[3]) / 65536.0 * (1.0 + ((double)dig_HS1[5]) / 67108864.0 * var_HS1 *
	(1.0 + ((double)dig_HS1[2]) / 67108864.0 * var_HS1)));
	var_HS1 = var_HS1 * (1.0 - ((double)dig_HS1[0]) * var_HS1 / 524288.0);
	humidityS1 = adc_HS1;

	//*****************************************SENSOR 2******************************************
	i2c_start((addressS2 << 1) + I2C_WRITE);	// start I2c in write Mode
	i2c_write(0xF4);											// control register
	i2c_write(BME280_TempPressMode1);							// get temperature config command
	i2c_stop();													//I2C Stop
	_delay_ms(6);												//set delay to convert
	i2c_start((addressS2 << 1) + I2C_WRITE);						//start I2C in write mode
	i2c_write(BME280_TEMPCOMP);									//Write Temp command 
	i2c_stop();													//I2C Stop
	i2c_start((addressS2 << 1) + I2C_READ);						//Red data
	uint16_t dig_tS2[3];
	uint16_t dig_PS2[9];
	uint16_t dig_HS2[5];

	dig_tS2[0] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_tS2[1] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_tS2[2] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[0] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[1] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[2] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[3] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[4] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[5] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[6] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[7] = i2c_read_ack() + (i2c_read_ack() << 8);
	dig_PS2[8] = i2c_read_ack() + (i2c_read_ack() << 8);
	i2c_read_ack();
	dig_HS2[0] = i2c_read_nack();
	i2c_stop();

	i2c_start((addressS2 << 1) + I2C_WRITE);						// start I2c in write Mode
	i2c_write(0xF2);
	i2c_write(BME280_HUMCOMP);
	i2c_stop();
	i2c_start((addressS2 << 1) + I2C_READ);
	dig_HS2[1] = i2c_read_ack() + ((uint16_t)i2c_read_ack() << 8);
	dig_HS2[2] = i2c_read_ack();
	dig_HS1[3] = ((uint16_t)i2c_read_ack() << 4) + (i2c_read_ack() & 0b00001111);
	dig_HS1[4] = (i2c_read_ack() >> 4) + ((uint16_t)i2c_read_ack() <<4);
	dig_HS1[5] = i2c_read_nack();
	i2c_stop();

	//************************ Temperatur berechnen *********************************
	int32_t var1tempS2, var2tempS2;
	adc_TS2 >>= 4;
	var1tempS2  = ((((adc_TS2>>3) - ((int32_t)dig_tS2[0] <<1))) * ((int32_t)dig_tS2[1])) >> 11;
	var2tempS2  = (((((adc_TS2>>4) - ((int32_t)dig_tS2[0])) * ((adc_TS2>>4) - ((int32_t)dig_tS2[0]))) >> 12) *
	((int32_t)dig_tS2[2])) >> 14;
	t_fineS2 = var1tempS2 + var2tempS2;
	float T2  = (t_fineS2 * 5 + 128) >> 8;
	temperatureS2 = T2;

	//*************************** Pressure Berechnen *********************************
	int64_t var1pressS2, var2pressS2, pS2;
	adc_PS2 >>= 4;
	var1pressS2 = ((int64_t)t_fineS2) - 128000;
	var2pressS2 = var1pressS2 * var1pressS2 * (int64_t)dig_PS2[5];
	var2pressS2 = var2pressS2 + ((var1pressS2*(int64_t)dig_PS2[4])<<17);
	var2pressS2 = var2pressS2 + (((int64_t)dig_PS2[3])<<35);
	var1pressS2 = ((var1pressS2 * var1pressS2 * (int64_t)dig_PS2[2])>>8) +
	((var1pressS2 * (int64_t)dig_PS2[1])<<12);
	var1pressS2 = (((((int64_t)1)<<47)+var1pressS2))*((int64_t)dig_PS2[0])>>33;
	pS2 = 1048576 - adc_PS2;
	pS2 = (((pS2<<31) - var2pressS2)*3125) / var1pressS2;
	var1pressS2 = (((int64_t)dig_PS2[8]) * (pS2>>13) * (pS2>>13)) >> 25;
	var2pressS2 = (((int64_t)dig_PS2[7]) * pS2) >> 19;
	pS2 = ((pS2 + var1pressS2 + var2pressS2) >> 8) + (((int64_t)dig_PS2[6])<<4);
	pressureS2 = pS2/256/100;

	//*********************** Berechnung Humidity*******************
	double var_HS2;
	adc_HS2 = read8(0XFD, addressS2);

	var_HS2 = (((double)t_fineS2) - 76800.0);
	var_HS2 = (adc_HS2 - (((double)dig_HS2[3]) * 64.0 + ((double)dig_HS2[4]) / 16384.0 * var_HS2)) *
	(((double)dig_HS2[3]) / 65536.0 * (1.0 + ((double)dig_HS2[5]) / 67108864.0 * var_HS2 *
	(1.0 + ((double)dig_HS2[2]) / 67108864.0 * var_HS2)));
	var_HS2 = var_HS2 * (1.0 - ((double)dig_HS2[0]) * var_HS2 / 524288.0);
	humidityS2 = adc_HS2;
}

int16_t WeatherSensor::getTemperatureS1()
{
	if (temperatureS1 == 0)
		readData();


	return (int16_t)temperatureS1;
}

uint16_t WeatherSensor::getPressureS1()
{
	if (pressureS1 == 0)
		readData();

	return (uint16_t)pressureS1;
}

uint16_t WeatherSensor:: getHumidityS1()
{
	if (m_humidityS1 == 0)
		readData();

	return (uint16_t)humidityS1;
}

int16_t WeatherSensor::getTemperatureS2()
{
	if (temperatureS2 == 0)
	readData();

	return (int16_t)temperatureS2;
}

uint16_t WeatherSensor::getPressureS2()
{
	if (pressureS2 == 0)
	readData();

	return (uint16_t)pressureS2;
}

uint16_t WeatherSensor:: getHumidityS2()
{
	if (humidityS2 == 0)
	readData();

	return (uint16_t)humidityS2;
}

uint8_t WeatherSensor::evaluateSignal()
{
	// TODO: get signal strength from previous measured distance
	return 0;
}
