/*
 * sim5360e_CICCID.cpp
 *
 * Created: 2017/11/7 下午 02:31:08
 *  Author: user
 */ 

#include "../sim5360e.h"
#include <ctype.h>



void SIM5360E_ATCEH::ATCEH_ReadICCID::sendCommand(Stream *uart)
{
	uart->println(F("AT+CICCID"));
}



void SIM5360E_ATCEH::ATCEH_ReadICCID::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	switch (token)
	{
		case RT_ICCID:
		if (lexer->recv_count >= 28)
		{
			int i = 0;
			
			for (; i < 20; i++)
			{
				cachedICCID[i] = lexer->responseBuffer[8 + i];
				if (!isdigit(cachedICCID[i]))
				{
					//ICCID應由純數字組成.中止.
					break;
				}
			}
			
			//加上NUL字元。若因ICCID格式有誤而中斷，cached_ICCID長度將會不等於20，令程式能以此作為判斷依據.
			cachedICCID[i] = '\0';
		}
		
		control = GenericATCmder::KEEPON;
		break;
		
		case RT_OK:
		control = GenericATCmder::DONE;
		break;
		
		case RT_ERROR:
		control = GenericATCmder::RESEND;
		break;
	}
}
