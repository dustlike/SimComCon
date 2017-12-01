/*
 * sim5360e.h
 *
 * Created: 2017/6/29 下午 04:07:10
 *  Author: user
 */ 


#ifndef SIM5360E_H_
#define SIM5360E_H_


#include "sim5360e_ATC_exchange_handle.h"
#include "Client.h"


class SIM5360E : public SIM5360E_ATCEH, public Client
{
public:
	
	enum STATES_OF_CONNECTION
	{
		DISCONNECTED,
		POWER_DOWN,
		POWER_ON,
		UART_OK,
		SIM_UNLOCK,
		NETWORK_REGISTERED,
		NETWORK_OPEN,
		CONNECTED
	};
	
	SIM5360E(Stream *uart) : SIM5360E_ATCEH(uart)
	{
		connect_state = DISCONNECTED;
	}
	
	void mark_as_disconnect()
	{
		connect_state = DISCONNECTED;
	}
	
	bool startup();
	
	//設定PIN，長度最大4
	//pin_error_handler: 當解鎖PIN出錯時會呼叫此一函數，傳入NULL代表不呼叫.
	void setPIN(const char *pin, void (*pin_error_handler)(void));
	
	//-----------------------
	//硬體直接相關
	
	void begin();			//硬體初始化。通常在setup()中呼叫
	void end();				//硬體停止(?)
	bool powerState();		//回傳電源狀態，true:電源開 false:電源關
	void pressPowerKey();	//按一次電源鈕
	bool powerOn();			//重複嘗試開機到電源開啟，回傳值表示在本函數結束後是開機成功(true)或失敗(false)
	bool powerOff();		//重複嘗試關機到電源關閉，回傳值表示在本函數結束後是關機成功(true)或失敗(false)
	
	//-----------------------
	//Interface of 'Client'
	
	virtual int connect(IPAddress ip, uint16_t port);
	virtual int connect(const char *host, uint16_t port);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);
	virtual int available();
	virtual int read();
	virtual int read(uint8_t *buf, size_t size);
	virtual int peek();
	virtual void flush();
	virtual void stop();
	virtual uint8_t connected();
	virtual operator bool();
	
	//-----------------------
	
private:
	int startConnection(const char *addr, IPAddress ip, uint16_t port);
	
	static constexpr unsigned long READ_REQ_COOLDOWN = 10000;
	unsigned long lastReadPacketRequest;
	
	STATES_OF_CONNECTION connect_state;
};


#endif /* SIM5360E_H_ */