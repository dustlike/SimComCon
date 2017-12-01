/*
 * sim5360e_CGPSINFO.cpp
 *
 * Created: 2017/11/10 上午 09:11:45
 *  Author: user
 */ 

#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_ReadGPSInfo::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 60000;
	maxTry = 32767;
	
	latitude[0] = '\0';
	longitude[0] = '\0';
}


void SIM5360E_ATCEH::ATCEH_ReadGPSInfo::sendCommand(Stream *uart)
{
	uart->println(F("AT+CGPSINFO"));
}


void SIM5360E_ATCEH::ATCEH_ReadGPSInfo::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	char *la, *ns, *lo, *ew;
	
	switch(token)
	{
		case RT_CGPSINFO:
		lexer->responseBuffer[lexer->recv_count] = 0;
		
		la = strtok((char *)lexer->responseBuffer + 10, ",");
		if (!la || *la == '\0') break;
		
		ns = strtok(nullptr, ",");
		if (!ns || *ns == '\0') break;
		
		lo = strtok(nullptr, ",");
		if (!lo || *lo == '\0') break;
		
		ew = strtok(nullptr, ",");
		if (!ew || *ew == '\0') break;
		
		latitude[0] = *ns;
		strncpy(latitude + 1, la, 11);
		
		longitude[0] = *ew;
		strncpy(longitude + 1, lo, 12);
		
		control = GenericATCmder::KEEPON;
		break;
		
		case RT_OK:
		if (latitude[0] && longitude[0])
		{
			control = GenericATCmder::DONE;
		}
		else
		{
			control = GenericATCmder::RESEND;
		}
		break;
		
		case RT_ERROR:
		control = GenericATCmder::RESEND;
		break;
	}
}
