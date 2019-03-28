/*
 * generic_AT_lexer.cpp
 *
 * Created: 2017/5/26 下午 03:26:28
 *  Author: user
 */ 


#include "generic_AT_lexer.h"



GATL::Token GATL::scan(char ch)
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
	
	//special process for CR+LF
	if (ch == '\n')
	{
		//meet CR+LF, the end of line
		if(prevChar == '\r')
		{
			resetLine = true;
			
			//add NUL character, make it easy to do string operation
			if (recv_count > GATL_RESPONSE_BUFFER_MAX)
				responseBuffer[GATL_RESPONSE_BUFFER_MAX] = '\0';
			else
				responseBuffer[recv_count] = '\0';
			
			//only one candidate left, and hit the end
			//or already "matched prefix"
			if ( (matchBegin + 1 == matchEnd && '\0' == patternTable[matchBegin].text[recv_count])
				|| matched_prefix )
			{
				//將parse_pos定到第一個非空白字元
				for (parse_pos = 1; parse_pos < recv_count; parse_pos++)
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
	else if(ch == '\r')
	{
		//just ignore CR
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
				if (ch == patternTable[k].text[recv_count])
				{
					break;
				}
			}
			matchBegin = k;
			
			for (; k < matchEnd; k++)
			{
				if (ch != patternTable[k].text[recv_count])
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
					'\0' == patternTable[matchBegin].text[recv_count + 1] )
				{
					matched_prefix = true;
					recv_count = 0;
				}
			}
		}
		
		// put data into buffer. will not put in if buffer overflow
		if (recv_count < GATL_RESPONSE_BUFFER_MAX)
		{
			responseBuffer[recv_count] = ch;
		}
		
		recv_count++;
	}
	
	prevChar = ch;
	
	return RT_NONE;
}


size_t GATL::initPatternTable(ATResponsePattern *patternTable, size_t szTable, void (*patternRegister)())
{
	memset(patternTable, 0, szTable);
	
	patternRegister();
	
	//selection sort
	for (size_t i = 0; i < szTable - 1; i++)
	{
		size_t min = i;
		
		for (size_t j = i + 1; j < szTable; j++)
		{
			if (patternTable[j].text != NULL &&
			(patternTable[min].text == NULL || strcmp(patternTable[j].text, patternTable[min].text) < 0))
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


size_t GATL::enclose_field()
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


bool GATL::parseInt32(int32_t *pInt32, int base /*= 10*/)
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


size_t GATL::parseString(char *dst, size_t maxLength)
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

void GATL::listAllPatterns()
{
	const char * quote = "\"";
	
	printf("**** SCC Pattern Table ****\r\n");
	
	for (size_t i = 0; i < patternTableSize; i++)
	{
		printf("%c%s = %d\r\n", (patternTable[i].prefixMatch)? '^' : ' ', patternTable[i].text, patternTable[i].token);
	}
	
	printf("****** Total %u patterns\r\n", patternTableSize);
}

#endif
