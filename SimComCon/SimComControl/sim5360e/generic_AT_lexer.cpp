/*
 * generic_AT_lexer.cpp
 *
 * Created: 2017/5/26 下午 03:26:28
 *  Author: user
 */ 


#include "generic_AT_lexer.h"
#include "trace.h"



GenericATLexer::GATLToken GenericATLexer::feed(int _ch)
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
	
	if (_ch < 0 || _ch > 255)
	{
		return RT_NONE;
	}
	
	unsigned char ch = _ch;
	
	if (inFLRmode)
	{
		responseBuffer[recv_count++] = ch;
		
		//收到足夠長的資料了，結束FLR回歸一般模式.
		if (recv_count >= fixed_recv_length)
		{
			inFLRmode = false;
			resetLine = true;
			return RT_FLR_RESPONSE;
		}
	}
	//special process for CR/LF
	else if (ch == '\n' || ch == '\r')
	{
		//meet CR+LF, the end of line
		if(prevChar == '\r' && ch == '\n')
		{
			resetLine = true;
			
			//add NUL character, make it easy to do string operation
			responseBuffer[recv_count] = '\0';
			
			//only one candidate left, and hit the end
			//or already "matched prefix"
			if (matched_prefix || matchBegin + 1 == matchEnd && '\0' == pgm_read_byte(&patternTable[matchBegin].text[recv_count]))
			{
				//將parse_pos定到pattern結尾
				parse_pos = strlen_P(patternTable[matchBegin].text);
				
				//將parse_pos定到第一個非空白(0x20)字元
				for (; parse_pos < recv_count; parse_pos++)
				{
					if (responseBuffer[parse_pos] != ' ' && responseBuffer[parse_pos] != '\t')
					{
						break;
					}
				}
				
				return patternTable[matchBegin].token;
			}
			else 
			{
				parse_pos = recv_count;
				return (recv_count > 0)? RT_OTHER : RT_NONE;
			}
		}
	}
	//設計給諸如AT+CIPSEND等命令，用來等待prompt符號('>')
	else if (recv_count == 0 && ch == '>')
	{
		resetLine = true;
		return RT_PROMPT;
	}
	else
	{
		// change the range of candidate patternTable
		
		//if prefix-matched, pass this step
		//if candidate range is empty, pass this step
		if (!matched_prefix && !(matchBegin >= matchEnd))
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
		
		// put data into buffer. will not put in if buffer overflow
		if (recv_count < GATC_RECV_BUFFER_MAX)
		{
			responseBuffer[recv_count++] = ch;
		}
	}
	
	prevChar = ch;
	
	return RT_NONE;
}


static int strcmp_pgm2pgm(const char *s1, const char *s2)
{
	char c1 = 0, c2 = 0;
	
	do
	{
		c1 = pgm_read_byte(s1++);
		c2 = pgm_read_byte(s2++);
		
	} while (c1 != 0 && c1 == c2);
	
	return c1 - c2;
}


size_t GenericATLexer::initPatternTable(ATResponsePattern *patternTable, size_t szTable, void (*patternRegister)())
{
	memset(patternTable, 0, szTable);
	
	patternRegister();
	
	//selection sort
	for (size_t i = 0; i < szTable - 1; i++)
	{
		size_t min = i;
		
		for (size_t j = i + 1; j < szTable; j++)
		{
			if (patternTable[j].text != NULL && (patternTable[min].text == NULL
			|| strcmp_pgm2pgm(patternTable[j].text, patternTable[min].text) < 0))
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
	
	//count valid patterns
	for (size_t i = 0; i < szTable; i++)
	{
		if(patternTable[i].text == NULL)
		{
			return i;
		}
	}

	return szTable;
}


void GenericATLexer::startFLRmode(size_t length)
{
	if (length <= 0 || length > GATC_RECV_BUFFER_MAX)
	{
		return;
	}
	
	fixed_recv_length = length;
	inFLRmode = true;
}


size_t GenericATLexer::enclose_field()
{
	bool inQuotation = false;
	size_t end_pos = recv_count;	//都找不到逗號的話，最後一定會結束在recv_count

	for(; parse_pos < recv_count; parse_pos++)
	{
		if(responseBuffer[parse_pos] == '"')
		{
			inQuotation = !inQuotation;
		}
		else if(responseBuffer[parse_pos] == ',' && !inQuotation)
		{
			end_pos = parse_pos;
			parse_pos++;
			break;
		}
	}
	
	return end_pos;
}


static int hex_to_int(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}


bool GenericATLexer::parseInt32(int32_t *pInt32, int base /*= 10*/)
{
	//search position has hit the end.
	if(parse_pos >= recv_count)
	{
		return false;
	}

	size_t start_pos = parse_pos;
	size_t end_pos = enclose_field();
	
	//invalid base
	if(base < 2 || base > 16) return false;

	int32_t n = 0;
	bool isNegative = false;

	if(responseBuffer[start_pos] == '-')
	{
		isNegative = true;
		start_pos++;
	}

	for(size_t i = start_pos; i < end_pos; i++)
	{
		int digit = hex_to_int(responseBuffer[i]);
		
		if(digit < 0 || digit >= base)	//Not a Number
		{
			return false;
		}
		else
		{
			n *= base;
			n += digit;
		}
	}

	if(isNegative)
	{
		//parseInt() don't accept "-" or "-0"
		if(n == 0) return false;
		*pInt32 = -n;
	}
	else
	{
		*pInt32 = n;
	}

	return true;
}


size_t GenericATLexer::parseString(char *dst, size_t maxLength)
{
	//search position has hit the end.
	if(parse_pos >= recv_count) return 0;
	
	size_t start_pos = parse_pos;
	size_t end_pos = enclose_field();
	
	if(maxLength == 0) return 0;	//來亂的?
	maxLength--;					//最大長度要留一格給NUL
	
	size_t actualLength = 0;
	
	for(size_t i = start_pos; i < end_pos; i++)
	{
		if(actualLength >= maxLength)
		{
			break;
		}
	
		dst[actualLength] = responseBuffer[i];
		actualLength++;
	}
	
	if(actualLength > 0) dst[actualLength] = '\0';
	
	return actualLength;
}



#ifdef SCC_TOKEN_MONITOR

void GenericATLexer::listAllPatterns()
{
	monitorSerial.println(F("**** SCC Pattern Table ****"));
	
	for (size_t i = 0; i < patternTableSize; i++)
	{
		monitorSerial.write(patternTable[i].prefixMatch? '^' : ' ');
		
		if (patternTable[i].text)
		{
			monitorSerial.write('"');
			monitorSerial.print((__FlashStringHelper *) patternTable[i].text);
			monitorSerial.write('"');
		}
		else
		{
			monitorSerial.print(F("(null)"));
		}
		
		monitorSerial.print(F(" = "));
		monitorSerial.println(patternTable[i].token);
	}
	
	monitorSerial.print(F("****** Total "));
	monitorSerial.print(patternTableSize);
	monitorSerial.println(F(" patterns"));
	monitorSerial.println();
}

#endif
