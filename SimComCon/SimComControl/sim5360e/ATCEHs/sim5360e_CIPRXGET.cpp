#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_ReadTCPStream::initialize(uint32_t& timeLimit, int& maxTry)
{
	maxTry = 1;
	front = sizeof(readBuffer);
	rest_len = 0;
}


void SIM5360E_ATCEH::ATCEH_ReadTCPStream::sendCommand(Stream *uart)
{
	uart->print(F("AT+CIPRXGET=2,0,"));
	uart->println(sizeof(readBuffer));
}


void SIM5360E_ATCEH::ATCEH_ReadTCPStream::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_CIPRXGET:	//ex) +CIPRXGET: 2,0,48,10
		
		//<mode>
		if (!(lexer->parseInt32(&tmp) && tmp == 2)) break;
		
		//<cid>
		lexer->parseInt32(&tmp);
		
		//<read_len>
		if (!(lexer->parseInt32(&tmp) && tmp > 0)) break;
		lexer->startFLRmode(tmp);
		
		//<rest_len>
		if (lexer->parseInt32(&tmp)) rest_len = tmp;
		
		control = KEEPON;
		break;
		
		case RT_FLR_RESPONSE:
		{
			size_t len;
			
			//雖然recv_count應該不會超過sizeof(readBuffer)，但還是做檢查以避免buffer overflow.
			if (lexer->recv_count <= sizeof(readBuffer))
			{
				front = sizeof(readBuffer) - lexer->recv_count;
				len = lexer->recv_count;
			}
			else
			{
				front = 0;
				len = sizeof(readBuffer);
			}
			
			memcpy(readBuffer + front, lexer->responseBuffer, len);
		}
		control = KEEPON;
		break;
		
		case RT_ERROR:
		control = ABORT;
		break;
		
		case RT_OK:
		control = DONE;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_CheckDataIncoming::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPRXGET=4,0"));
}


void SIM5360E_ATCEH::ATCEH_CheckDataIncoming::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_OK:
		control = DONE;
		break;
		
		case RT_CIPRXGET:
		if (!(lexer->parseInt32(&tmp) && tmp == 4)) break;
		
		lexer->parseInt32(&tmp);
		if (lexer->parseInt32(&tmp) && tmp > 0)
		{
			modem->IncomingTransmission = true;
		}
		control = KEEPON;
		break;
	}
}
