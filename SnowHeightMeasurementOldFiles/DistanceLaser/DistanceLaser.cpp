/*
* DistanceLaser.cpp
*
* Created: 03.03.2017 17:04:35
*  Author: Nikolas Pacik
*/

#include <avr\io.h>
#include "DistanceLaser.h"

extern "C" 
{
	#include "../I2C_MASTER/i2c_master.h"
}

#define defaultLaserAddress 0x62
#define defaultLaserConfiguration 0x00

DistanceLaser::DistanceLaser()
{
	i2c_init();
	write_i2c(laserAddress, 0x00, defaultLaserConfiguration, 1);  // initialize laser with default configuration
	
	m_done = false; 
}

// iniate with custom set I2C address
DistanceLaser::DistanceLaser(uint8_t address)
{
	// init laser with custom I2C address
	i2c_init();
	setAddress(address);
	laserAddress = address;

	m_done = false; 
}

// changes the I2C address of the device
uint8_t DistanceLaser::setAddress(bool keepAsDefault)
{
	uint8_t receivedData[2];
	
	// read two byte serial number
	read_i2c(laserAddress, 0x96, receivedData, 2);
	write_i2c(laserAddress, 0x18, &receivedData[0], 1);	// write received serial low byte to register 
	write_i2c(laserAddress, 0x19, &receivedData[1], 1);	// write received serial high byte to register 
	
	if(keepAsDefault){
		write_i2c(laserAddress, 0x1e, &commandSetAddressAsDefault, 1);		// set new address and keep as default after 
	}
	else{
		write_i2c(laserAddress, 0x1e, &commandSetAddressAsTemporary, 1);		// set new address 
	}
}

// measures and returns distance in cm 
uint16_t DistanceLaser::getDistance()
{
	uint8_t receivedData[2];					// used variable for received data from twi (max two bytes)
	uint16_t calculatedDistance;
	
	m_done = false; 
	
	//uint8_t signalStrength = getSignalStrength(); 
	
	write_i2c(laserAddress, 0x00, &commandGetAcquisition, 1);	// initiate distance acquisition
	read_i2c(laserAddress, 0x01, receivedData, 1);				// check laser status
	
	while((receivedData[0] >> 0) & 1){
		read_i2c(laserAddress, 0x01, receivedData, 1);			// check laser status until laser is done with distance acquisition
	}
	
	read_i2c(laserAddress, 0x8f, receivedData, 2);				// get calculated distance from laser register
	calculatedDistance = (receivedData[0] << 8) + receivedData[1];
	
	m_done = true; 
	
	return calculatedDistance;
}

uint8_t DistanceLaser::getSignalStrength()
{
	uint8_t receivedData[1]; 
	
	read_i2c(laserAddress, 0x5b, receivedData, 1); 
	
	return receivedData[0]; 
}

// write to I2C register
uint8_t DistanceLaser::write_i2c(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint8_t length)
{
	if (i2c_start((devaddr << 1) + I2C_WRITE)) return 1;

	i2c_write(regaddr);

	for (uint16_t i = 0; i < length; i++)
	{
		if (i2c_write(data[i])) return 1;
	}

	i2c_stop();

	return 0;
}

// read from I2C register
uint8_t DistanceLaser::read_i2c(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint8_t length)
{
		if (i2c_start((devaddr << 1) + I2C_WRITE)) return 1;

		i2c_write(regaddr);

		i2c_stop();

		if (i2c_start((devaddr << 1) + I2C_READ)) return 1;

		for (uint16_t i = 0; i < (length-1); i++)
		{
			data[i] = i2c_read_ack();
		}
		
		data[(length-1)] = i2c_read_nack();

		i2c_stop();

		return 0;
}