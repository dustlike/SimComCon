/*
 * generic_AT_lexer.h
 *
 * Created: 2017/5/26 下午 03:24:54
 *  Author: user
 */ 


#ifndef GENERIC_AT_LEXER_H_
#define GENERIC_AT_LEXER_H_


#include "Arduino.h"
#include "config.h"


class GenericATLexer
{
public:
	typedef int GATLToken;
	
	struct ATResponsePattern
	{
		const char *text;
		GATLToken token;
		bool prefixMatch;
	};
	
	template<size_t N> struct PatternTable
	{
		static constexpr size_t TABLE_SIZE = N;
		
		inline void register_as_complete(const char *text, GATLToken token)
		{
			if (token >= 0 && token < N)	//avoid out of array bound
			{
				table[token].text = text;
				table[token].token = token;
				table[token].prefixMatch = 0;
			}
		}
		
		inline void register_as_prefix(const char *text, GATLToken token)
		{
			if (token >= 0 && token < N)	//avoid out of array bound
			{
				table[token].text = text;
				table[token].token = token;
				table[token].prefixMatch = 1;
			}
		}
		
		ATResponsePattern table[N];
	};
	
	//////////////////////////////////////////////////////////////////////////
	
	GenericATLexer(ATResponsePattern *table, size_t szTable, void (*patternRegister)())
		: patternTable(table), patternTableSize(initPatternTable(table, szTable, patternRegister))
	{
		resetLine = true;
		recv_count = 0;
		prevChar = 0;
		matchBegin = matchEnd = 0;
		inFLRmode = false;
		parse_pos = 0;
	}
	
	
	//將UART收到的資料給GATL，並回傳解析出的token
	//注意：feed()在一般模式下「不會」把CR與LF扔進responseBuffer[]中
	//欲接收CR與LF，請切換至FLR mode
	GATLToken feed(int);
	
	//進入FLR模式。length不在[1, GATC_RECV_BUFFER_MAX]的話不會進入FLR
	void startFLRmode(size_t length);
	
	bool parseInt32(int32_t *, int base = 10);
	size_t parseString(char *, size_t);
	
	uint8_t responseBuffer[GATC_RECV_BUFFER_MAX + 1];	//留1byte塞NUL以方便字串操作.
	size_t recv_count;
	
	//GATL pre-defined response token. 值都要 < 0
	enum
	{
		RT_OTHER = -4,
		RT_FLR_RESPONSE = -3,
		RT_PROMPT = -2,
		RT_NONE = -1
	};
	
#ifdef SCC_TOKEN_MONITOR
	void listAllPatterns();
#endif
	
protected:
	ATResponsePattern * const patternTable;
	
private:
	unsigned char prevChar;
	bool resetLine;
	size_t matchBegin, matchEnd;
	const size_t patternTableSize;
	bool matched_prefix;
	bool inFLRmode;
	size_t fixed_recv_length;
	size_t parse_pos;

	static size_t initPatternTable(ATResponsePattern *, size_t, void (*)());
	size_t enclose_field();
};



#endif /* GENERIC_AT_LEXER_H_ */