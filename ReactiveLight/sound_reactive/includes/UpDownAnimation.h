
#ifndef __UPDOWNANIMATION_H__
#define __UPDOWNANIMATION_H__

#include "SampleProcess.h"
#include "LedUpdateProcessBase.h"
#include "ColorUpdateProcess.h"
#include "AnimationBase.h"


class UpDownLedUpdateProcess : public LedUpdateProcessBase
{
public:
	UpDownLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
					const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
		: LedUpdateProcessBase(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
	}

protected:
	virtual void service();
};

class UpDownAnimation : public AnimationBase<SampleProcess, ColorUpdateProcess, UpDownLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, ColorUpdateProcess, UpDownLedUpdateProcess> SUPER;
	
public:
	UpDownAnimation(Scheduler &manager, CLEDController* pController) : SUPER(manager, pController)
	{
	}

};

#endif //__UPDOWNANIMATION_H__
