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

class LedUpdateProcessBaseEx : public LedUpdateProcessBase
{
	typedef LedUpdateProcessBase SUPER;
	public:
	enum  DirectionT { DT_FORWARD, DT_REVERSE };

	public:
	LedUpdateProcessBaseEx(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
	const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
	: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps= getNumLEDs();
		_Index = 0;
		_Direction = DT_REVERSE;
	}

	virtual void Increment()
	{
		if (_Direction == DT_FORWARD)
		{
			_Index= (_Index < _TotalSteps)? _Index+1 : 0;
		}
		else // Direction == REVERSE
		{
			_Index= (_Index > 0)? _Index-1 : _TotalSteps-1;
		}
	}
	
protected:
	DirectionT _Direction;     // direction to run the pattern
	
	uint16_t _TotalSteps;  // total number of steps in the pattern
	uint16_t _Index;  // current step within the pattern
};

#endif //__LEDUPDATEPROCESSBASE_H__
