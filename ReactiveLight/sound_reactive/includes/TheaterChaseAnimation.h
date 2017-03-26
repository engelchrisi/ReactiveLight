#include "SampleProcess.h"
#include "ColorUpdateProcess.h"
#include "LedUpdateProcessBase.h"
#include "AnimationBase.h"

#ifndef __THEATERCHASEANIMATION_H__
#define __THEATERCHASEANIMATION_H__

#include "AnimationBase.h"

class TheaterChaseColorUpdateProcess : public ColorUpdateProcess
{
	typedef ColorUpdateProcess SUPER;
public:
	TheaterChaseColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
		const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: ColorUpdateProcess(manager, pr, period, modeStats, songStats, colorSettings)
		{
		}
	
protected:
	virtual void service();
};

class TheaterChaseLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	TheaterChaseLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
					const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
	}

protected:
	virtual void service();
	
};

class TheaterChaseAnimation : public AnimationBase<SampleProcess, TheaterChaseColorUpdateProcess, TheaterChaseLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, TheaterChaseColorUpdateProcess, TheaterChaseLedUpdateProcess> SUPER;
public:
	TheaterChaseAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1, uint32_t color2);
};

#endif //__THEATERCHASEANIMATION_H__
