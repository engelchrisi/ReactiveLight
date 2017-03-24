#ifndef __DISCOLIGHTTASK_H__
#define __DISCOLIGHTTASK_H__

#include "taskQueue.h"
#include "FastLedTask.h"

class DiscoLightTask : public SoundBase, public RecurringTask, public FastLedTaskBase
{
public:
	DiscoLightTask(CLEDController& controller, unsigned long liftimeMs);
	
protected:
	virtual void execute();
	
private:
	void updateLEDs(float fade_scale, RgbColorT colorOffset);

private:

};

#endif //__DISCOLIGHTTASK_H__
