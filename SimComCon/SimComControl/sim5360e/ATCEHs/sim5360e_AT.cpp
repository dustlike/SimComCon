#include "../sim5360e.h"



void SIM5360E_ATCEH::ATCEH_TestAlive::sendCommand(Stream *uart)
{
	uart->println(F("AT"));
}
