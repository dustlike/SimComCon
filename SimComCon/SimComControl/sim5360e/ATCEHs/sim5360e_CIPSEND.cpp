#include "../sim5360e.h"
#include "../util/monitored_serial.h"



void SIM5360E_ATCEH::ATCEH_IPSendFixedLength::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 87000;
	maxTry = 1;
}


void SIM5360E_ATCEH::ATCEH_IPSendFixedLength::sendCommand(Stream *uart)
{
	last_uart = uart;
	uart->print(F("AT+CIPSEND=0,"));
	uart->println(length);
}


void SIM5360E_ATCEH::ATCEH_IPSendFixedLength::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t reqSendLength, cnfSendLength;
	
	switch(token)
	{
		case RT_PROMPT:
#ifdef ECHO_MODEM_INPUT
		for (size_t i = 0; i < length; i++)
		{
			if(source[i] < 16) Serial1.write('0');
			monitorSerial.print(source[i], HEX);
			monitorSerial.write(' ');
		}
		monitorSerial.println();
		
		reinterpret_cast<MonitoredSerial *>(last_uart)->direct_write_bytes(source, length);
#else
		last_uart->write(source, length);
#endif
		break;
		
		case RT_OK:
		control = DONE;
		break;
		
		default:
		if (source && retryTimer.timeout(1000))
		{
			control = ABORT;
		}
		else
		{
			control = KEEPON;
		}
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_BeginIPSendVariableLength::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 1000;
	maxTry = 1;
}


void SIM5360E_ATCEH::ATCEH_BeginIPSendVariableLength::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPSEND=0,"));
}


void SIM5360E_ATCEH::ATCEH_BeginIPSendVariableLength::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	if (token == RT_PROMPT)
	{
		control = DONE;
	}
	else
	{
		control = KEEPON;
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_FinishIPSendVariableLength::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 70000;
	maxTry = 1;
}


void SIM5360E_ATCEH::ATCEH_FinishIPSendVariableLength::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t reqSendLength, cnfSendLength;
	
	switch(token)
	{
		case RT_CIPSEND:
		
		lexer->parseInt32(&reqSendLength);	//借reqSendLength來讀<cid>
		
		if (lexer->parseInt32(&reqSendLength) && lexer->parseInt32(&cnfSendLength)
			&& reqSendLength == cnfSendLength)
		{
			control = DONE;
		}
		else
		{
			control = ABORT;
		}
		break;
		
		case RT_ERROR:
		control = ABORT;
		break;
		
		default:
		control = KEEPON;
		break;
	}
}
