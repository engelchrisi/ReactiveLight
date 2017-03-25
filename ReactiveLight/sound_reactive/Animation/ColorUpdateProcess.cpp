#include "stdafx.h"

#include "ColorUpdateProcess.h"


ColorUpdateProcess::ColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
										const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
	: _modeStats(modeStats), _songStats(songStats), _colorSettings(colorSettings), Process(manager, pr, period)
{
}
	
/*virtual*/ void ColorUpdateProcess::service()
{

	//LOGF("ColorUpdateProcess");
	float fadeScale= 1.2;
	RgbColorT colorOffset;
		
	if (_modeStats.songMode == SM_NORMAL) {
		fadeScale = 2.0;
		colorOffset.r = -1;
		colorOffset.g = 1;
		colorOffset.b = 2;
	}
	else if (_modeStats.songMode == SM_HIGH) {
		fadeScale = 3.0;
		colorOffset.r =  5;
		colorOffset.g =  3;
		colorOffset.b = -1;
	}
		
	_colorSettings.fadeScale= fadeScale;
	_colorSettings.colorOffset= colorOffset;
		
	//LOGF("EOF ColorUpdateProcess");
}
