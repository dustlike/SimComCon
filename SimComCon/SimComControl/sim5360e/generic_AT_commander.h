/*
 * generic_AT_commander.h
 *
 * Created: 2017/5/8 上午 10:57:13
 *  Author: user
 */ 


#ifndef GENERIC_AT_COMMANDER_H_
#define GENERIC_AT_COMMANDER_H_


#include "generic_AT_lexer.h"
#include "util/simple_timer.h"
#include "util/monitored_serial.h"


struct ATCExchangeHandler;

class GenericATCmder
{
public:
	
	enum GATC_CTRL
	{
		DONE,
		ABORT,
		KEEPON,
		RESEND,
		UNTRAPPED
	};
	
	enum
	{
		EXEC_OK = 0,
		EXEC_ERROR = -1,
		EXEC_TRYOUT = -2
	};
	
	
	GenericATCmder(GenericATLexer *gatl, Stream *uart, ATCExchangeHandler *globalHandler = nullptr)
#ifdef ECHO_MODEM_INPUT
		: monitored_serial(uart)
#endif
	{
		lexer = gatl;
#ifdef ECHO_MODEM_INPUT
		this->uart = &monitored_serial;
#else
		this->uart = uart;
#endif
		globalATCEHandler = globalHandler;
		
#ifdef SCC_TOKEN_MONITOR
		traceToken = nullptr;
#endif
	}
	
	int execATCommand(ATCExchangeHandler& handler = empty_handler);
	
	void clearUart()
	{
		while(uart->read() >= 0);
	}
	
protected:
	Stream *uart;
#ifdef SCC_TOKEN_MONITOR
	void (*traceToken)(GenericATLexer::GATLToken, GenericATLexer*);
#endif
#ifdef ECHO_MODEM_INPUT
	MonitoredSerial monitored_serial;
#endif
	
private:
	GenericATLexer *lexer;
	static ATCExchangeHandler empty_handler;
	ATCExchangeHandler *globalATCEHandler;
};



struct ATCExchangeHandler
{
	virtual void initialize(uint32_t& timeLimit, int& maxTry);
	virtual void sendCommand(Stream *uart);
	virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
};


#endif /* GENERIC_AT_COMMANDER_H_ */
