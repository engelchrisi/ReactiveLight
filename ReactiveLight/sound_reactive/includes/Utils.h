// Utils.h

#ifndef _UTILS_h
#define _UTILS_h

class Utils
{
	public:

	 // return true => continue busyDelay otherwise interrupt
	 typedef bool(*CallbackT)(void* pUserData);

	static void setup();
	static void delayMs(unsigned long delayMs);
	static void busyDelayMs(unsigned long maxDelayMs, unsigned long inbetweenDelayMs, CallbackT pCallback, void* pUserData);
    
	static void softReset();

    
};


#endif

