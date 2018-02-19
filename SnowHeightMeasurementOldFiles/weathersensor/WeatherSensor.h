/*
 * WeatherSensor.h
 *
 * Created: 03.03.2017 17:02:11
 *  Author: Aleksandar V., Stefan S.
 */ 

#ifndef WEATHERSENSOR_H_
#define WEATHERSENSOR_H_

#define BME280_TEMPDATA				0xF7
#define BME280_TempPressMode1		0x4E
#define BME280_HumidityMode1		0xF2
#define BME280_TEMPCOMP				0x88
#define BME280_HUMCOMP				0xE1
#define BME280_Hoversa0				0X0
#define BME280_Hoversa1				0x1

#define BME280_REGISTER_CONTROLHUMID 0xF2,
#define BME280_REGISTER_STATUS       0XF3,
#define BME280_REGISTER_CONTROL      0xF4,
#define BME280_REGISTER_CONFIG       0xF5,
#define BME280_REGISTER_PRESSUREDATA 0xF7,
#define BME280_REGISTER_TEMPDATA     0xFA,
#define BME280_REGISTER_HUMIDDATA    0xFD


enum
{
	BME280_REGISTER_DIG_T1              = 0x88,
	BME280_REGISTER_DIG_T2              = 0x8A,
	BME280_REGISTER_DIG_T3              = 0x8C,

	BME280_REGISTER_DIG_P1              = 0x8E,
	BME280_REGISTER_DIG_P2              = 0x90,
	BME280_REGISTER_DIG_P3              = 0x92,
	BME280_REGISTER_DIG_P4              = 0x94,
	BME280_REGISTER_DIG_P5              = 0x96,
	BME280_REGISTER_DIG_P6              = 0x98,
	BME280_REGISTER_DIG_P7              = 0x9A,
	BME280_REGISTER_DIG_P8              = 0x9C,
	BME280_REGISTER_DIG_P9              = 0x9E,

	BME280_REGISTER_DIG_H1              = 0xA1,
	BME280_REGISTER_DIG_H2              = 0xE1,
	BME280_REGISTER_DIG_H3              = 0xE3,
	BME280_REGISTER_DIG_H4              = 0xE4,
	BME280_REGISTER_DIG_H5              = 0xE5,
	BME280_REGISTER_DIG_H6              = 0xE7,
};

class WeatherSensor
{
	public:
		WeatherSensor(uint8_t address);

		int16_t getTemperatureS1();
		uint16_t getHumidityS1();
		uint16_t getPressureS1();
		int16_t getTemperatureS2();
		uint16_t getHumidityS2();
		uint16_t getPressureS2();

	private:
		int32_t temperatureS1;
		uint64_t m_humidityS1;
		uint32_t humidityS1;
		uint32_t pressureS1;
		int32_t   t_fineS1;

		int32_t temperatureS2;
		uint64_t m_humidityS2;
		uint32_t humidityS2;
		uint32_t pressureS2;
		int32_t   t_fineS2;
		//RAW Wert
		int32_t adc_PS1;
		int32_t adc_TS1;
		int32_t adc_HS1;
		int32_t adc_PS2;
		int32_t adc_TS2;
		int32_t adc_HS2;
		void readData();
		void CompensateData();

		uint8_t evaluateSignal();
		uint8_t sensorAddress; //0b1110111;
	
};

#endif /* WEATHERSENSOR_H_ */