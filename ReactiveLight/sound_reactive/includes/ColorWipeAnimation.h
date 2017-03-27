
#ifndef __COLORWIPEANIMATION_H__
#define __COLORWIPEANIMATION_H__


#include "SampleProcess.h"
#include "LedUpdateProcessBase.h"
#include "ColorUpdateProcess.h"
#include "AnimationBase.h"

class ColorWipeColorUpdateProcess : public ColorUpdateProcess
{
	typedef ColorUpdateProcess SUPER;
public:
	ColorWipeColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
								const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, modeStats, songStats, colorSettings)
	{
	}
	
protected:
	virtual void service();
};


class ColorWipeLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	ColorWipeLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
		const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps = getNumLEDs();
		setPeriod(1000 / _TotalSteps);
		_Direction= DT_FORWARD;
	}

protected:
	virtual void service();
	virtual void Increment();
};

class ColorWipeAnimation : public AnimationBase<SampleProcess, ColorWipeColorUpdateProcess, ColorWipeLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, ColorWipeColorUpdateProcess, ColorWipeLedUpdateProcess> SUPER;
	
public:
	ColorWipeAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1) : SUPER(manager, pController)
	{
		LOGF("=== ColorWipeAnimation ===");
		
		_colorSettings._Color1 = color1;
	}

};

#endif //__COLORWIPEANIMATION_H__
