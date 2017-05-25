#include "Arduino.h"
#include "avr/pgmspace.h"


#if 0
class asdf
{
	public:
	struct foo
	{
		int a;
		const char *b;
		float j;
	};
	
	static foo bar[];
};


asdf::foo asdf::bar[] =
{
	{.a = 2, .b = "Hello"},
	{.a = 4, .b = "world"},
	{.a = 94, .b = "87"},
	{.a = 87, .b = "94"}
};


asdf x;

void setup()
{

x.bar[0].a++;	
}

void loop()
{
}
#endif


#if 1


#include "SimComCon/SimComCon.h"

#include "SimComCon/SimComCon_private.h"


#define GATC_RECV_BUFFER_MAX 256


class GenericATLexer
{
public:
	typedef unsigned int GATLToken;
	
	struct ATResponsePattern
	{
		const char *text;
		bool prefixMatch;
		GATLToken token;
	};
	
	GenericATLexer(ATResponsePattern *table = nullptr, size_t tSize = 0)
		: patternTableSize(tSize), patternTable(table)
	{
		resetLine = true;
		recv_count = 0;
		prevChar = 0;
		matchBegin = matchEnd = 0;
		modeFLR = false;
		
		initPatternTable();
	}
	
	int feed(int);
	
	void init()
	{
		resetLine = true;
	}
	
	uint8_t responseBuffer[GATC_RECV_BUFFER_MAX];
	uint16_t recv_count;
	
	enum
	{
		AT_NONE,
		AT_OTHER,
		AT_OK,
		AT_ERROR,
		AT_FLR_RESPONSE,
		//若要追加GATL的預定義token，請務必定義在 GATL_PREDEFINE_TOKEN_MAX 之前.
		GATL_PREDEFINE_TOKEN_MAX
	};
	
	ATResponsePattern * const patternTable;
	
	#ifdef DEBUG
	void printConfigure()
	{
		Serial.println(F("*********************"));
		
		Serial.print("PTS = ");
		Serial.println(patternTableSize);
		
		for (size_t i = 0; i < patternTableSize; i++)
		{
			if (patternTable[i].prefixMatch)
			{
				Serial.write('^');
			}
			
			Serial.println((__FlashStringHelper *) patternTable[i].text);
		}
		
		Serial.println(F("*********************"));
	}
	
	#endif

private:
	unsigned char prevChar;
	bool resetLine;
	size_t matchBegin, matchEnd;
	const size_t patternTableSize;
	bool matched_prefix;
	bool modeFLR;
	size_t fixed_recv_length;
	
	void initPatternTable();
	//void startFLRmode(size_t length);
};


//注意：feed()在一般模式下「不會」把CR與LF扔進responseBuffer[]中
//欲接收CR與LF，請切換至FLR mode
int GenericATLexer::feed(int _ch)
{
	if (resetLine)
	{
		resetLine = false;
		recv_count = 0;
		matchBegin = 0;
		matchEnd = patternTableSize;
		matched_prefix = false;
		prevChar = 0;
	}
	
	if (_ch < 0)
	{
		return AT_NONE;
	}
	
	unsigned char ch = _ch;
	
	//special process for CR/LF
	if (ch == '\n' || ch == '\r')
	{
		//meet CR+LF, the end of line
		if(prevChar == '\r' && ch == '\n')
		{
			resetLine = true;
			
			//only one candidate left, and hit the end
			//or already "matched prefix"
			if (matched_prefix || matchBegin + 1 == matchEnd && '\0' == pgm_read_byte(&patternTable[matchBegin].text[recv_count]))
			{
				return patternTable[matchBegin].token;
			}
			else if (recv_count > 0)
			{
				return AT_OTHER;
			}
			else
			{
				return AT_NONE;
			}
		}
	}
	else
	{
		// change the range of candidate patternTable
		
		#ifdef TEST_TRACE
		printf("[A1] b=%d e=%d\n", matchBegin, matchEnd);
		#endif
		
		//if prefix-matched, pass this step
		if (!matched_prefix &&
		//if candidate range is empty, pass this step
			!(matchBegin >= matchEnd))
		{
			size_t k = matchBegin;
			
			for (; k < matchEnd; k++)
			{
				if (ch == pgm_read_byte(&patternTable[k].text[recv_count]))
				{
					break;
				}
			}
			matchBegin = k;
			
			for (; k < matchEnd; k++)
			{
				if (ch != pgm_read_byte(&patternTable[k].text[recv_count]))
				{
					break;
				}
			}
			matchEnd = k;
			
			//there's only one candidate left
			if (matchBegin + 1 == matchEnd)
			{
				//if the candidate has 'prefixMatch'
				if(patternTable[matchBegin].prefixMatch &&
				//and the candidate is fully matched
					'\0' == pgm_read_byte(&patternTable[matchBegin].text[recv_count + 1]))
				{
					matched_prefix = true;
				}
			}
		}
		
		// put data into buffer
		
		if (recv_count >= GATC_RECV_BUFFER_MAX)
		{
			/* 超出buffer大小的資料就不丟進去 */
			//Serial.println("+++++ buffer overflow");
			printf("+++++ buffer overflow");
		}
		else
		{
			responseBuffer[recv_count++] = ch;
		}
	}
	
	prevChar = ch;
	
	return AT_NONE;
}


