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
	
	location_string[0] = '\0';
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
		
		//組成格式 [N/S][Latitude],[E/W][Longitude]
		//例如"N24.141847,E120.639634"
		{
			char *p = location_string;
			*p++ = *ns;
			
			for (int i=0; i<11 && *la; i++)
			{
				*p++ = *la++;
			}
			
			*p++ = ',';
			*p++ = *ew;
			
			for (int i=0; i<12 && *lo; i++)
			{
				//(有待驗證)照手冊的描述，經度小於100時「貌似」會產生空白字元，過濾掉.
				if (*lo != ' ') *p++ = *lo;
				lo++;
			}
			
			*p = '\0';
		}
		
		control = GenericATCmder::KEEPON;
		break;
		
		case RT_OK:
		if (location_string[0])
		{
			control = GenericATCmder::DONE;
		}
		break;
	}
}
