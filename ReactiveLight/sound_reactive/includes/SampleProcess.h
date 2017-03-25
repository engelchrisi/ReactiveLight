#ifndef __SAMPLEPROCESS_H__
#define __SAMPLEPROCESS_H__

#include "SoundUtils.h"

class SampleProcess : public Process, public SoundBase
{
public:
	SampleProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
					ModeStatisticsT& modeStats, SongStatisticsT& songStats);
	
protected:
	virtual void service();
	virtual void cleanup();
	
private:
	ModeStatisticsT&		_modeStats;
	SongStatisticsT&		_songStats;
};
#endif //__SAMPLEPROCESS_H__
