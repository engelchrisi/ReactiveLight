#include "stdafx.h"


#include "LedUpdateProcessBase.h"

LedUpdateProcessBase::LedUpdateProcessBase(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
									const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
	: _modeStats(modeStats), _songStats(songStats), _colorSettings(colorSettings),
		Process(manager, pr, period), FastLedBase(pController)
{
}
	
	
/*virtual*/ int LedUpdateProcessBase::AmountLedsToShow() const 
{
	const int numLEDs= getNumLEDs();
	const int maxMicLevel= _songStats.rawSoundStats.maxMicLevel;
	const int sensorValue= min(_songStats.sensorValue, maxMicLevel);
		
	return map(sensorValue, 0, maxMicLevel, 0, numLEDs);
}
