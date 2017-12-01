/*
 * monitored_serial.h
 *
 * Created: 2017/11/28 上午 09:24:05
 *  Author: user
 */ 


#ifndef MONITORED_SERIAL_H_
#define MONITORED_SERIAL_H_

#include "../config.h"


#ifdef ECHO_MODEM_INPUT

#include "Arduino.h"

class MonitoredSerial : public Stream
{
public:
	MonitoredSerial(Stream *serial)
	{
		this->serial = serial;
	}
	
	virtual size_t write(uint8_t x);
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();
	void direct_write_bytes(const uint8_t *, size_t);
	
	Stream *serial;
};

#endif


#endif /* MONITORED_SERIAL_H_ */