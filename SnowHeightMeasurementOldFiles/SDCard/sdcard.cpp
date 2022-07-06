#include <stdio.h>
#include <stdlib.h>

#include "sdcard.h"

extern "C" {
	#include "vendor/mmc_config.h"
	#include "vendor/file.h"
	#include "vendor/fat.h"
	#include "vendor/mmc.h"
	#include "vendor/uart.h"
}

#include <string.h>

DataManager::DataManager(unsigned char* strFilePath, unsigned char* strKeys) :
	m_strFilePath(strFilePath),
	m_strKeys(strKeys)
{	
	uputs((unsigned char*)"!!!IMPORTANT!!!->\0");
	uputs(m_strFilePath);
    initFile();
}

LaserData::LaserData() :
	DataManager(getFilePath(), getKeys())
{

}

ErrorData::ErrorData() :
	DataManager(getFilePath(), getKeys())
{

}

GenericData::GenericData() :
	DataManager(getFilePath(), getKeys())
{

}

bool DataManager::initSDCard()
{
	uinit();
	if (FALSE == mmc_init() || FALSE == fat_loadFatData())
	{
		uputs((unsigned char*)" *** mmc init or fat load failed *** \0");
		return FALSE;
	}
	
	return TRUE;
}

///
/// \brief fileExists Check if the FilePath is correct and exists
/// \return File exists?
///
bool DataManager::fileExists()
{		
	uputs((unsigned char*)" *** startly *** \0");
    if (MMC_FILE_OPENED == ffopen(m_strFilePath,'r'))
    {
		uputs((unsigned char*)" *** okely *** \0");
		ffclose();
		return 1;
    }

	return 0;
}

///
/// \brief DataManager::initFile Initializes a file recreating it if it does not exist and adding the corresponding KeyHeader
///
void DataManager::initFile()
{
    if (!fileExists())
    {
		uputs((unsigned char*)" *** not existly *** \0");
		uputs((unsigned char*)strcat((char*)m_strFilePath, (char*)"\0"));
        ffopen(m_strFilePath,'c');
		//ffwrite(0x00);
		ffclose();
		
		
		uputs((unsigned char*)" *** before write *** \0");

		writeData((char*)m_strKeys);
		
		uputs((unsigned char*)" *** after write *** \0");
    }
}

char* DataManager::getNextID()
{
    // @Mellauner: Führe hier eine Lesefunktion aus die die letzte Zeile ließt und alles was vor dem ersten ; ist, ist unsere letzte ID
    //             Dannach muss nur noch der wert um 1 erhöht werden
    char* strNextID = "1";
    return strNextID;
}

///
/// \brief DataManager::readData Reads the Data of a file
/// \return The read Data
///
unsigned char* DataManager::readData(uint32_t lineNumber)
{
    ffopen(m_strFilePath,'r');

	seek = 0;
	m_lineNumber = 0;

	unsigned char* strLine = (unsigned char*)malloc(1);
	char currChar;
	uputs((unsigned char*)"\r /// reading now === \r");
	do
	{
		currChar = (char)ffread();
		if (m_lineNumber == lineNumber)
		{
			strLine = (unsigned char*)realloc(strLine, sizeof(strLine) + 1);
			memcpy(strLine + seek, &currChar, strlen(&currChar) + 1);
			uputc(currChar);
		}
		
		if (currChar == '\n')
		{
			if (m_lineNumber == lineNumber)
			{
				//uputs((unsigned char*)"\r /// reading now === \r");				
				//uputs(strLine);
				uputs((unsigned char*)"\r === reading end /// \r");	
										
				ffclose();
				++m_lineNumber;
				return strLine;
			}
						
			++m_lineNumber;
		}
	} while(++seek < file.length);
	 
	ffclose();
	
	uputs((unsigned char*)"\r LINE NOT FOUND !!!\r");
    return (unsigned char*)"";
}  

///
/// \brief DataManager::writeData Appends data to the SDCard
/// \param strData Semicolumn seperated list of data
///
void DataManager::writeData(char* strData)
{
	uint16_t nStringLength = strlen(strData);
	
	//Simple pre-calculations when the Data is not the Keyrow
	//Get the next ID and convert it to a string
	char *strNextID = getNextID();

	//Get the Datestring
	char strDateTime[11] = ";DATETIME;";

	if ((unsigned char*)strData != getKeys())
	{
		//Merge ID;DateTime;Data
		nStringLength = strlen(strNextID) + strlen(strDateTime) + strlen(strData);
	}
	
	char strFullData[nStringLength];
	
	//Combine strings together to form the full csv row (only do this if it's not the Keyrow)
	if ((unsigned char*)strData != getKeys())
	{
		strncpy(strFullData, strNextID, nStringLength);
		strcat(strFullData, strDateTime);
		strcat(strFullData, (char*)strData);
	}
	else
		strcat(strFullData, (char*)strData);

	//Write data to the file
	uputs((unsigned char*)" *** before inner write *** \0");
	ffopen(m_strFilePath, 'r');
	uputs((unsigned char*)" *** after inner write file open *** \0");

	ffseek(file.length);
	uputs((unsigned char*)" *** after inner write file seek *** \0");

	ffwrites((unsigned char*)strFullData);
	ffwrite(0x0D);		// new line in file
	ffwrite(0x0A);
	uputs((unsigned char*)" *** after inner write file write *** \0");

	ffclose();
	uputs((unsigned char*)" *** after inner write *** \0");

}

///
/// \brief DataManager::deleteData Deletes the contents of a file and reinitializes it
///
void DataManager::deleteData()
{
	ffrm(m_strFilePath);

    initFile();
}