int strcmp_2P(const char *s1, const char *s2)
{
	char c1 = 0, c2 = 0;
	
	do
	{
		c1 = pgm_read_byte(s1++);
		c2 = pgm_read_byte(s2++);
		
	} while (c1 != 0 && c1 == c2);
	
	return c1 - c2;
}


void GenericATLexer::initPatternTable()
{
	//set attribute
	for (size_t i = 0; i < patternTableSize; i++)
	{
		if (patternTable[i].text[0] == '^')
		{
			patternTable[i].prefixMatch = true;
			patternTable[i].text++;
		}
		else
			patternTable[i].prefixMatch = false;
	}
	
	//selection sort
	for (size_t i = 0; i < patternTableSize - 1; i++)
	{
		size_t min = i;
		
		for (size_t j = i + 1; j < patternTableSize; j++)
		{
			if (strcmp_2P(patternTable[j].text, patternTable[min].text) < 0)
			{
				min = j;
			}
		}
		
		if (min != i)
		{
			ATResponsePattern tmp = patternTable[min];
			patternTable[min] = patternTable[i];
			patternTable[i] = tmp;
		}
	}
}



class SimTest : public GenericATLexer
{
public:
	SimTest(size_t sz) : GenericATLexer(table_of_SimTest, sz){
		
	}

	static ATResponsePattern table_of_SimTest[];
	
	void printToken(GATLToken token);
	
	enum
	{
		AT_CPIN = GATL_PREDEFINE_TOKEN_MAX,
		AT_CALL_READY
	};
};


//PROGMEM const char atrp[] = "";
PROGMEM const char atrpT001[] = "OK";
PROGMEM const char atrpT002[] = "ERROR";
PROGMEM const char atrpT003[] = "^+CPIN:";
PROGMEM const char atrpT004[] = "Call Ready";

SimTest::ATResponsePattern SimTest::table_of_SimTest[] =
{
	{.text = atrpT001, .token = SimTest::AT_OK},
	{.text = atrpT002, .token = SimTest::AT_ERROR},
	{.text = atrpT003, .token = SimTest::AT_CPIN},
	{.text = atrpT004, .token = SimTest::AT_CALL_READY}
};


void SimTest::printToken(GATLToken token)
{
	switch(token)
	{
		case SimTest::AT_OK:
		Serial1.println("{OK}");
		break;
		
		case SimTest::AT_ERROR:
		Serial1.println("{ERROR}");
		break;
		
		case SimTest::AT_CALL_READY:
		Serial1.println("{Call Ready}");
		break;
		
		case SimTest::AT_CPIN:
		Serial1.println("{+CPIN:}");
		break;
		
		case SimTest::AT_OTHER:
		Serial1.print("rsps(");
		Serial1.print(recv_count);
		Serial1.print("): ");
		Serial1.write(responseBuffer, recv_count);
		Serial1.println();
		break;
	}
}


//////////////////////////////////////////////////////////////////////////

SimTest lexer(sizeof(SimTest::table_of_SimTest) / sizeof(SimTest::table_of_SimTest[0]));


void setup()
{
	Serial.begin(19200);
	Serial.println(F("General SimCom Controller   build: " __DATE__ ", " __TIME__));
	
	Serial1.begin(19200);
	Serial1.println(F("GATL Result Terminal   build: " __DATE__ ", " __TIME__));
	
	Serial2.begin(19200);
	
	#ifdef DEBUG
	lexer.printConfigure();
	#endif
}


void loop()
{
	//read from PC, and send to SIMCOM module
	if (Serial.available())
	{
		int ch = Serial.read();
		MODEM_UART_WRITE((const byte *)&ch, 1);
	}
	
	//read from SIMCOM module, and send to PC
	if (MODEM_UART_AVAILABLE())
	{
		int ch = MODEM_UART_READ();
		
		Serial.write(ch);	//print to PC
		
		lexer.printToken(lexer.feed(ch));
	}
}

#endif