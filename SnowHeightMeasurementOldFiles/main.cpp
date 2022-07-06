/*
 * RealTimeClock.cpp
 *
 * Created: 13.03.2017 16:10:54
 * Author : arfuetsch
 */
#include "general.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <string.h>
#include <avr/wdt.h>


#include "I2C_MASTER/i2c_master.h"
#include "Eeprom/eeprom.h"
#include "DS3231/DS3231.h"
#include "usonic/usonic.h"
#include "usart/usart.h"
#include "SDCard/sdcard.h"
#include "weathersensor/WeatherSensor.h"
#include "ServoControl/Servo.h"
#include "usart/myUart.h"

bool bLaserError = false;
uint8_t iOldLasercycle = 0;
uint8_t counter = 0;

void AddEEPROMTestData()
{
	EEPROM oEeprom;
	oEeprom.clearEeprom();
	oEeprom.m_oParameters.nCountLasermeasurements = 300;
	oEeprom.m_oParameters.nHeight = 900;
	oEeprom.m_oParameters.nPowerSaveVoltage = 230;
	oEeprom.m_oParameters.nServoDrivingTime = 500;
	oEeprom.m_oParameters.nUltraSonicDelay = 10;
	oEeprom.m_oParameters.nLaserDelay = 6;
	oEeprom.WriteToEeprom();
	uint16_t iLaserValues[oEeprom.m_oParameters.nCountLasermeasurements][2];

	uint16_t iTestValueX = 450; // 450
	uint16_t iTestValueY = 450; // 450
	bool bRight = true;
	uint16_t iCounter = 0;
	//AF - Test mit Lasermatrix 300 = 20 * 15
	
	for(uint16_t iYCounter = 0; iYCounter < 15; iYCounter++)
	{
		for(uint16_t iXCounter = 0; iXCounter < 20; iXCounter++)
		{
			iLaserValues[iCounter][0] = iTestValueX;
			iLaserValues[iCounter][1] = iTestValueY;
			iCounter++;
			
			if (bRight == true)
			{
				if (iTestValueX < 1350)
				{
					iTestValueX = 45 + iTestValueX;
				}
			}
			else
			{
				if (iTestValueX > 450)
				{
					iTestValueX -= 45;
				}
			}
		}

		bRight = !bRight;
		
		if (iTestValueY < 1350)
		{
			iTestValueY += 60;
		}
	}
	
	bool bWrite = oEeprom.GotoFirstValue();
	oEeprom.m_oParameters.nCountLasermeasurements = 300;
	oEeprom.nCurrentLasercount = 0;
	uint16_t iWriteCounter = 0;
	while (bWrite)
	{
		oEeprom.SetValueX(iLaserValues[iWriteCounter][0]);
		oEeprom.SetValueY(iLaserValues[iWriteCounter][1]);
		bWrite = oEeprom.GotoNextValue();
		iWriteCounter++;
	}
}

/**
 * \brief 
 * 
 * \param zahl
 * \param message
 * 
 * \return void
 */
void AddIntegerToChar(long zahl, char* message)
{
	long restzahl = zahl;
	int8_t rest = 0;
	char strZahl[11];
	uint8_t stelle = 0;
	
	if(restzahl<0)	{
		while(restzahl != 0){
			rest = restzahl % 10;
			restzahl /= 10;
			strZahl[stelle] = ((rest * -1) + 48);
			stelle++;
		}
		strZahl[stelle] = '-';
		stelle++;
	}
	else
	{
		while(restzahl != 0){
			rest = restzahl % 10;
			restzahl /= 10;
			strZahl[stelle] = (rest + 48);
			stelle++;
		}
	}
	
	uint8_t counter = strlen(message);
	for (int i = stelle-1; i >= 0; i--)	{
		 message[counter++] = strZahl[i];
	}
}

