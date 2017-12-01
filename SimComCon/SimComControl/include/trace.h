/*
 * trace.h
 *
 * Created: 2017/9/22 上午 11:14:29
 *  Author: user
 */ 


#ifndef TRACE_H_
#define TRACE_H_


#define debugSerial Serial


#define trace_print(...) debugSerial.print(__VA_ARGS__)
#define trace_printF(str) debugSerial.print(F(str))
#define trace_println(...) debugSerial.println(__VA_ARGS__)
#define trace_printlnF(str) debugSerial.println(F(str))
#define trace_printByte(x) { \
	if((x) < 0x10) debugSerial.print('0'); \
	debugSerial.print(x, HEX); \
}


#endif /* TRACE_H_ */