/*
 * simple_timer.h
 *
 * Created: 2017/5/3 下午 05:28:41
 *  Author: user
 */ 


#ifndef SIMPLETIMER_H_
#define SIMPLETIMER_H_


#include "Arduino.h"


class SimpleTimer
{
public:
	inline void reset(void)
	{
		memTime = millis();
	}
	
	inline bool timeout(uint32_t time_limit)
	{
		return millis() - memTime >= time_limit; 
	}
	
private:
	uint32_t memTime;
};



#endif /* SIMPLETIMER_H_ */