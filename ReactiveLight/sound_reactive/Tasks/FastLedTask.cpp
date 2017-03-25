#include "stdafx.h"

#if 0

#include "FastLedTask.h"
#include "SoundUtils.h"

struct RgbColorT g_ColorOffset;



// Constructor - calls base-class constructor to initialize strip
FastLedTask::FastLedTask(CLEDController& controller, 
							unsigned long liftimeMs,
						   PatternT  pattern0, uint32_t color1, uint32_t color2, DirectionT dir) : FastLedTaskBase(controller)
	, Process(0 /*intervalMs*/, liftimeMs)
{
	mRuntimeMs= 4000;
		
	_ActivePattern= pattern0;
	
	switch(_ActivePattern)
	{
		case PT_RAINBOW_CYCLE:
			InitRainbowCycle(dir);
			break;
		case PT_THEATER_CHASE:
			InitTheaterChase(color1, color2, dir);
			break;
		case PT_COLOR_WIPE:
			InitColorWipe(color1, dir);
			break;
		case PT_SCANNER:
			InitScanner(color1, dir);
			break;
		case PT_FADE:
			InitFade(color1, color2, 10, dir);
			break;
		
		default:
		LOGF("FastLedTask::execute: WHICH mode?");
		break;
	}
#ifdef DPIN_NEO_5V
	pinMode(DPIN_NEO_5V, OUTPUT);
	digitalWrite(DPIN_NEO_5V, HIGH);
#endif
}

FastLedTask::~FastLedTask()
{
	#ifdef DPIN_NEO_5V
	digitalWrite(DPIN_NEO_5V, LOW);
	#endif
}

	
// Three of these time-weightings have been internationally standardized, 'S' (1 s) originally called Slow, 'F' (125 ms) originally called Fast and 'I' (35 ms) originally called Impulse.
#define LOG2_SAMPLES	8	

	
// Update the pattern
/*virtual*/ void FastLedTask::execute()
{
	//LOGF("FastLedTask::execute:");
	
	if (!updateSoundStatistics(LOG2_SAMPLES))
		return;

	switch(_ActivePattern)
	{
		case PT_RAINBOW_CYCLE:
			RainbowCycleUpdate();
			break;
		case PT_THEATER_CHASE:
			TheaterChaseUpdate();
			break;
		case PT_COLOR_WIPE:
			ColorWipeUpdate();
			break;
		case PT_SCANNER:
			ScannerUpdate();
			break;
		case PT_FADE:
			FadeUpdate();
			break;
		
		default:
			//LOGF("FastLedTask::execute: WHICH mode?");
		break;
	}
}

// Increment the Index and reset at the end
void FastLedTask::Increment()
{
	if (_Direction == DT_FORWARD)
	{
		_Index++;
		if (_Index >= _TotalSteps)
		{
			_Index = 0;
		}
	}
	else // Direction == REVERSE
	{
		--_Index;
		if (_Index <= 0)
		{
			_Index = _TotalSteps-1;
		}
	}
}

// Reverse pattern direction
void FastLedTask::Reverse()
{
	if (_Direction == DT_FORWARD)
	{
		_Direction = DT_REVERSE;
		_Index = _TotalSteps-1;
	}
	else
	{
		_Direction = DT_FORWARD;
		_Index = 0;
	}
}

// Initialize for a RainbowCycle
void FastLedTask::InitRainbowCycle(DirectionT dir)
{
	_ActivePattern = PT_RAINBOW_CYCLE;
	_TotalSteps = 255;
	_Index = 0;
	_Direction = dir;

	setInterval(random(5,10));
}

// Update the Rainbow Cycle Pattern
void FastLedTask::RainbowCycleUpdate()
{
	const int numLEDs= getNumLEDs();
	for(int i=0; i< numLEDs; i++)
	{
		setPixelColor(i, Wheel(((i * 256 / numLEDs) + _Index) & 255, _modeStats.songMode));
	}
	show();
	Increment();
}

// Initialize for a Theater Chase
void FastLedTask::InitTheaterChase(uint32_t color1, uint32_t color2, DirectionT dir)
{
	_ActivePattern = PT_THEATER_CHASE;
	_TotalSteps = getNumLEDs();
	_Color1 = color1;
	#if 1
	_Color2= color2;
	
	#else
	_Color2= color1;
	fade(_Color2, 7.0);
	#endif
	_Index = 0;
	_Direction = dir;
	
	setInterval( mRuntimeMs / _TotalSteps);
}

