/*
 * sim5360e_patterns.h
 *
 * Created: 2017/6/29 下午 04:07:28
 *  Author: user
 */ 


#ifndef SIM5360E_ATCEH_H_
#define SIM5360E_ATCEH_H_


#include "IPAddress.h"
#include "generic_AT_commander.h"


class SIM5360E_ATCEH : public GenericATLexer, public GenericATCmder
{
public:
	//Response Token 都會直接對應到patternTable[]中的某個元素，故值不能重複且 >= 0
	enum ResponseToken
	{
		RT_OK,
		RT_ERROR,
		RT_CPIN,
		RT_CMERROR,
		RT_ICCID,
		RT_CGREG,
		RT_NETOPEN,
		RT_CIPOPEN,
		RT_CIPSEND,
		RT_CIPRXGET,
		RT_CCLK,
		RT_START,
		RT_CGPSINFO,
		RT_CIPCLOSE,
		RT_CREG,
		RT_PDPDEACT,
		RT_CGATT,
		RT_CSQ,
		RT_IPCLOSE,
		//所有Response Token都必須定義在LAST_RESPONSE_TOKEN之前
		LAST_RESPONSE_TOKEN
	};
	
	//////////////////////////////////////////////////////////////////////////
	// AT Command Exchange Handlers
	//////////////////////////////////////////////////////////////////////////
	
	struct ATCExchange_SimpleWriteHandler : public ATCExchangeHandler
	{
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	};
	
	
	struct ATCEH_WaitModemReady : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);

	} WaitModemReady;


	struct ATCEH_NoEcho : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} NoEcho;


	struct ATCEH_NoIPHeader : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} NoIPHeader;


	struct ATCEH_HideRemoteHost : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} HideRemoteHost;


	struct ATCEH_SetManualReceive : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} SetManualReceive;


	struct ATCEH_DefinePdpContext : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} DefinePdpContext;


	struct ATCEH_SetActiveProfile : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} SetActiveProfile;


	struct ATCEH_NonTransparentMode : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} NonTransparentMode;


	struct ATCEH_WaitTcpAckMode : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} WaitTcpAckMode;


	struct ATCEH_SetTimeoutParameter : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} SetTimeoutParameter;
	
	
	struct ATCEH_EnableGPS : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
		
	} EnableGPS;
	
	
	struct ATCEH_TestAlive : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
		
	} TestAlive;
	
	
	/*
	
	struct ATCEH_ : public ATCExchange_SimpleWriteHandler
	{
		virtual void sendCommand(Stream *uart);
	
	} ;

	*/
	
	//////////////////////////////////////////////////////////////////////////
	
	struct ATCEH_QueryPIN : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		bool isResponsed;
		bool needPIN;

	} QueryPIN;
	
	
	struct ATCEH_EnterPIN : public ATCExchangeHandler
	{
		ATCEH_EnterPIN()
		{
			pin[0] = '\0';
			onPINError = nullptr;
		}
		
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		char pin[4];	//[[警告]] pin不是null-terminated的字串!
		void (*onPINError)(void);
		
	} EnterPIN;
	
	
	struct ATCEH_WaitGPRSReady : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		int stat;
	
	} WaitGPRSReady;
	
	
	struct ATCEH_OpenNetwork : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		int errno;
		bool recvOK;
	
	} OpenNetwork;
	
	
	struct ATCEH_CheckNetwork : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		bool received_answer;
		bool net_opened;
	
	} CheckNetwork;
	
	
	struct ATCEH_SocketConnect : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		enum {TCP, UDP} protocol;
		const char *host;
		IPAddress ip_addr;
		uint16_t port;
		bool recvOK;
	
	} SocketConnect;
	
	
	struct ATCEH_SocketClose : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		int errno;
	
	} SocketClose;
	
	
	struct ATCEH_IPSendFixedLength : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
		const uint8_t *source;
		size_t length;
		Stream *last_uart;
	
	} IPSendFixedLength;
	
	
	struct ATCEH_BeginIPSendVariableLength : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
	} BeginIPSendVariableLength;
	
	
	struct ATCEH_FinishIPSendVariableLength : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
	
	} FinishIPSendVariableLength;
	
	
	struct ATCEH_CheckDataIncoming : public ATCExchangeHandler
	{
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		SIM5360E_ATCEH *modem;
		
	} CheckDataIncoming;
	
	
	struct ATCEH_ReadTCPStream : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		ATCEH_ReadTCPStream()
		{
			front = sizeof(readBuffer);
			rest_len = 0;
		}
		
		size_t remain()
		{
			return sizeof(readBuffer) - front;
		}
		
		uint8_t readBuffer[GATC_RECV_BUFFER_MAX];	//由於GATL的FLR一次只能收最多GATC_RECV_BUFFER_MAX的資料，所以乾脆令兩者一樣長
		size_t front;
		size_t rest_len;
		
	} ReadTCPStream;


	struct ATCEH_GlobalHandler : public ATCExchangeHandler
	{
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		SIM5360E_ATCEH *modem;
		
	} GlobalHandler;
	
	
	struct ATCEH_SocketState : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		int8_t s0_active;
		
	} SocketState;
	
	
	struct ATCEH_ReadIMEI : public ATCExchangeHandler
	{
		ATCEH_ReadIMEI()
		{
			cachedIMEI[0] = '\0';
		}
		
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		char cachedIMEI[15 + 1];
		
	} ReadIMEI;
	
	
	struct ATCEH_ReadICCID : public ATCExchangeHandler
	{
		ATCEH_ReadICCID()
		{
			cachedICCID[0] = '\0';
		}
		
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		char cachedICCID[20 + 1];
		
	} ReadICCID;
	
	
	struct ATCEH_ReadGPSInfo : public ATCExchangeHandler
	{
		ATCEH_ReadGPSInfo()
		{
			location_string[0] = '\0';
		}
		
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		char location_string[27];
		
	} ReadGPSInfo;
	
	
	struct ATCEH_SignalQuality : public ATCExchangeHandler
	{
		ATCEH_SignalQuality() : rssi_level(-1) {}
		
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
		int rssi_level;
		
	} SignalQuality;
	
	
	/*
	
	struct ATCEH_ : public ATCExchangeHandler
	{
		virtual void initialize(uint32_t& timeLimit, int& maxTry);
		virtual void sendCommand(Stream *uart);
		virtual void parseResponse(GenericATLexer::GATLToken token, GenericATLexer *lexer, GenericATCmder::GATC_CTRL& control, SimpleTimer& retryTimer);
		
	} ;
	
	*/
	
	
	//////////////////////////////////////////////////////////////////////////
	
	bool IncomingTransmission;
	bool PDP_deactive;
	bool PassiveSocketClose;
	
	SIM5360E_ATCEH(Stream *uart) :
		GenericATLexer(patternTable.table, patternTable.TABLE_SIZE, set_response_patterns),
		GenericATCmder(this, uart, &GlobalHandler)
	{
		IncomingTransmission = false;
		PDP_deactive = false;
		PassiveSocketClose = false;
		
		CheckDataIncoming.modem = this;
		GlobalHandler.modem = this;
		
#ifdef SCC_TOKEN_MONITOR
		traceToken = printToken;
#endif
	}
	
private:
	static GenericATLexer::PatternTable<LAST_RESPONSE_TOKEN> patternTable;
	static void set_response_patterns();
#ifdef SCC_TOKEN_MONITOR
	static void printToken(GenericATLexer::GATLToken token, GenericATLexer*);
#endif
};



#endif /* SIM5360E_PATTERNS_H_ */