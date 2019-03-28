/*
 * sim5360e.cpp
 *
 * Created: 2017/6/29 下午 04:06:57
 *  Author: user
 */ 


#include "sim5360e.h"


GATL::PatternTable<SIM5360E_ATCEH::LAST_RESPONSE_TOKEN> SIM5360E_ATCEH::patternTable;

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

void SIM5360E_ATCEH::printToken(GATL::Token token, GATL *lexer)
{
	if (token == GATL::RT_NONE)
	{
		return ;
	}
	
	switch(token)
	{
		case GATL::RT_OTHER:
		printf("{?}%s", lexer->responseBuffer);
		break;
		
		case GATL::RT_PROMPT:
		printf("{>}");
		break;
		
		case RT_OK:
		printf("{OK}");
		break;
		
		case RT_ERROR:
		printf("{ERROR}");
		break;
		
		case RT_CPIN:
		printf("{+CPIN}%s", lexer->responseBuffer);
		break;
		
		case RT_CMERROR:
		printf("{+CME ERROR}%s", lexer->responseBuffer);
		break;
		
		case RT_ICCID:
		printf("{+ICCID}%s", lexer->responseBuffer);
		break;
		
		case RT_CGREG:
		printf("{+CGREG}%s", lexer->responseBuffer);
		break;
		
		case RT_NETOPEN:
		printf("{+NETOPEN}%s", lexer->responseBuffer);
		break;
		
		case RT_CIPOPEN:
		printf("{+CIPOPEN}%s", lexer->responseBuffer);
		break;
		
		case RT_CIPSEND:
		printf("{+CIPSEND}%s", lexer->responseBuffer);
		break;
		
		case RT_CIPRXGET:
		printf("{+CIPRXGET}%s", lexer->responseBuffer);
		break;
		
		case RT_CCLK:
		printf("{+CCLK}%s", lexer->responseBuffer);
		break;
		
		case RT_START:
		printf("{START}");
		break;
		
		case RT_CGPSINFO:
		printf("{+CGPSINFO}%s", lexer->responseBuffer);
		break;
		
		case RT_CIPCLOSE:
		printf("{+CIPCLOSE}%s", lexer->responseBuffer);
		break;
		
		case RT_CREG:
		printf("{+CREG}%s", lexer->responseBuffer);
		break;
		
		case RT_PDPDEACT:
		printf("{PDP DEACT}");
		break;
		
		case RT_CGATT:
		printf("{+CGATT}%s", lexer->responseBuffer);
		break;
		
		case RT_CSQ:
		printf("{+CSQ}%s", lexer->responseBuffer);
		break;
		
		case RT_IPCLOSE:
		printf("{+IPCLOSE}%s", lexer->responseBuffer);
		break;
		
		default:
		printf("<Unknown Token> %d", token);
		break;
	}
	
	printf("\r\n");
}

#endif



void SIM5360E::begin(uint32_t baudrate, SERCOM *scm, uint16_t pinRX, uint16_t pinTX, SercomRXPad padRX, SercomUartTXPad padTX)
{
}


void SIM5360E::feed(char c)
{
	GATL::Token token = lexer.scan(c);
	
	printToken(token, &lexer);
	if(token != GATL::RT_NONE) printf("%s\r\n", lexer.responseBuffer);
}


void SIM5360E::IRQHandler()
{
	if (sercom->availableDataUART())
	{
		feed(sercom->readDataUART());
	}
	
	if (sercom->isDataRegisterEmptyUART())
	{
		sercom->disableDataRegisterEmptyInterruptUART();
	}
	
	if (sercom->isUARTError())
	{
		sercom->acknowledgeUARTError();
		sercom->clearStatusUART();
	}
}
