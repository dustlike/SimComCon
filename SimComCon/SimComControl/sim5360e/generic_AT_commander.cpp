/*
 * generic_AT_commander.cpp
 *
 * Created: 2017/5/8 上午 10:57:19
 *  Author: user
 */ 


#include "generic_AT_commander.h"



int GenericATCmder::execATCommand(ATCExchangeHandler& handler)
{
	bool requestToSend = true;
	SimpleTimer retryTimer;
	int tryCount = 3;
	uint32_t startTime, timeLimit = 10000;
	int rc = EXEC_TRYOUT;
	
	handler.initialize(timeLimit, tryCount);
	
	startTime = millis();
	
	while (millis() - startTime < timeLimit)
	{
		if (requestToSend)
		{
			if (tryCount <= 0) break;
			tryCount--;
			
			handler.sendCommand(uart);
			
			retryTimer.reset();
			requestToSend = false;
		}
		
		int ch = uart->read();
		if (ch < 0) yield();	//do context switch
		
		GenericATLexer::GATLToken token = lexer->feed(ch);
		
#ifdef SCC_TOKEN_MONITOR
		if (traceToken) traceToken(token, lexer);
#endif
		
		//parse response
		
		GATC_CTRL control = UNTRAPPED;
		
		//先將token丟給global exchange handler
		if (globalATCEHandler && token != GenericATLexer::RT_NONE)
		{
			globalATCEHandler->parseResponse(token, lexer, control, retryTimer);
		}
		
		//若global handler不收，再丟給local handler 
		if (control == UNTRAPPED)
		{
			handler.parseResponse(token, lexer, control, retryTimer);
		}
		
		//判別流程控制.
		
		switch(control)
		{
			case KEEPON:
			//do nothing. just keep on.
			break;
			
			case DONE:
			rc = EXEC_OK;
			goto point_of_return;
			
			case ABORT:
			rc = EXEC_ERROR;
			goto point_of_return;
			
			case RESEND:
			requestToSend = true;
			
			//由於在許多場合中會連續發生RESEND，為了避免MCU用AT指令轟炸modem故加入一個冷卻時間
			delay(500);
			break;
			
			//UNTRAPPED or unknown control value(just treated as UNTRAPPED)
			default:
			if (retryTimer.timeout(3000))
			{
				//auto resend triggered
				requestToSend = true;
			}
			
		}
	}
	
point_of_return:
	return rc;
}



ATCExchangeHandler GenericATCmder::empty_handler;

//////////////////////////////////////////////////////////////////////////


void ATCExchangeHandler::initialize(uint32_t& timeLimit, int& maxTry)
{
	//do nothing by default
}

void ATCExchangeHandler::sendCommand(Stream *uart)
{
	//do nothing by default
}

void ATCExchangeHandler::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	//just finish
	control = GenericATCmder::DONE;
}
