#include "stdafx.h"

#include "SophiaAnimation.h"

const int MaxHinUndHerZaehler= 3;
int HinUndHerZaehler= 0;

/*virtual*/ void SophiaColorUpdateProcess::service()
{
	SUPER::service();
	//
	//addOffset(_colorSettings._Color1, _colorSettings.colorOffset);
	//addOffset(_colorSettings._Color2, _colorSettings.colorOffset);
}



/*virtual*/ void SophiaLedUpdateProcess::service()
{
	//LOGF("SophiaLedUpdateProcess::service");
	const int numLEDs= getNumLEDs();
	const int maxMicLevel= _songStats.rawSoundStats.maxMicLevel;
	const int show_amount = AmountLedsToShow();
		
	CRGB* leds= getLEDs();
	CRGB color1= _colorSettings._Color1;
	CRGB color2= _colorSettings._Color2;
	
	if (HinUndHerZaehler > MaxHinUndHerZaehler)
	{
		color1= MorpheColors(color1, CRGB::Green, _Index, _TotalSteps);
	}
		
	for(int i=0; i< show_amount; i++)
	{
		if ((i + _Index) % 3 == 0)
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
			
	Increment();
}


/*virtual*/ void SophiaLedUpdateProcess::Increment()
{
	if (_Direction == DT_FORWARD)
	{
		if (_Index < _TotalSteps - 1)
			++_Index;
		else
		{
			HinUndHerZaehler= HinUndHerZaehler + 1;
			_Direction= DT_REVERSE;
		}
	}
	else // Direction == REVERSE
	{
		if  (_Index > 0)
		--_Index;
		else
		{
			HinUndHerZaehler= HinUndHerZaehler + 1;
			_Direction= DT_FORWARD;
		}
	}
}

SophiaAnimation::SophiaAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1, uint32_t color2) 
	: SUPER(manager, pController)
{
	LOGF("=== SophiaAnimation ===");
		
	_colorSettings._Color1 = color1;
	_colorSettings._Color2= color2;
}
