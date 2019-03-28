/*
 * sim5360e.h
 *
 * Created: 2017/6/29 下午 04:07:10
 *  Author: user
 */


#pragma once

#include "generic_AT_commander.h"


class SIM5360E_ATCEH : public GenericATCmder
{
	public:
	enum ResponseToken
	{
		RT_OK,
		RT_ERROR,
		RT_CPIN,
		RT_CMERROR,
		RT_ICCID,
		RT_CGREG,
		RT_NETOPEN,
		RT_CIPOPEN,
		RT_CIPSEND,
		RT_CIPRXGET,
		RT_CCLK,
		RT_START,
		RT_CGPSINFO,
		RT_CIPCLOSE,
		RT_CREG,
		RT_PDPDEACT,
		RT_CGATT,
		RT_CSQ,
		RT_IPCLOSE,
		//所有Response Token都必須定義在LAST_RESPONSE_TOKEN之前
		LAST_RESPONSE_TOKEN
	};
	
	SIM5360E_ATCEH()
		: GenericATCmder(patternTable.table, patternTable.TABLE_SIZE, set_response_patterns)
	{
		
	}
	
	static GATL::PatternTable<LAST_RESPONSE_TOKEN> patternTable;
	static void set_response_patterns();
#ifdef SCC_TOKEN_MONITOR
	static void printToken(GATL::Token, GATL *);
#endif
};




class SIM5360E : public SIM5360E_ATCEH, public ISercomIRQ
{
	public:
	
	void begin(uint32_t baudrate, SERCOM *scm, uint16_t pinRX, uint16_t pinTX, SercomRXPad padRX, SercomUartTXPad padTX);
	void feed(char c);
	virtual void IRQHandler();
	
	SERCOM_USART *sercom;
};