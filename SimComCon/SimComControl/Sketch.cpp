#include "Arduino.h"
#include "sim5360e/sim5360e.h"
#include "PubSubClient.h"
#include "trace.h"
#include "CMTCS.h"
#include "Keypad.h"


#define MODEM_UART Serial2


SIM5360E modem(&MODEM_UART);




static void common_setup()
{
	debugSerial.begin(115200);
#ifdef SCC_TOKEN_MONITOR
	monitorSerial.begin(115200);
#endif
	
	const char* system_startup_message();
	
	debugSerial.print(F("SimCom Controller(SimComCon) w/MQTT"));
	debugSerial.println((__FlashStringHelper *) system_startup_message());
	
#ifdef SCC_TOKEN_MONITOR
	monitorSerial.print(F("GATL Token Monitor Terminal"));
	monitorSerial.println((__FlashStringHelper *) system_startup_message());
	monitorSerial.flush();
#endif
	
	modem.begin();
	
	debugSerial.print("size of SIM5360E = ");
	debugSerial.println(sizeof(SIM5360E));
	
#ifdef SCC_TOKEN_MONITOR
	modem.listAllPatterns();
#endif
}



#if 1


bool isKeyPress()
{
	static byte swPin1 = 47;	//連接的腳位1
	static byte swPin2 = 43;	//連接的腳位2
	
	static char the_key_value = '#';	//隨便設.
	static Keypad customKeypad = Keypad(&the_key_value, &swPin1, &swPin2, 1, 1);
	
	return customKeypad.getKey() == the_key_value;
}



void setup()
{
	common_setup();
	
	#if 1
	if (!modem.startup())
	{
		debugSerial.println("modem startup failed");
	}
	else
	{
		//IMEI
		debugSerial.print("IMEI: ");
		modem.execATCommand(modem.ReadIMEI);
		debugSerial.println(modem.ReadIMEI.cachedIMEI);
		
		//ICCID
		debugSerial.print("ICCID: ");
		modem.execATCommand(modem.ReadICCID);
		debugSerial.println(modem.ReadICCID.cachedICCID);
		
		//signal quality
		debugSerial.print("SQ = ");
		modem.execATCommand(modem.SignalQuality);
		debugSerial.println(modem.SignalQuality.rssi_level);
		
		//連到測試伺服器
		modem.connect("cloud.simis.tw", 9487);
	}
	#endif
}


void loop()
{
	static bool plain_mode = true;
	
	if (!plain_mode)
	{
		if (modem.connected() && modem.available()) debugSerial.write(modem.read());
	}
	else
	{
		if (MODEM_UART.available()) debugSerial.write(MODEM_UART.read());
		if (debugSerial.available()) MODEM_UART.write(debugSerial.read());
	}
	
	if (isKeyPress())
	{
		plain_mode = !plain_mode;
		debugSerial.print("switch to ");
		debugSerial.println(plain_mode? "plain mode" : "scc mode");
	}
}


#endif



//////////////////////////////////////////////////////////////////////////

#if 0

static unsigned long lastSend = 0;


static void callback(char *topic, uint8_t *payload, unsigned int len)
{
	debugSerial.print("-> [");
	debugSerial.print(topic);
	debugSerial.print("](");
	debugSerial.print(len);
	debugSerial.print("): ");
	debugSerial.write(payload, len);
	debugSerial.println();
}



PubSubClient mqtt(modem);

void setup()
{
	common_setup();
	
	mqtt.setServer("220.134.69.15", 61613);
	mqtt.setCallback(callback);
	
	randomSeed(analogRead(0));
}


static char *message[] =
{
	"Hello, little star",
	"Are you doing fine?",
	"I'm lonely as everything in birth",

	"Sometimes in the dark",
	"When I close my eyes",
	"I dream of you, the planet earth",

	"If I could fly across this night",
	"Faster than the speed of light",
	"I would spread these wings of mine",

	"Through the years and far away",
	"Far beyond the milky way",
	"See the shine that never blinks",
	"The shine that never fades",

	"Thousand years and far away",
	"Far beyond the silky way",
	"You're the shine that never blinks",
	"The shine that never dies",

	"-+-+-+-+-+-+-+-+-+-+-"
};


void loop()
{
	while (!mqtt.connected())
	{
		debugSerial.println("Attempting MQTT connection...");
		if (mqtt.connect("SimComCon", "admin", "9527"))
		{
			mqtt.subscribe("inTopic");
			debugSerial.println("MQTT connected");
		}
		else
		{
			debugSerial.println("failed");
			
		}
	}
	
	mqtt.loop();
	
	#if 1
	uint32_t now = millis();
	if (now - lastSend >= 10000 || lastSend == 0)
	{
		static int idx = 0;
		debugSerial.print("<- ");
		debugSerial.println(message[idx]);
		mqtt.publish("outTopic", message[idx]);
		idx = (idx + 1) % (sizeof(message) / sizeof(message[0]));
		
		lastSend = now;
	}
	#endif
}

#endif