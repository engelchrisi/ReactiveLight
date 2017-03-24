/* 
* DiscoLightTask.cpp
*
* Created: 19.03.2017 21:54:29
* Author: D031288
*/

#include "stdafx.h"
#include "SoundUtils.h"

#include "DiscoLightTask.h"


DiscoLightTask::DiscoLightTask(CLEDController& controller, unsigned long liftimeMs) 
	: RecurringTask(100, liftimeMs), FastLedTaskBase(controller)
{
}

// Three of these time-weightings have been internationally standardized, 'S' (1 s) originally called Slow, 'F' (125 ms) originally called Fast and 'I' (35 ms) originally called Impulse.
#define LOG2_SAMPLES	8 //12
	
/*virtual*/ void DiscoLightTask::execute()
{
	//LOGF("#BEFORE");
	if (updateSoundStatistics(LOG2_SAMPLES))
	{
	//LOGF("#AFTER");
	
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
			
		updateLEDs(fade_scale, colorOffset);
	}
	else
	{
	LOGF("AFTER2");
		
	}
}

void DiscoLightTask::updateLEDs(float fade_scale, RgbColorT colorOffset)
{
	const int numLEDs= getNumLEDs();
	const int maxMicLevel= _songStats.rawSoundStats.maxMicLevel;
	const int sensorValue= min(_songStats.sensorValue, maxMicLevel);
	const int show_amount = map(sensorValue, 0, maxMicLevel, 0, numLEDs);
	
	if (_DEBUG)
	{
		//LOGF2("#led=", show_amount);
	}
	
	CRGB* leds= getLEDs();

	/*Set the different leds. Control for too high and too low values.
	Fun thing to try: Dont account for overflow in one direction,
	some interesting light effects appear! */
	for (int i = 0; i < show_amount; i++)
	{
		addOffset(leds[i], colorOffset);
	}
	
	//All the other LEDs begin their fading journey to eventual total darkness
	for (int i = show_amount; i < numLEDs; i++)
	{
		fade(leds[i], fade_scale);
	}
	
	show();
}

	