uint8_t GetFailureCycle(uint8_t alarmcycle)
{
	uint8_t year = 0;
	uint8_t month = 0;
	uint8_t day = 0;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t second = 0;
	rtc.DS3231_getdate(year, month, day, hour, minute, second);

	uint8_t hourcount = 60 / alarmcycle;
	uint8_t minutecount = (60 - minute) / alarmcycle;

	uint8_t cycle = 0;
	//AF - Berechne Zeit bis Mittag.
	do
	{
		cycle += hourcount;
		hour += 1;
		if (hour == 24)
		{
			hour = 0;
		}
	}
	while(hour == 12);
	do
	{
		cycle += minutecount;
		minute += alarmcycle;
		if (minute >= 60)
		{
			minute = 0;
		}
	}
	while(minute != 0);
	return cycle;
}

// Hier jede Klasse einzeln Testen und die anderen derweil auskommentieren
void Measurementcycle(uint8_t alarmcycle, uint8_t &counterlasercycle){
	rtc.DS3231_Alarmcounter += 1;
	//AF - Clear Alarmflag;
	rtc.DS3231_clearalarmflag();
	//AF - Set next alarm;
	rtc.DS3231_setalarm(alarmcycle);
	wdt_reset();
	//AF - start weather measurement
	WeatherSensor oWeathersensor(WeatherAddress);
	//AF - Sensor1 Pressure
	uint32_t nPress1 = oWeathersensor.getPressureS1();
	wdt_reset();
	//AF - Sensor1 Temperature
	int32_t nTemp1 = oWeathersensor.getTemperatureS1();
	wdt_reset();
	//AF - Sensor1 Humidity
	uint32_t nHum1 = oWeathersensor.getHumidityS1();
	wdt_reset();
	//AF - Sensor2 Pressure
	uint32_t nPress2 = oWeathersensor.getPressureS2();
	wdt_reset();
	//AF - Sensor2 Temperature
	int32_t nTemp2 = oWeathersensor.getTemperatureS2();
	wdt_reset();
	//AF - Sensor2 Humidity
	uint32_t nHum2 = oWeathersensor.getHumidityS2();
	wdt_reset();
	//AF - start ultrasonic measurement
	uint16_t nUsonicDistance = uSonic.getAverageDistance();
	wdt_reset();
	//AF - check if Laser measurement is necessary
	if (rtc.DS3231_Alarmcounter >= counterlasercycle)
	{
		//AF - Check errorstate, set cycle back to normal
		if (bLaserError)
		{
			bLaserError = false;
			counterlasercycle = iOldLasercycle;
		}
		rtc.DS3231_Alarmcounter = 0;
		//AF - start Laser measurement
		wdt_reset();
		//Laser Fehlerstatus auslesen.
		bool bServoSuccess = servo.LaserMeasure();
		//AF - Save old Lasercycle, calculate next measurement and set Errorstate.
		if(bServoSuccess == false)
		{
			iOldLasercycle = counterlasercycle;
			counterlasercycle = GetFailureCycle(alarmcycle);
			bLaserError = true;
		}
	}
	wdt_reset();
	//AF - send all data to the SD Card as struct.
	//AF - Lasermessdaten werden vom Laser aus gesendet.
	char strUltrasonicData[50] = "Ultrasonic; ";
	wdt_reset();
	char strWS1PressData[50] = "Weathersensor1_Press; ";
	wdt_reset();
	char strWS1TempData[50] = "Weathersensor1_Temp; ";
	wdt_reset();
	char strWS1HumData[50] = "Weathersensor1_Hum; ";
	wdt_reset();
	char strWS2PressData[50] = "Weathersensor2_Press; ";
	wdt_reset();
	char strWS2TempData[50] = "Weathersensor2_Temp; ";
	wdt_reset();
	char strWS2HumData[50] = "Weathersensor2_Hum; ";
	wdt_reset();
	AddIntegerToChar(nUsonicDistance, strUltrasonicData);
	AddIntegerToChar(nPress1, strWS1PressData);
	AddIntegerToChar(nTemp1, strWS1TempData);
	AddIntegerToChar(nHum1, strWS1HumData);
	AddIntegerToChar(nPress2, strWS2PressData);
	AddIntegerToChar(nTemp2, strWS2TempData);
	AddIntegerToChar(nHum2, strWS2HumData);
	wdt_reset();
	GenericData oGenericData;
	wdt_reset();
	oGenericData.writeData(strUltrasonicData); //MODULENAME;DATA
	wdt_reset();
	oGenericData.writeData(strWS1PressData);
	wdt_reset();
	oGenericData.writeData(strWS1TempData);
	wdt_reset();
	oGenericData.writeData(strWS1HumData);
	wdt_reset();
	oGenericData.writeData(strWS2PressData);
	wdt_reset();
	oGenericData.writeData(strWS2TempData);
	wdt_reset();
	oGenericData.writeData(strWS2HumData);
}

