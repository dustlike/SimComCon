/*
 * config.h
 *
 * Created: 2017/9/22 上午 11:00:06
 *  Author: user
 */ 


#ifndef SIMCOMCON_CONFIG_H_
#define SIMCOMCON_CONFIG_H_


#define SCC_TOKEN_MONITOR


#ifdef SCC_TOKEN_MONITOR
#  define monitorSerial Serial1
#  define ECHO_MODEM_INPUT
#endif


#define GATC_RECV_BUFFER_MAX 127



#endif /* SIMCOMCON_CONFIG_H_ */