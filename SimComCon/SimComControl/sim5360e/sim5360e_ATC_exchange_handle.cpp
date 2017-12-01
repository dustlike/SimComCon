/*
 * sim5360e_ATC_exchange_handle.cpp
 *
 * Created: 2017/11/28 上午 09:27:04
 *  Author: user
 */ 


#include "sim5360e_ATC_exchange_handle.h"
#include "trace.h"



GenericATLexer::PatternTable<SIM5360E_ATCEH::LAST_RESPONSE_TOKEN> SIM5360E_ATCEH::patternTable;


void SIM5360E_ATCEH::set_response_patterns()
{
	//[複製貼上用]
	//patternTable.register_as_prefix(PSTR(""), );
	//patternTable.register_as_complete(PSTR(""), );
	
	patternTable.register_as_complete(PSTR("OK"), RT_OK);
	patternTable.register_as_complete(PSTR("ERROR"), RT_ERROR);
	patternTable.register_as_prefix(PSTR("+CPIN:"), RT_CPIN);
	patternTable.register_as_prefix(PSTR("+CME ERROR:"), RT_CMERROR);
	patternTable.register_as_prefix(PSTR("+ICCID:"), RT_ICCID);
	patternTable.register_as_prefix(PSTR("+CGREG:"), RT_CGREG);
	patternTable.register_as_prefix(PSTR("+NETOPEN:"), RT_NETOPEN);
	patternTable.register_as_prefix(PSTR("+CIPOPEN:"), RT_CIPOPEN);
	patternTable.register_as_prefix(PSTR("+CIPSEND:"), RT_CIPSEND);
	patternTable.register_as_prefix(PSTR("+CIPRXGET:"), RT_CIPRXGET);
	patternTable.register_as_prefix(PSTR("+CCLK:"), RT_CCLK);
	patternTable.register_as_complete(PSTR("START"), RT_START);
	patternTable.register_as_prefix(PSTR("+CGPSINFO:"), RT_CGPSINFO);
	patternTable.register_as_prefix(PSTR("+CIPCLOSE:"), RT_CIPCLOSE);
	patternTable.register_as_prefix(PSTR("+CREG:"), RT_CREG);
	patternTable.register_as_complete(PSTR("+CIPEVENT: NETWORK CLOSED UNEXPECTEDLY"), RT_PDPDEACT);
	patternTable.register_as_prefix(PSTR("+CGATT:"), RT_CGATT);
	patternTable.register_as_prefix(PSTR("+CSQ:"), RT_CSQ);
	patternTable.register_as_prefix(PSTR("+IPCLOSE:"), RT_IPCLOSE);
	
	
	//新增完記得去printToken()增加新的token辨識ㄛ！ (?
}



#ifdef SCC_TOKEN_MONITOR

void SIM5360E_ATCEH::printToken(GenericATLexer::GATLToken token, GenericATLexer *lexer)
{
	if (token == RT_NONE)
	{
		return ;
	}
	
	switch(token)
	{
		case RT_OTHER:
		monitorSerial.print(F("{?}"));
		monitorSerial.print((const char *)lexer->responseBuffer);
		break;
		
		case RT_FLR_RESPONSE:
		monitorSerial.print(F("{FLR: "));
		monitorSerial.print(lexer->recv_count);
		monitorSerial.write('}');
		
		for (size_t i = 0; i < lexer->recv_count; i++)
		{
			monitorSerial.write(' ');
			uint8_t tmp = lexer->responseBuffer[i];
			if (tmp < 16) monitorSerial.write('0');
			monitorSerial.print(tmp, HEX);
		}
		break;
		
		case RT_PROMPT:
		monitorSerial.print(F("{>}"));
		break;
		
		case RT_OK:
		monitorSerial.print(F("{OK}"));
		break;
		
		case RT_ERROR:
		monitorSerial.print(F("{ERROR}"));
		break;
		
		case RT_CPIN:
		monitorSerial.print(F("{+CPIN}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 5);
		break;
		
		case RT_CMERROR:
		monitorSerial.print(F("{+CME ERROR}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 10);
		break;
		
		case RT_ICCID:
		monitorSerial.print(F("{+ICCID}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 6);
		break;
		
		case RT_CGREG:
		monitorSerial.print(F("{+CGREG}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 6);
		break;
		
		case RT_NETOPEN:
		monitorSerial.print(F("{+NETOPEN}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 8);
		break;
		
		case RT_CIPOPEN:
		monitorSerial.print(F("{+CIPOPEN}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 8);
		break;
		
		case RT_CIPSEND:
		monitorSerial.print(F("{+CIPSEND}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 8);
		break;
		
		case RT_CIPRXGET:
		monitorSerial.print(F("{+CIPRXGET}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 9);
		break;
		
		case RT_CCLK:
		monitorSerial.print(F("{+CCLK}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 5);
		break;
		
		case RT_START:
		monitorSerial.print(F("{START}"));
		break;
		
		case RT_CGPSINFO:
		monitorSerial.print(F("{+CGPSINFO}"));
		break;
		
		case RT_CIPCLOSE:
		monitorSerial.print(F("{+CIPCLOSE}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 9);
		break;
		
		case RT_CREG:
		monitorSerial.print(F("{+CREG}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 5);
		break;
		
		case RT_PDPDEACT:
		monitorSerial.print("{PDP DEACT}");
		break;
		
		case RT_CGATT:
		monitorSerial.print(F("{+CGATT}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 6);
		break;
		
		case RT_CSQ:
		monitorSerial.print(F("{+CSQ}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 4);
		break;
		
		case RT_IPCLOSE:
		monitorSerial.print(F("{+IPCLOSE}"));
		monitorSerial.print((const char *)lexer->responseBuffer + 8);
		break;
		
		default:
		monitorSerial.print(F("<Uncaptured Token> "));
		monitorSerial.print(token);
		break;
	}
	
	monitorSerial.println();
}

#endif
