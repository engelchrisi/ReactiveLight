#include "stdafx.h"

#include "FadeAnimation.h"

/*virtual*/ void FadeLedUpdateProcess::service()
{
	CRGB color1= _colorSettings._Color1;
	CRGB color2= _colorSettings._Color2;
	
	CRGB color= MorpheColors(color1, color2, _Index, _TotalSteps);
	CRGB* leds= getLEDs();
	const int numLEDs= getNumLEDs();
	const int show_amount = AmountLedsToShow();
	
	for (int i = 0; i < show_amount; i++)
	{
		leds[i]= color;
	}
	
	float fadeScale= _colorSettings.fadeScale;
	for (int i = show_amount; i < numLEDs; i++)
	{
		leds[i]= CRGB::Black;
	}
	
	show();
	Increment();
}

/*virtual*/ void FadeLedUpdateProcess::Increment()
{
	if (_Direction == DT_FORWARD)
	{
		if (_Index < _TotalSteps - 1)
			++_Index;
		else
		{
			_colorSettings._Color1= _colorSettings._Color2;
			_colorSettings._Color2= Wheel(random(255), _modeStats.songMode);
			_Index= 0;
			//Utils::delayMs(2000);
		}
	}
	else // Direction == REVERSE
	{
		if  (_Index > 0)
			--_Index;
		else
		{
			_colorSettings._Color2= _colorSettings._Color1;
			_colorSettings._Color1= Wheel(random(255), _modeStats.songMode);
			_Index= _TotalSteps - 1;
			//Utils::delayMs(2000);
		}
	}
}