#include "stdafx.h"

#include "TheaterChaseAnimation.h"

/*virtual*/ void TheaterChaseColorUpdateProcess::service()
{
	SUPER::service();
	
	addOffset(_colorSettings._Color1, _colorSettings.colorOffset);
	
	_colorSettings._Color2= _colorSettings._Color1;
	fade(_colorSettings._Color2, 7.0);
}



/*virtual*/ void TheaterChaseLedUpdateProcess::service()
{
	//LOGF("TheaterChaseLedUpdateProcess::service");
	const int numLEDs= getNumLEDs();
	const int maxMicLevel= _songStats.rawSoundStats.maxMicLevel;
	const int show_amount = AmountLedsToShow();
		
	CRGB* leds= getLEDs();
	CRGB color1= _colorSettings._Color1;
	CRGB color2= _colorSettings._Color2;
		
	for(int i=0; i< show_amount; i++)
	{
		if ((i + _Index) % 4 == 0)
		{
			leds[i]= color1;
		}
		else
		{
			leds[i]= color2;
		}
	}
		
	for (int i = show_amount; i < numLEDs; i++)
	{
		leds[i]= CRGB::Black;
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

TheaterChaseAnimation::TheaterChaseAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1, uint32_t color2) 
	: SUPER(manager, pController)
{
	LOGF("=== TheaterChaseAnimation ===");
		
	_colorSettings._Color1 = color1;
	#if 1
	_colorSettings._Color2= color2;
	
	#else
	_colorSettings._Color2= color1;
	fade(_colorSettings._Color2, 7.0);
	#endif
	
	//_sampleProcess.setPeriod(50);
	////_colorUpdateProcess.setPeriod(200);
	//_ledUpdateProcess.setPeriod(50);
}

