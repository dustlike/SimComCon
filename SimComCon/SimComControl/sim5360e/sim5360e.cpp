/*
 * sim5360e.cpp
 *
 * Created: 2017/6/29 下午 04:06:57
 *  Author: user
 */ 


#include "sim5360e.h"
#include "trace.h"



int SIM5360E::startConnection(const char *host, IPAddress ip_addr, uint16_t port)
{
	uint32_t start;
	
	debugSerial.print(F("connect to "));
	if (host)
	{
		debugSerial.print(host);
	}
	else
	{
		debugSerial.print(F("(IP)"));
		debugSerial.print(ip_addr);
	}
	debugSerial.write(':');
	debugSerial.println(port);
	
	
	//check GPRS registration
	if (execATCommand(WaitGPRSReady) != EXEC_OK) goto conn_fail;
	
	//check packet network is opened?
	if (execATCommand(CheckNetwork) != EXEC_OK) goto conn_fail;
	if (!CheckNetwork.net_opened)
	{
		//open network
		if (execATCommand(OpenNetwork) != EXEC_OK) goto conn_fail;
	}
	
	//check socket state
	if (execATCommand(SocketState) != EXEC_OK) goto conn_fail;
	if (SocketState.s0_active == 1)
	{
		//close s0
		if (execATCommand(SocketClose) != EXEC_OK) goto conn_fail;
	}
	
	//connect to server
	SocketConnect.host = host;
	SocketConnect.ip_addr = ip_addr;
	SocketConnect.port = port;
	if (execATCommand(SocketConnect) != EXEC_OK) goto conn_fail;
	
	lastReadPacketRequest = millis() - READ_REQ_COOLDOWN;
	
	connect_state = CONNECTED;
	
	return 1;
	
	
conn_fail:
	
	debugSerial.println("connection failed");
	
	return 0;
}


bool SIM5360E::startup()
{
	uint32_t start;
	
	mark_as_disconnect();
	
	if(!powerOff()) return false;
	
	clearUart();
	
	if(!powerOn()) return false;
	
	//test UART communication alive
	start = millis();
	while (execATCommand(TestAlive) < 0)
	{
		if (millis() - start > 60000)
		{
			debugSerial.println("[TestAlive] time out");
			return false;
		}
	}
	
	//disable command echo
	if (execATCommand(NoEcho) != EXEC_OK) return false;
	
	//test SIM PIN
	if (execATCommand(QueryPIN) != EXEC_OK) return false;
	
	//check PIN state
	if (QueryPIN.needPIN)
	{
		//enter PIN
		if (execATCommand(EnterPIN) != EXEC_OK) return false;
	}
	
	//將解鎖PIN後的Unsolicited Response給全部清掉
	start = millis();
	do 
	{
		execATCommand();
	} while (millis() - start < 5000);
	
	//AT+CGSOCKCONT=1,"IP","INTERNET"
	if (execATCommand(DefinePdpContext) != EXEC_OK) return false;
	
	//AT+CSOCKSETPN=1
	if (execATCommand(SetActiveProfile) != EXEC_OK) return false;
	
	//AT+CIPMODE=0
	if (execATCommand(NonTransparentMode) != EXEC_OK) return false;
	
	//AT+CIPTIMEOUT=45000,60000,40000
	if (execATCommand(SetTimeoutParameter) != EXEC_OK) return false;
	
	//AT+CIPHEAD=0
	if (execATCommand(NoIPHeader) != EXEC_OK) return false;

	//AT+CIPSRIP=0
	if (execATCommand(HideRemoteHost) != EXEC_OK) return false;

	//AT+CIPRXGET=1
	if (execATCommand(SetManualReceive) != EXEC_OK) return false;
	
	return true;
}


void SIM5360E::setPIN(const char *pin, void (*pin_error_handler)(void))
{
	if (strlen(pin) != 4)
	{
		debugSerial.print("PIN '");
		debugSerial.print(pin);
		debugSerial.println("' length is not correct");
		return;
	}
	
	memcpy(EnterPIN.pin, pin, 4);
	EnterPIN.onPINError = pin_error_handler;
	
	debugSerial.print("set PIN to ");
	debugSerial.write(EnterPIN.pin, 4);
	debugSerial.println();
}


#if 0
const char* SIM5360E::text_connec_state()
{
	switch(connect_state)
	{
		case DISCONNECTED:
		return PSTR("disconnected");
		
		case POWER_DOWN:
		return PSTR("power OFF");
		
		case POWER_ON:
		return PSTR("power ON");
		
		case UART_OK:
		return PSTR("UART test OK");
		
		case SIM_UNLOCK:
		return PSTR("SIM unlocked");
		
		case NETWORK_REGISTERED:
		return PSTR("AT+CGREG OK");
		
		case NETWORK_OPEN:
		return PSTR("3G network open");
		
		case CONNECTED:
		return PSTR("connected");
	}
}
#endif


//////////////////////////////////////////////////////////////////////////


int SIM5360E::available()
{
	int buffer_remain = ReadTCPStream.remain();
	
	if (buffer_remain <= 0)
	{
		if (millis() - lastReadPacketRequest > READ_REQ_COOLDOWN)
		{
			//send AT+CIPRXGET=4 to check data incoming
			execATCommand(CheckDataIncoming);
			lastReadPacketRequest = millis();
		}
		else
		{
			//trigger global handler to "listen" data incoming
			execATCommand();
		}
		
		if (IncomingTransmission || ReadTCPStream.rest_len > 0)
		{
			IncomingTransmission = false;
			
			execATCommand(ReadTCPStream);
			
			lastReadPacketRequest = millis();
		}
	}
	
	return buffer_remain;
}


int SIM5360E::peek()
{
	return (available() > 0)? ReadTCPStream.readBuffer[ReadTCPStream.front] : -1;
}


int SIM5360E::read()
{
	if (available() > 0)
	{
		return ReadTCPStream.readBuffer[ReadTCPStream.front++];
	}
	
	return -1;
}


int SIM5360E::read(uint8_t *buf, size_t size)
{
	int byteRead = 0, data;
	
	while (byteRead < size)
	{
		if ((data = read()) < 0)
		{
			break;
		}
		
		buf[byteRead++] = data;
	}
	
	return byteRead;
}


int SIM5360E::connect(IPAddress ip, uint16_t port)
{
	return startConnection(nullptr, ip, port);
}


int SIM5360E::connect(const char *host, uint16_t port)
{
	return startConnection(host, IPAddress(), port);
}


size_t SIM5360E::write(const uint8_t *data, size_t dlen)
{
	IPSendFixedLength.source = data;
	IPSendFixedLength.length = dlen;
	
	return (execATCommand(IPSendFixedLength) >= 0)? dlen : 0;
}


size_t SIM5360E::write(uint8_t data)
{
	IPSendFixedLength.source = &data;
	IPSendFixedLength.length = 1;
	return (execATCommand(IPSendFixedLength) >= 0)? 1 : 0;
}


void SIM5360E::flush()
{
	uart->flush();
}


void SIM5360E::stop()
{
	if (execATCommand(SocketClose) >= 0)
	{
		if (SocketClose.errno != 0)
		{
			debugSerial.print("!!!!!! SIM5360E::stop() errno = ");
			debugSerial.println(SocketClose.errno);
		}
		
		//目前暫定忽略任何非零的errno，只要有回應+CIPCLOSE就算成功stop()
		mark_as_disconnect();
	}
}


uint8_t SIM5360E::connected()
{
	return connect_state == CONNECTED;
}


SIM5360E::operator bool()
{
	return connected();
}