int main(void)
{	
	//MAIN PROGRAMM
	sei();
	
	// Laden der UART settings
	uartSettings settings = uartSettings();
	
	// Uart mit diesen Settings initialisieren
	myUart uart = myUart(settings);
	
	uart.enable();
	
	
	//AF - Add EEPROM Testdata
	EEPROM oEeprom;
	AddEEPROMTestData();
	//AF - Get alarmcycle from EEPROM
	uint8_t alarmcycle = oEeprom.m_oParameters.nUltraSonicDelay;
	//AF - Get lasercycle from EEPROM
	uint8_t counterlasercycle = oEeprom.m_oParameters.nLaserDelay;
	//AF - set sleep mode to power down
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//AF - start laser measurement in first cycle
	rtc.DS3231_Alarmcounter = oEeprom.m_oParameters.nLaserDelay;
	//AF - enable alarm 1
	rtc.DS3231_enablealarm();

	/************ init Watchdog ************/
	wdt_reset();
	//set up WDT interrupt
	WDTCSR = (1<<WDCE)|(1<<WDE);
	//Start watchdog timer with 4s prescaler
	WDTCSR = (1<<WDE)|(1<<WDP3);
	wdt_enable(WDTO_4S);
	wdt_reset();
	/**************************/
	while (1)
	{
	//if komm mode false
		//AF - Start mesurement - Turn Modules on and off
		CutoffPort |= (1 << ModuleCutoff);	//turn Module Cutoff Pin on
		Measurementcycle(alarmcycle, counterlasercycle);
		CutoffPort &= ~(1 << ModuleCutoff);	//turn Module Cutoff Pin off
		//AF Turn Watchdog off  
		wdt_disable();
		//AF - go into sleep mode
		sleep_mode();
		//AF - Turn Watchdog on
		wdt_enable(WDTO_4S);
	}
}

/*
 * external interrupt on alarm
 * wakes the controller from sleep
 *	Set the next alarm
 *	Run the environment measurement
 *	Run the ultrasonic measurement
 *	Check if the time for the laser measurement is reached and run it
 */
ISR(DS3231_ISR)
{
	USART_Init();
	USART_sendInteger(69);
	rtc.DS3231_clearalarmflag();
	rtc.DS3231_disablealarm();
}

ISR(usonicOVF_ISR)
{
	//Too far -> Increase Overflow
	uSonic.setOverflows(uSonic.getOverflows() + 1);
}

ISR(usonicOCRA_ISR)
{
	//Restarts the 16bit Timer when the TriggerPin is active
	if (usonicPIN & (1 << usonicTriggerPin))
	{
		usonicPrescaler0();
		usonicPort &= ~(1 << usonicTriggerPin);
		EIMSK |= (1 << usonicEchoINT);				// ext. INT 3
	}
}

ISR(usonicEchoISR)
{
	//Check if Interrupt was caused by raising edge or falling edge
	if (usonicPIN & (1 << usonicEchoPin))
	{
		// raising edge detection
		usonicTCNT = 0;
		usonicPrescaler1();
	}
	else
	{
		// falling edge detection
		usonicPrescaler0();
		EIMSK &= ~(1 << usonicEchoINT);
		cli();

		// Get the real Temperature * 10
		uint16_t convertedDistance = uSonic.convertDistance((uSonic.getOverflows() * uint16max) + usonicTCNT, 200);
		uSonic.setDistance(convertedDistance);
		sei();
		uSonic.setReady(true);
		usonicTCNT = 0;
	}
}

// Timer/Counter1 Compare Match C       	(TIMER1_COMPC_vect)
ISR (TIMER1_COMPC_vect) 
{
	servo.Interrupt_Servo();
}

