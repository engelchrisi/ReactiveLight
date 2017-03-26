#ifndef __RAINBOWANIMATION_H__
#define __RAINBOWANIMATION_H__

#include "SampleProcess.h"
#include "LedUpdateProcessBase.h"
#include "ColorUpdateProcess.h"
#include "AnimationBase.h"

class RainbowLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	RainbowLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
							const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps = 255;
		setPeriod(random(5,10));
	}

protected:
	virtual void service();
};

class RainbowAnimation : public AnimationBase<SampleProcess, ColorUpdateProcess, RainbowLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, ColorUpdateProcess, RainbowLedUpdateProcess> SUPER;
	
public:
	RainbowAnimation(Scheduler &manager, CLEDController* pController) : SUPER(manager, pController)
	{
	}

	virtual void Enable()
	{
		SUPER::Enable();
		_colorUpdateProcess.disable(); // not needed
	}
};

#endif //__RAINBOWANIMATION_H__
