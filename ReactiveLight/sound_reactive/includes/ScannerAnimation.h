
#ifndef __ScannerANIMATION_H__
#define __ScannerANIMATION_H__


#include "SampleProcess.h"
#include "LedUpdateProcessBase.h"
#include "ColorUpdateProcess.h"
#include "AnimationBase.h"

class ScannerColorUpdateProcess : public ColorUpdateProcess
{
	typedef ColorUpdateProcess SUPER;
public:
	ScannerColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
								const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, modeStats, songStats, colorSettings)
	{
	}
	
protected:
	virtual void service();
};


class ScannerLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	ScannerLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
		const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps = (getNumLEDs() - 1) * 2;
		_Index = 0;
		setPeriod( 4000 / _TotalSteps);
	}

protected:
	virtual void service();
};

class ScannerAnimation : public AnimationBase<SampleProcess, ScannerColorUpdateProcess, ScannerLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, ScannerColorUpdateProcess, ScannerLedUpdateProcess> SUPER;
	
public:
	ScannerAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1) : SUPER(manager, pController)
	{
		_colorSettings._Color1 = color1;
	}

};

#endif //__ScannerANIMATION_H__
