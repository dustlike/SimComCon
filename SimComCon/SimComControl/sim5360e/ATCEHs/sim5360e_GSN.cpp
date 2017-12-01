/*
 * sim5360e_GSN.cpp
 *
 * Created: 2017/11/8 上午 10:10:21
 *  Author: user
 */ 

#include "../sim5360e.h"
#include <ctype.h>



void SIM5360E_ATCEH::ATCEH_ReadIMEI::sendCommand(Stream *uart)
{
	uart->println(F("AT+GSN"));
}



void SIM5360E_ATCEH::ATCEH_ReadIMEI::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	switch (token)
	{
		case RT_OTHER:
		//由於OTHER會比較雜，總之先以長度=15這項嚴苛條件來過濾.
		if (lexer->recv_count == 15)
		{
			int i = 0;
			
			for (; i < 15; i++)
			{
				cachedIMEI[i] = lexer->responseBuffer[i];
				if (!isdigit(cachedIMEI[i]))
				{
					//IMEI應由純數字組成.中止.
					break;
				}
			}
			
			//加上NUL字元。若因IMEI格式有誤而中斷，cached_IMEI長度將會不等於15，令程式能以此作為判斷依據.
			cachedIMEI[i] = '\0';
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
