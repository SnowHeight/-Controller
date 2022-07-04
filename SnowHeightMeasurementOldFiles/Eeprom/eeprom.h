/*
 * eeprom.h
 *
 * Created: 27.02.2017 11:15:03
 *  Author: arfuetsch
 */ 
 
//ToDo 

//Array f�r Matrix speichern

//Function zum auslesen des Arrays erstellen
// - Function started
// - Register auslesen
// - Register weitersetzen
//R�ckgabewert?

//Function zum zur�cksetzen des Registers w�hlen

class EEPROM 
{
	public:
		EEPROM(bool bClearEEPROM = false);
	//AF - Struct for Parameters
		struct eepromstr_Parameters
		{
			//Height of the SHM to the ground in millimeters.
			uint16_t nHeight;
			//Timespan of the cycle for the ultrasonic and environmental measurements.
			uint8_t nUltraSonicDelay;
			//Timespan of the cycle for the laser measurements.
			uint16_t nLaserDelay;
			//Timespan between servo driving and laser measurement.S
			uint16_t nServoDrivingTime;
			//Bool for Watchdog Timer
			bool bWatchdog;		
			//Value for the power save voltage, Value = voltage * 10 
			uint8_t nPowerSaveVoltage;	
			//Count of Laser mearurements
			uint16_t nCountLasermeasurements;
		} m_oParameters;
		//Array for the lasermeasurement matrix

		//uint16_t iLaserMatrix[500][2];

		void clearEeprom();
		void WriteToEeprom();
		void ReadFromEeprom();

		//uint16_t* GetMeasurementpoint(uint16_t iValueregister);
		bool GotoFirstValue();
		uint16_t GetValueX();
		uint16_t GetValueY();
		void SetValueX(uint16_t ValueX);
		void SetValueY(uint16_t ValueY);
		bool GotoNextValue();
		uint16_t nCurrentLasercount = 0;
	private:
		uint16_t iValueregister;
		
};

#ifndef EEPROM_H_
#define EEPROM_H_
#endif /* EEPROM_H_ */