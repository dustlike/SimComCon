#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_SocketConnect::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 60000;
	recvOK = false;
}


void SIM5360E_ATCEH::ATCEH_SocketConnect::sendCommand(Stream *uart)
{
	uart->print(F("AT+CIPOPEN=0,\"TCP\",\""));
	
	if (host)
	{
		//avoid injection
		for (const char *p = host; *p; p++)
		{
			if(*p != '"' || *p != ',') uart->write(*p);
		}
	}
	else
	{
		uart->print(ip_addr);
	}
	
	uart->print(F("\","));
	
	uart->println(port);
}


void SIM5360E_ATCEH::ATCEH_SocketConnect::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_CIPOPEN:
		lexer->parseInt32(&tmp);	//link_num
		if (lexer->parseInt32(&tmp) && tmp == 0)
		{
			control = DONE;
		}
		else
		{
			control = RESEND;
		}
		break;
		
		case RT_OK:
		recvOK = true;
		control = KEEPON;
		break;
		
		default:
		if (!recvOK && retryTimer.timeout(5000))
		{
			control = RESEND;
		}
		else
		{
			control = KEEPON;
		}
	}
}
