#include "stdafx.h"
#include "utils.h"

void Utils::delayMs(unsigned long delayMs)
{
	#undef delay
	::delay(delayMs);
}


void Utils::busyDelayMs(unsigned long maxDelayMs, unsigned long inbetweenDelayMs, CallbackT pCallback, void* pUserData)
{
    const unsigned long startTimeMs = millis();
    const unsigned long endTimeMs = startTimeMs + maxDelayMs;
	unsigned long	delta;
    unsigned long currentTimeMs;
    bool res;
    
    do
    {
        res = pCallback(pUserData);
        if (!res)
            break;
        
		delta= endTimeMs - millis();
        if (inbetweenDelayMs > 0) {
            delayMs(min(inbetweenDelayMs, delta)); // SYNC
        }
        
        currentTimeMs = millis();
    } while (currentTimeMs < endTimeMs);
    
    /*TESTLOGF2("StartTime=", startTimeMs);
     TESTLOGF2("maxDelayMs=", maxDelayMs);
     TESTLOGF2("currentTimeMs=", currentTimeMs);
     TESTLOGF2("endTimeMs=", endTimeMs);*/
}


// http://forum.arduino.cc/index.php?topic=49581.0
void Utils::softReset()
{
	LOGF("Forcing Reboot");
	Utils::delayMs(250); // SYNC

	asm volatile ("jmp 0");
}