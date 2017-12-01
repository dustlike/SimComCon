#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCExchange_SimpleWriteHandler::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	if (token == RT_OK)
	{
		control = GenericATCmder::DONE;
	}
}


void SIM5360E_ATCEH::ATCEH_NoEcho::sendCommand(Stream *uart)
{
	uart->println(F("ATE0"));
}


void SIM5360E_ATCEH::ATCEH_NoIPHeader::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPHEAD=0"));
}


void SIM5360E_ATCEH::ATCEH_HideRemoteHost::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPSRIP=0"));
}


void SIM5360E_ATCEH::ATCEH_SetManualReceive::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPRXGET=1"));
}


void SIM5360E_ATCEH::ATCEH_DefinePdpContext::sendCommand(Stream *uart)
{
	uart->println(F("AT+CGSOCKCONT=1,\"IP\",\"INTERNET\""));
}


void SIM5360E_ATCEH::ATCEH_SetActiveProfile::sendCommand(Stream *uart)
{
	uart->println(F("AT+CSOCKSETPN=1"));
}


void SIM5360E_ATCEH::ATCEH_NonTransparentMode::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPMODE=0"));
}


void SIM5360E_ATCEH::ATCEH_WaitTcpAckMode::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPSENDMODE=1"));
}


void SIM5360E_ATCEH::ATCEH_SetTimeoutParameter::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPTIMEOUT=45000,60000,40000"));
}


void SIM5360E_ATCEH::ATCEH_EnableGPS::sendCommand(Stream *uart)
{
	uart->println(F("AT+CGPS=1"));
}
