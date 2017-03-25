#include "stdafx.h"
#include "UpDownAnimation.h"

/*virtual*/ void UpDownLedUpdateProcess::service()
{
	//LOGF("UpDownLedUpdateProcess"); 
	const int numLEDs= getNumLEDs();
	const int show_amount = AmountLedsToShow();
	
	if (_DEBUG)
	{
		LOGF2("#led=", show_amount);
	}
	
	CRGB* leds= getLEDs();

	/*Set the different leds. Control for too high and too low values.
	Fun thing to try: Dont account for overflow in one direction,
	some interesting light effects appear! */
	RgbColorT colorOffset= _colorSettings.colorOffset;
	for (int i = 0; i < show_amount; i++)
	{
		addOffset(leds[i], colorOffset);
	}
	
	//All the other LEDs begin their fading journey to eventual total darkness
	float fadeScale= _colorSettings.fadeScale;
	for (int i = show_amount; i < numLEDs; i++)
	{
		fade(leds[i], fadeScale);
	}
	
	show();
		
	//LOGF("EOF UpDownLedUpdateProcess"); 
}
