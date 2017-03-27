#ifndef _STDAFX_h
#define _STDAFX_h

#include <Arduino.h>
#include <FastLED.h>
#include <ProcessScheduler.h>


#ifndef _countof
    #define _countof(buf) (sizeof(buf)/sizeof(buf[0]))
#endif
#define MAX_ULONG	0xFFFFFFFF

// modes
#define EXTERNAL_VOL_REF		// use 3.3 voltage. the 5v voltage from usb is not regulated, this is much more stable.
#define ADCReClock	// switch to higher clock, not needed if we are OK with freq between 0 and 4Khz.
#define ADCFlow		// read data from adc with free-run (not interrupt). much better data, dc low. hard coded for A0.

#include "pins.h"


#include "Logging.h"
#include "Utils.h"
#include "SoundUtils.h"
#include "LedUtils.h"


// use utils::delayMs instead
//#define delay				DO_NOT_USE_DELAY
//#define delayMicroseconds	DO_NOT_USE_DELAY

extern bool _DEBUG;


#endif