#ifndef __LEDUPDATEPROCESSBASE_H__
#define __LEDUPDATEPROCESSBASE_H__

#include "FastLedBase.h"
#include "ColorUpdateProcess.h"

class LedUpdateProcessBase : public Process, public FastLedBase
{
public:
	LedUpdateProcessBase(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
						const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings);
	
protected:
	virtual void service()= 0;
	virtual int AmountLedsToShow() const;

protected:
	const ModeStatisticsT&		_modeStats;
	const SongStatisticsT&		_songStats;
	const AnimationColorSettingsT&	_colorSettings;

};

#endif //__LEDUPDATEPROCESSBASE_H__
