#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_GlobalHandler::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_CIPRXGET:
		//+CIPRXGET: 1
		if (lexer->responseBuffer[11] == '1')
		{
			modem->IncomingTransmission = true;
			control = GenericATCmder::KEEPON;
		}
		break;
		
		case RT_PDPDEACT:
		modem->PDP_deactive = true;
		control = GenericATCmder::KEEPON;
		break;
		
		case RT_IPCLOSE:
		modem->PassiveSocketClose = true;
		control = GenericATCmder::KEEPON;
		break;
	}
}
