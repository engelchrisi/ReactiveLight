#include "stdafx.h"

#include "RainbowAnimation.h"

/*virtual*/ void RainbowLedUpdateProcess::service()
{
	const int show_amount = AmountLedsToShow();
	
	const int numLEDs= getNumLEDs();
	
	CRGB* leds= getLEDs();
	for(int i=0; i< show_amount; i++)
	{
		leds[i]= Wheel(((i * 256 / numLEDs) + _Index) & 255, _modeStats.songMode);
	}
	
	//All the other LEDs begin their fading journey to eventual total darkness
	float fadeScale= _colorSettings.fadeScale;
	for (int i = show_amount; i < numLEDs; i++)
	{
		fade(leds[i], fadeScale);
	}
	
	show();
	
	if (_songStats.songAvgDirection == DT_DOWN)
	{
		//setPeriod(mIntervalMs-2);
		_Direction= DT_FORWARD;
	}
	else if (_songStats.songAvgDirection == DT_UP)
	{
		//setPeriod(mIntervalMs+2);
		_Direction= DT_REVERSE;
		
	}
	
	Increment();
}