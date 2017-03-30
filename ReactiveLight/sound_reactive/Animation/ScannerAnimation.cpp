#include "stdafx.h"

#include "ScannerAnimation.h"

/*virtual*/ void ScannerColorUpdateProcess::service()
{
	SUPER::service();
	
	addOffset(_colorSettings._Color1, _colorSettings.colorOffset);
}


/*virtual*/ void ScannerLedUpdateProcess::service()
{
	CRGB color1= _colorSettings._Color1;
	
	CRGB* leds= getLEDs();
	
	for (int i = 0; i < getNumLEDs(); i++)
	{
		if (i == _Index)  // Scan Pixel to the right
		{
			leds[i]= color1;
		}
		else if (i == _TotalSteps - _Index) // Scan Pixel to the left
		{
			leds[i]= color1;
		}
		else // Fading tail
		{
			float fadeScale= 1.2;
			fade(leds[i], fadeScale); 
		}
	}
	
	show();
	Increment();
}
