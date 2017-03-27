
#ifndef __FadeANIMATION_H__
#define __FadeANIMATION_H__


#include "SampleProcess.h"
#include "LedUpdateProcessBase.h"
#include "ColorUpdateProcess.h"
#include "AnimationBase.h"

class FadeLedUpdateProcess : public LedUpdateProcessBaseEx
{
	typedef LedUpdateProcessBaseEx SUPER;
	
public:
	FadeLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
		const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, AnimationColorSettingsT& colorSettings)
		: SUPER(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps = 20;
		_Index = 0;
		_Direction= DT_FORWARD;
		
		setPeriod( 2000 / _TotalSteps);
	}

protected:
	virtual void service();
	virtual void Increment();
};

class FadeAnimation : public AnimationBase<SampleProcess, ColorUpdateProcess, FadeLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, ColorUpdateProcess, FadeLedUpdateProcess> SUPER;
	
public:
	FadeAnimation(Scheduler &manager, CLEDController* pController) : SUPER(manager, pController)
	{
		LOGF("=== FadeAnimation ===");
		_colorSettings._Color1 = Wheel(random(255), SM_NORMAL);
		_colorSettings._Color2 =  Wheel(random(255), SM_NORMAL);
	}

};

#endif //__FadeANIMATION_H__
