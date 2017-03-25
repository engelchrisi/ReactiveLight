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

//Function imported from the arduino website.
//Basically map, but with a curve on the scale (can be non-uniform).
float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{
	float OriginalRange = 0;
	float NewRange = 0;
	float zeroRefCurVal = 0;
	float normalizedCurVal = 0;
	float rangedValue = 0;
	boolean invFlag = 0;

	// condition curve parameter
	// limit range

	if (curve > 10) curve = 10;
	if (curve < -10) curve = -10;

	curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - positive numbers give more weight to high end on output
	curve = pow(10, curve); // convert linear scale into logarithmic exponent for other pow function

	// Check for out of range inputValues
	if (inputValue < originalMin) {
		inputValue = originalMin;
	}
	if (inputValue > originalMax) {
		inputValue = originalMax;
	}

	// Zero Reference the values
	OriginalRange = originalMax - originalMin;

	if (newEnd > newBegin){
		NewRange = newEnd - newBegin;
	}
	else
	{
		NewRange = newBegin - newEnd;
		invFlag = 1;
	}

	zeroRefCurVal = inputValue - originalMin;
	normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

	// Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
	if (originalMin > originalMax ) {
		return 0;
	}

	if (invFlag == 0){
		rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

	}
	else     // invert the ranges
	{
		rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
	}

	return rangedValue;
}