#include "../sim5360e.h"
#include "trace.h"



void SIM5360E_ATCEH::ATCEH_EnterPIN::initialize(uint32_t& timeLimit, int& maxTry)
{
	maxTry = 1;
}


void SIM5360E_ATCEH::ATCEH_EnterPIN::sendCommand(Stream *uart)
{
	//PIN為空的時候就不輸出AT+CPIN
	if (pin[0])
	{
		uart->print(F("AT+CPIN="));
		uart->write(pin, 4);
		uart->println();
	}
	else
	{
		debugSerial.println(F("Empty PIN!!!"));
	}
}


void SIM5360E_ATCEH::ATCEH_EnterPIN::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	switch(token)
	{
		case RT_OK:
		control = DONE;
		break;
		
		case RT_CMERROR:
		case RT_ERROR:
		memset(pin, 0, sizeof(pin));		//清掉PIN. Enter PIN會出現錯誤通常是PIN碼不正確，不清掉的話要是又再次execATC(EnterPIN)恐會鎖卡.
		if(onPINError) onPINError();
		control = ABORT;
		break;
		
		default:
		control = KEEPON;
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_QueryPIN::initialize(uint32_t& timeLimit, int& maxTry)
{
	needPIN = false;
	isResponsed = false;
}


void SIM5360E_ATCEH::ATCEH_QueryPIN::sendCommand(Stream *uart)
{
	uart->println(F("AT+CPIN?"));
}


void SIM5360E_ATCEH::ATCEH_QueryPIN::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	char res[32];
	
	switch(token)
	{
		case RT_CPIN:
		
		lexer->parseString(res, sizeof(res));
		
		if (!strcmp(res, "SIM PIN"))
		{
			isResponsed = true;
			needPIN = true;
		}
		else if(!strcmp(res, "READY"))
		{
			isResponsed = true;
			needPIN = false;
		}
		control = KEEPON;
		break;
		
		case RT_OK:
		control = isResponsed? DONE : RESEND;
		break;
		
		default:
		if (retryTimer.timeout(1000))
		{
			control = RESEND;
		}
		else
		{
			control = KEEPON;
		}
	}
}
