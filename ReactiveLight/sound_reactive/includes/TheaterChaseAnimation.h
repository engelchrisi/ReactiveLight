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

class TheaterChaseLedUpdateProcess : public LedUpdateProcessBase
{
	typedef LedUpdateProcessBase SUPER;
	
public:
	enum  DirectionT { DT_FORWARD, DT_REVERSE };

public:
	TheaterChaseLedUpdateProcess(Scheduler &manager, ProcPriority pr, uint32_t period, CLEDController* pController,
					const ModeStatisticsT& modeStats, const SongStatisticsT& songStats, const AnimationColorSettingsT& colorSettings)
		: LedUpdateProcessBase(manager, pr, period, pController, modeStats, songStats, colorSettings)
	{
		_TotalSteps= getNumLEDs(); 
		_Index = 0;
		_Direction = DT_REVERSE;
	}

protected:
	virtual void service();
	
	void Increment();
	
private:
	DirectionT _Direction;     // direction to run the pattern
	
	uint16_t _TotalSteps;  // total number of steps in the pattern
	uint16_t _Index;  // current step within the pattern
};

class TheaterChaseAnimation : public AnimationBase<SampleProcess, TheaterChaseColorUpdateProcess, TheaterChaseLedUpdateProcess>
{
	typedef AnimationBase<SampleProcess, TheaterChaseColorUpdateProcess, TheaterChaseLedUpdateProcess> SUPER;
public:
	TheaterChaseAnimation(Scheduler &manager, CLEDController* pController, uint32_t color1, uint32_t color2);
};

#endif //__THEATERCHASEANIMATION_H__
