/*
 * DistanceLaser.h
 *
 * Created: 03.03.2017 17:02:11
 *  Author: Nikolas Pacik
 */ 

#ifndef DISTANCELASER_H_
#define DISTANCELASER_H_

class DistanceLaser
{
	public:
		DistanceLaser();
		DistanceLaser(uint8_t);
		
		bool isDone() { return m_done;  };

		uint8_t getSignalStrength();
		uint16_t getDistance();
		uint8_t setAddress(bool keepAsDefault); 
		
	private:
		/* properties */
		bool m_done = false; 
		uint8_t laserAddress = 0x62;	// default address is 0x62
		uint8_t commandGetAcquisition = 0x04;
		uint8_t commandSetAddressAsDefault = 0x08;
		uint8_t commandSetAddressAsTemporary = 0x00;
	 	
		/* methods */ 
		uint8_t write_i2c(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint8_t length);		// write to I2C register
		uint8_t read_i2c(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint8_t length);		// read from I2C register
		
	
};

#endif /* DISTANCELASER_H_ */