#ifndef SDCARD_H
#define SDCARD_H

#include <inttypes.h>

class DataManager
{
public:
    DataManager(unsigned char* strFilePath, unsigned char* strKeys);

	unsigned char* getFilePath() { return m_strFilePath; }
    unsigned char* getKeys() { return m_strKeys; }
	
	void resetLineNumber() { m_lineNumber = 0; }

private:
	unsigned char* m_strFilePath;
	unsigned char* m_strKeys;
	uint32_t m_lineNumber = 0;
	uint32_t seek = 0;

    bool fileExists();
    void initFile();
    char* getNextID();

public:
    unsigned char* readData(uint32_t lineNumber);
    void writeData(char *strData);
    void deleteData();
	static bool initSDCard();
};

class LaserData : public DataManager
{
public:
    LaserData();

private:
    unsigned char* getFilePath() { return (unsigned char*)"LD.CSV"; }
    unsigned char* getKeys() { return (unsigned char*)"ID;DateTime;LDID;Distance\0"; }
};

class ErrorData : public DataManager
{
public:
    ErrorData();

private:
    unsigned char* getFilePath() { return (unsigned char*)"ED.CSV"; }
    unsigned char* getKeys() { return (unsigned char*)"ID;DateTime;Module;Error\0"; }
};

class GenericData : public DataManager
{
public:
    GenericData();

private:
    unsigned char* getFilePath() { return (unsigned char*)"GD.CSV"; }
    unsigned char* getKeys() { return (unsigned char*)"ID;DateTime;Module;Value\0"; }
};

#endif // SDCARD_H