// Update the Theater Chase Pattern
void FastLedTask::TheaterChaseUpdate()
{
	float fade_scale= 1.2;
	RgbColorT colorOffset;
				
	if (_modeStats.songMode == SM_NORMAL) {
		fade_scale = 2.0;
		colorOffset.r = -1;
		colorOffset.g = 1;
		colorOffset.b = 2;
	}
	else if (_modeStats.songMode == SM_HIGH) {
		fade_scale = 3.0;
		colorOffset.r =  5;
		colorOffset.g =  3;
		colorOffset.b = -1;
	}
#if 1		
	addOffset(_Color1, colorOffset);
	
	_Color2= _Color1;
	fade(_Color2, 7.0);
#endif
	const int numLEDs= getNumLEDs();
	const int maxMicLevel= _songStats.rawSoundStats.maxMicLevel;
	const int sensorValue= min(_songStats.sensorValue, maxMicLevel);
	const int show_amount = map(sensorValue, 0, maxMicLevel, 0, numLEDs);
	
	CRGB* leds= getLEDs();
	
	for(int i=0; i< show_amount; i++)
	{
		if ((i + _Index) % 4 == 0)
		{
			leds[i]= _Color1;
		}
		else
		{
			leds[i]= _Color2;
		}
	}
	
	for (int i = show_amount; i < numLEDs; i++)
	{
		leds[i]= CRGB::Black;
	}

	show();
	
	if (_songStats.songAvgDirection == DT_DOWN)
	{
		//setInterval(mIntervalMs-2);
		_Direction= DT_FORWARD;
	}
	else if (_songStats.songAvgDirection == DT_UP)
	{
		//setInterval(mIntervalMs+2);
		_Direction= DT_REVERSE;
		
	}
	//refreshInterval();
	
	Increment();
}

// Initialize for a ColorWipe
void FastLedTask::InitColorWipe(uint32_t color, DirectionT dir)
{
	_ActivePattern = PT_COLOR_WIPE;
	_TotalSteps = getNumLEDs();
	_Color1 = color;
	_Index = 0;
	_Direction = dir;
	
	setInterval( mRuntimeMs / _TotalSteps);
}

// Update the Color Wipe Pattern
void FastLedTask::ColorWipeUpdate()
{
	setPixelColor(_Index, _Color1);
	show();
	Increment();
}

// Initialize for a SCANNNER
void FastLedTask::InitScanner(uint32_t color1, DirectionT dir)
{
	setInterval(55);
	
	_ActivePattern = PT_SCANNER;
	_TotalSteps = (getNumLEDs() - 1) * 2;
	_Color1 = color1;
	_Index = 0;
	_Direction = dir;
	
	setInterval( mRuntimeMs / _TotalSteps);
}

// Update the Scanner Pattern
void FastLedTask::ScannerUpdate()
{
	for (int i = 0; i < getNumLEDs(); i++)
	{
		if (i == _Index)  // Scan Pixel to the right
		{
			setPixelColor(i, _Color1);
		}
		else if (i == _TotalSteps - _Index) // Scan Pixel to the left
		{
			setPixelColor(i, _Color1);
		}
		else // Fading tail
		{
			setPixelColor(i, DimColor(getPixelColor(i)));
		}
	}
	show();
	Increment();
}

// Initialize for a Fade
void FastLedTask::InitFade(uint32_t color1, uint32_t color2, uint16_t steps, DirectionT dir)
{
	_ActivePattern = PT_FADE;
	_TotalSteps = steps;
	_Color1 = color1;
	_Color2 = color2;
	_Index = 0;
	_Direction = dir;
	
	setInterval( mRuntimeMs / _TotalSteps);
}

// Update the Fade Pattern
void FastLedTask::FadeUpdate()
{
	// Calculate linear interpolation between Color1 and Color2
	// Optimize order of operations to minimize truncation error
	uint8_t red = ((Red(_Color1) * (_TotalSteps - _Index)) + (Red(_Color2) * _Index)) / _TotalSteps;
	uint8_t green = ((Green(_Color1) * (_TotalSteps - _Index)) + (Green(_Color2) * _Index)) / _TotalSteps;
	uint8_t blue = ((Blue(_Color1) * (_TotalSteps - _Index)) + (Blue(_Color2) * _Index)) / _TotalSteps;
	
	ColorSet(Color(red, green, blue));
	show();
	Increment();
}

// Set all pixels to a color (synchronously)
void FastLedTask::ColorSet(uint32_t color)
{
	for (int i = 0; i < getNumLEDs(); i++)
	{
		setPixelColor(i, color);
	}
	show();
}



#endif