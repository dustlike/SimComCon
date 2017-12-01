/*
 * CMTCS.h
 *
 * Cooperative MultiTask library based on Context Switch for AVR-8
 *
 * Created: 2017/9/18 14:27:51
 *  Author: dust
 */ 


#ifndef CMTCS_H_
#define CMTCS_H_


#include "Arduino.h"


class CMTCS
{
public:
	enum {MAX_TASK_COUNT = 7};
	
	template<void(*routine)(void), size_t stack_size>
	inline bool createTask(char(*)[stack_size >= 192] = 0)
	{
		static uint8_t stack[stack_size];
		return registTask(routine, stack + stack_size);
	}
	
	static void startMultiTask(void);
	
private:
	static bool registTask(void(*routine)(void), uint8_t *stack_bottom);
};

extern CMTCS cmtcs;


#endif /* CMTCS_H_ */
