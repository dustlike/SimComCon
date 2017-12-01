#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_SocketClose::initialize(uint32_t& timeLimit, int& maxTry)
{
	timeLimit = 20000;
}


void SIM5360E_ATCEH::ATCEH_SocketClose::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPCLOSE=0"));
	errno = -1;
}


void SIM5360E_ATCEH::ATCEH_SocketClose::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		//暫定無視所有error，只靠+CIPCLOSE判斷就好.
		case RT_CMERROR:
		case RT_ERROR:
		if (errno >= 0)	//already received +CIPCLOSE
		{
			control = DONE;
		}
		else
		{
			errno = -2;
			control = KEEPON;
		}
		break;
		
		case RT_OK:
		if (errno >= 0)	//already received +CIPCLOSE
		{
			control = DONE;
		}
		else
		{
			errno = -2;
			control = KEEPON;
		}
		break;
		
		case RT_CIPCLOSE:
		
		//為了分辨出ATCEH_SocketState的+CIPCLOSE
		//這邊給定條件：必須帶著恰好兩個參數才給過
		
		control = (errno == -2)? DONE : KEEPON;
		
		lexer->parseInt32(&tmp);	//<link_num>
		errno = lexer->parseInt32(&tmp)? tmp : -1;
		if (lexer->parseInt32(&tmp))	//拖著太多參數了!!!
		{
			errno = -1;
			control = KEEPON;
		}
		break;
		
		default:
		//若超過5秒還沒收到OK或+CIPCLOSE，就重送.
		if (retryTimer.timeout(5000) && errno == -1)
		{
			control = RESEND;
		}
		else
		{
			control = KEEPON;
		}
	}
}


//////////////////////////////////////////////////////////////////////////


void SIM5360E_ATCEH::ATCEH_SocketState::initialize(uint32_t& timeLimit, int& maxTry)
{
	s0_active = -1;
}


void SIM5360E_ATCEH::ATCEH_SocketState::sendCommand(Stream *uart)
{
	uart->println(F("AT+CIPCLOSE?"));
}


void SIM5360E_ATCEH::ATCEH_SocketState::parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GATC_CTRL& control, SimpleTimer& retryTimer)
{
	int32_t tmp;
	
	switch(token)
	{
		case RT_OK:
		
		if (s0_active == 1 || s0_active == 0)
		{
			control = DONE;
		}
		else
		{
			//尚未收到正確的+CIPCLOSE，直接跳掉
			s0_active = -1;
			control = ABORT;
		}
		break;
		
		case RT_ERROR:
		control = ABORT;
		break;
		
		case RT_CIPCLOSE:
		
		//為了分辨出ATCEH_SocketClose的+CIPCLOSE
		//這邊給定條件：起碼要拖著3個以上的參數才給過.
		
		s0_active = lexer->parseInt32(&tmp)? tmp : -1;
		
		if (!lexer->parseInt32(&tmp) || !lexer->parseInt32(&tmp))
		{
			s0_active = -1;
		}
		
		control = KEEPON;
		break;
	}
}
