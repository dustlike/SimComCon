/*
 * generic_AT_commander.h
 *
 * Created: 2017/5/8 上午 10:57:13
 *  Author: user
 */ 


#pragma once

#include "generic_AT_lexer.h"


class GenericATCmder
{
	public:
	
	GenericATCmder(GATL::ATResponsePattern *table, size_t szTable, void (*patternRegister)())
		: lexer(table, szTable, patternRegister)
	{
		
	}
	
	GATL lexer;
};
