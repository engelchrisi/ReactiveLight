
#ifndef __ANIMATIONBASE_H__
#define __ANIMATIONBASE_H__

class IAnimation
{
public:
	virtual ~IAnimation() {}
		
	virtual void Enable()= 0;
	virtual void Disable()= 0;
};

template <class SampleProcessType, class ColorUpdateProcessType, class LedUpdateProcessType>
class AnimationBase : public IAnimation
{
	
public:
	AnimationBase(Scheduler &manager, CLEDController* pController)
		: _sampleProcess(manager, HIGH_PRIORITY, 100, _modeStats, _songStats),
		_colorUpdateProcess(manager, MEDIUM_PRIORITY, 50, _modeStats, _songStats, _colorSettings),
		_ledUpdateProcess(manager, LOW_PRIORITY, 100, pController, _modeStats, _songStats, _colorSettings)
	{
		_sampleProcess.add();
		_colorUpdateProcess.add();
		_ledUpdateProcess.add();
	}
	
	virtual ~AnimationBase()
	{
		// remove from scheduler
		_sampleProcess.destroy();
		_colorUpdateProcess.destroy();
		_ledUpdateProcess.destroy();	
	}

	virtual void Enable()
	{
		_sampleProcess.enable();
		_colorUpdateProcess.enable();
		_ledUpdateProcess.enable();
	}

	virtual void Disable()
	{
		_sampleProcess.disable();
		_colorUpdateProcess.disable();
		_ledUpdateProcess.disable();
	}

protected:
	SampleProcessType		_sampleProcess;
	ColorUpdateProcessType	_colorUpdateProcess;
	LedUpdateProcessType	_ledUpdateProcess;
	
protected:
	ModeStatisticsT				_modeStats;
	SongStatisticsT				_songStats;
	AnimationColorSettingsT		_colorSettings;
}; 

#endif //__ANIMATIONBASE_H__
