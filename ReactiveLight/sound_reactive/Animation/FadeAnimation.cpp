#include "stdafx.h"

#include "FadeAnimation.h"

/*virtual*/ void FadeLedUpdateProcess::service()
{
	CRGB color1= _colorSettings._Color1;
	CRGB color2= _colorSettings._Color2;
	
	// Calculate linear interpolation between Color1 and Color2
	// Optimize order of operations to minimize truncation error
	uint8_t red = ((Red(color1) * (_TotalSteps - _Index)) + (Red(color2) * _Index)) / _TotalSteps;
	uint8_t green = ((Green(color1) * (_TotalSteps - _Index)) + (Green(color2) * _Index)) / _TotalSteps;
	uint8_t blue = ((Blue(color1) * (_TotalSteps - _Index)) + (Blue(color2) * _Index)) / _TotalSteps;
	
	CRGB color(red, green, blue);
	CRGB* leds= getLEDs();
	const int numLEDs= getNumLEDs();
	
	const int show_amount = AmountLedsToShow();
	
	for (int i = 0; i < show_amount; i++)
	{
		leds[i]= color;
	}
	
	//All the other LEDs begin their fading journey to eventual total darkness
	float fadeScale= _colorSettings.fadeScale;
	for (int i = show_amount; i < numLEDs; i++)
	{
		leds[i]= CRGB::Black;
	}
	
	show();
	Increment();
}
