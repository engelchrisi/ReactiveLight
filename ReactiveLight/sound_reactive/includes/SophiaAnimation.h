#include "SampleProcess.h"
#include "ColorUpdateProcess.h"
#include "LedUpdateProcessBase.h"
#include "AnimationBase.h"

#ifndef __SophiaANIMATION_H__
#define __SophiaANIMATION_H__

#include "AnimationBase.h"

class SophiaColorUpdateProcess : public ColorUpdateProcess
{
	typedef ColorUpdateProcess SUPER;
public:
	SophiaColorUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
		const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: ColorUpdateProcess(manager, pr, period, modeStats, songStats, colorSettings)
		{
		}
	
protected:
	virtual void service();
};

class SophiaLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	SophiaLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
					const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		//setIterations(RUNTIME_ONCE);
		setPeriod(2*period);
	}

protected:
	virtual void service();
	virtual void Increment();
	
};

class SophiaAnimation : public AnimationBase<SampleProcess, SophiaColorUpdateProcess, SophiaLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, SophiaColorUpdateProcess, SophiaLedUpdateProcess> SUPER;
public:
	SophiaAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1, uint32_t color2);
};

#endif //__SophiaANIMATION_H__
