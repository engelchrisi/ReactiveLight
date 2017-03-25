#ifndef __COLORUPDATEPROCESS_H__
#define __COLORUPDATEPROCESS_H__

#include "LedUtils.h"

struct AnimationColorSettingsT
{
	float fadeScale;
	RgbColorT colorOffset;
	
	CRGB	_Color1, _Color2;  // What colors are in use
	
	AnimationColorSettingsT()
	{
		memset(this, 0, sizeof(*this));
	}
};

class ColorUpdateProcess : public Process
{
public:
	ColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
						const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings);
	
protected:
	virtual void service();

protected:
	const ModeStatisticsT&		_modeStats;
	const SongStatisticsT&		_songStats;
	AnimationColorSettingsT&	_colorSettings;
};

#endif //__COLORUPDATEPROCESS_H__
