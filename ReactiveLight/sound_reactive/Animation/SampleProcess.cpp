#include "stdafx.h"

#include "SampleProcess.h"

SampleProcess::SampleProcess(Scheduler &manager, ProcPriority pr, uint32_t period,
							ModeStatisticsT& modeStats, SongStatisticsT& songStats)
	: Process(manager, pr, period), _modeStats(modeStats), _songStats(songStats),
		SoundBase(modeStats, songStats)
{
}
	
/*virtual*/ void SampleProcess::service()
{
	//LOGF("SampleProcess");
		
	// Three of these time-weightings have been internationally standardized, 'S' (1 s) originally called Slow, 'F' (125 ms) originally called Fast and 'I' (35 ms) originally called Impulse.
	#define LOG2_SAMPLES	10 //12

	//LOGF("#BEFORE");
	updateSoundStatistics(LOG2_SAMPLES);
	//LOGF("#AFTER");
		
	//LOGF("EOF SampleProcess");
}

/*virtual*/ void SampleProcess::cleanup()
{
	Process::cleanup();
}