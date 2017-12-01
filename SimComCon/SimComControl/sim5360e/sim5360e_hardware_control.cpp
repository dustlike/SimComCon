/*
 * sim5360e_hardware_control.cpp
 *
 * Created: 2017/11/28 上午 09:05:36
 *  Author: user
 */ 


#include "sim5360e.h"
#include "trace.h"


#define MODEM_PIN_PWRKEY 7
#define MODEM_PIN_GPIO40 A1		//the power state

#define MODEM_BAUD 57600



void SIM5360E::begin()
{
	digitalWrite(MODEM_PIN_PWRKEY, HIGH);
	pinMode(MODEM_PIN_PWRKEY, OUTPUT);
	
	pinMode(MODEM_PIN_GPIO40, INPUT);
	
	setPIN("7259", nullptr);
	
#ifdef ECHO_MODEM_INPUT
	static_cast<HardwareSerial *>(monitored_serial.serial)->begin(MODEM_BAUD);
#else
	static_cast<HardwareSerial *>(uart)->begin(MODEM_BAUD);
#endif
}


void SIM5360E::end()
{
	//static_cast<HardwareSerial *>(uart)->end();
}


bool SIM5360E::powerState()
{
	return analogRead(MODEM_PIN_GPIO40) > 340;
}


void SIM5360E::pressPowerKey()
{
	digitalWrite(MODEM_PIN_PWRKEY, HIGH);
	debugSerial.println("[PWRKEY] press");
	digitalWrite(MODEM_PIN_PWRKEY, LOW);
	delay(800);
	digitalWrite(MODEM_PIN_PWRKEY, HIGH);
}


bool SIM5360E::powerOn()
{
	debugSerial.println(F("SIM5360E::powerOn()"));
	
	constexpr int maxRetry = 3;
	constexpr uint32_t pollingTime = 8000;
	
	uint32_t start;
	uint32_t waiting = 0;
	
	for(int i = 0; i < maxRetry; i++)
	{
		start = millis();
		do
		{
			if (powerState()) return true;
			
		} while(millis() - start < waiting);
		
		waiting = pollingTime;
		
		pressPowerKey();
	}
	
	return false;
}


bool SIM5360E::powerOff()
{
	debugSerial.println(F("SIM5360E::powerOff()"));
	
	constexpr int maxRetry = 3;
	constexpr uint32_t pollingTime = 3000;
	
	uint32_t start;
	uint32_t waiting = 0;
	
	for(int i = 0; i < maxRetry; i++)
	{
		start = millis();
		do
		{
			if (!powerState()) return true;
			
		} while(millis() - start < waiting);
		
		waiting = pollingTime;
		
		pressPowerKey();
	}
	
	return false;
}
