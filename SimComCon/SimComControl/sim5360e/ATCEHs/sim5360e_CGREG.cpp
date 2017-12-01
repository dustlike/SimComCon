#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_WaitGPRSReady::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 60000;
	maxTry = 32767;
	stat = 0;
}

void SIM5360E_ATCEH::ATCEH_WaitGPRSReady::sendCommand(Stream *uart)
{
	uart->println(F("AT+CGREG?"));
}


void SIM5360E_ATCEH::ATCEH_WaitGPRSReady::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch (token)
	{
		case RT_CGREG:
		{
			lexer->parseInt32(&tmp);
			if (!lexer->parseInt32(&tmp))
			{
				stat = -1;
			}
			else
			{
				stat = tmp;
			}
		}
		
		control = GenericATCmder::KEEPON;
		break;
		
		case RT_OK:
		if (stat == 1	//registered, home network
		 || stat == 5)	//registered, roaming
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
	