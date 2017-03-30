#include "stdafx.h"

#include "ColorWipeAnimation.h"

/*virtual*/ void ColorWipeColorUpdateProcess::service()
{
	SUPER::service();
	
	addOffset(_colorSettings._Color1, _colorSettings.colorOffset);
}


/*virtual*/ void ColorWipeLedUpdateProcess::service()
{
	CRGB color= (_Direction == DT_FORWARD)? _colorSettings._Color1 : CRGB::Black; // TODO: fade instead of black
	setPixelColor(_Index, color);
	show();
	
	Increment();
}

/*virtual*/ void ColorWipeLedUpdateProcess::Increment()
{
	if (_Direction == DT_FORWARD)
	{
		if (_Index < _TotalSteps - 1)
			++_Index;
		else 
		{
			_Direction= DT_REVERSE;
		}
	}
	else // Direction == REVERSE
	{
		if  (_Index > 0)
			--_Index;
		else
		{
			_Direction= DT_FORWARD;
		}
	}
}