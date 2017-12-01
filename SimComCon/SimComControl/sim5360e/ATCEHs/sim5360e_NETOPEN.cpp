#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_OpenNetwork::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 45000;
}


void SIM5360E_ATCEH::ATCEH_OpenNetwork::sendCommand(Stream *uart)
{
	uart->println(F("AT+NETOPEN"));
	errno = -1;
}


void SIM5360E_ATCEH::ATCEH_OpenNetwork::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_CMERROR:
		case RT_ERROR:
		control = ABORT;
		break;
		
		case RT_NETOPEN:
		control = (errno == -2)? DONE : KEEPON;
		
		errno = lexer->parseInt32(&tmp)? tmp : -1;
		break;
		
		case RT_OK:
		if (errno >= 0)	//already received +NETOPEN
		{
			control = DONE;
		}
		else
		{
			errno = -2;
			control = KEEPON;
		}
		break;
		
		default:
		//fase resend: 若超過5秒還沒收到OK或+NETOPEN，就重送.
		if (retryTimer.timeout(5000) && errno == -1)
		{
			control = RESEND;
		}
		else
		{
			control = KEEPON;
		}
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_CheckNetwork::initialize(uint32_t& timeLimit, int& maxTry)
{
	received_answer = net_opened = false;
}


void SIM5360E_ATCEH::ATCEH_CheckNetwork::sendCommand(Stream *uart)
{
	uart->println(F("AT+NETOPEN?"));
}


void SIM5360E_ATCEH::ATCEH_CheckNetwork::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_OK:
		if (received_answer)
		{
			control = GenericATCmder::DONE;
		}
		else
		{
			control = GenericATCmder::RESEND;
		}
		break;
		
		case RT_NETOPEN:
		if (lexer->parseInt32(&tmp))
		{
			received_answer = true;
			net_opened = (tmp == 1);
		}
		control = GenericATCmder::KEEPON;
		break;
	}
}
