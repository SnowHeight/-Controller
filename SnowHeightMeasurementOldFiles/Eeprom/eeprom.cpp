/*
 * eeprom.cpp
 *
 * Created: 27.02.2017 10:36:31
 *  Author: arfuetsch
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include "eeprom.h"
#include "../general.h"

//uint8_t EEMEM enemParemeters;

//uint16_t iValueregister;

// AF - Array im EEPROM Speichern

/**
 * \brief 
 * Constructor
 * \param bClearEEPROM
 * If the value of bClearEEPROM is true, all user-defined parameters will be set to 0. 
 * \return 
 */
EEPROM::EEPROM(bool bClearEEPROM)
{
	if (bClearEEPROM)
		clearEeprom();
	
	ReadFromEeprom();
}

/**
 * \brief 
 * Sets the user-defined parameters to 0.
 * \return void
 */
void EEPROM::clearEeprom()
{
	m_oParameters.nHeight = 0;
	m_oParameters.nUltraSonicDelay = 0;
	m_oParameters.nServoDrivingTime = 0;
	m_oParameters.nLaserDelay = 0;
	m_oParameters.bWatchdog = 0;
	m_oParameters.nPowerSaveVoltage = 0;
	m_oParameters.nCountLasermeasurements = 0;
	WriteToEeprom();									
}

//AF - Read/Write Functions for EEPROM.

/**
 * \brief 
 * Saves the user defined parameters to the EEPROM 
 * \return void
 */
void EEPROM::WriteToEeprom()
{
	//AF - Update(pointer structaddress, pointer EEPROM storage address, size of struct)
	eeprom_update_block ((const void*)&m_oParameters, (void*)EEPROM_ParameterStart , sizeof(m_oParameters));
}

/**
 * \brief 
 * Fetches the user-defined parameters from the EEPROM.
 * \return void
 */
void EEPROM::ReadFromEeprom()
{
	//AF - Read(pointer structaddress, pointer EEPROM storage address, size of struct)
	eeprom_read_block ((void *)&m_oParameters ,(const void*)EEPROM_ParameterStart , sizeof(m_oParameters));
}

void EEPROM::SetValueX(uint16_t ValueX)
{
	eeprom_update_word (( uint16_t *)iValueregister, ValueX);
}

void EEPROM::SetValueY(uint16_t ValueY)
{
	// AF - Plus 2 da wir zwei register weiter müssen(16 bit).
	iValueregister += 2;
	eeprom_update_word (( uint16_t *)iValueregister, ValueY);
}

bool EEPROM::GotoFirstValue()
{
	nCurrentLasercount = 0; //zero zero
	
	iValueregister = EEPROM_MatrixStart;
	return true;
}

uint16_t EEPROM::GetValueX()
{
	uint16_t Value = eeprom_read_word(( uint16_t *)iValueregister);
	return Value;
}

uint16_t EEPROM::GetValueY()
{
	// AF - Plus 2 da wir zwei register weiter müssen(16 bit).
	iValueregister += 2;
	uint16_t Value = eeprom_read_word(( uint16_t *)iValueregister);
	return Value;
}

bool EEPROM::GotoNextValue()
{
	nCurrentLasercount++;
	// AF - Plus 2 da wir zwei register weiter müssen(16 bit).
	iValueregister += 2;
	if (nCurrentLasercount >= m_oParameters.nCountLasermeasurements) // schreib 300
	{
		nCurrentLasercount = 0;
		return false;
	}
	else
	{
		return true;
	}
}