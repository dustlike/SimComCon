/*
 * monitored_serial.cpp
 *
 * Created: 2017/9/22 下午 02:27:53
 *  Author: user
 */ 



#include "monitored_serial.h"


#ifdef ECHO_MODEM_INPUT

size_t MonitoredSerial::write(uint8_t x)
{
	monitorSerial.write(x);
	serial->write(x);
}


int MonitoredSerial::available()
{
	return serial->available();
}


int MonitoredSerial::read()
{
	return serial->read();
}


int MonitoredSerial::peek()
{
	return serial->peek();
}


void MonitoredSerial::flush()
{
	serial->flush();
}


void MonitoredSerial::direct_write_bytes(const uint8_t *src, size_t length)
{
	serial->write(src, length);
}

#endif