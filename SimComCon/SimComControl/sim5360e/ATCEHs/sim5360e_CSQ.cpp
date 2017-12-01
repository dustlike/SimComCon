/*
 * sim5360e_CSQ.cpp
 *
 * Created: 2017/11/22 下午 02:28:02
 *  Author: user
 */ 

#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_SignalQuality::sendCommand(Stream *uart)
{
	uart->println(F("AT+CSQ"));
}


void SIM5360E_ATCEH::ATCEH_SignalQuality::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	switch(token)
	{
		case RT_OK:
		control = DONE;
		break;
		
		case RT_CSQ:
		rssi_level = atoi((char *)lexer->responseBuffer + 6);
		break;
		
		case RT_CMERROR:
		case RT_ERROR:
		control = ABORT;
		break;
	}
}
