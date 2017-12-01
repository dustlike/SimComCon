#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_WaitModemReady::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 30000;
}


void SIM5360E_ATCEH::ATCEH_WaitModemReady::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	switch(token)
	{
		case RT_START:
		control = GenericATCmder::DONE;
		break;
		
		default:
		control = GenericATCmder::KEEPON;
	}
}
