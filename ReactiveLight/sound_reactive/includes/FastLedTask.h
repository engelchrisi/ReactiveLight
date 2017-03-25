#ifndef __NeoPatterns_H_
#define __NeoPatterns_H_

#include "SoundUtils.h"






class FastLedTask : public Process, public FastLedTaskBase, public SoundBase
{
private:
	unsigned long mRuntimeMs;
		
public:
	// Pattern types supported:
	enum  PatternT { PT_NONE, PT_RAINBOW_CYCLE, PT_THEATER_CHASE, PT_COLOR_WIPE, PT_SCANNER, PT_FADE };
	// Pattern directions supported:
	enum  DirectionT { DT_FORWARD, DT_REVERSE };

protected:
	// Member Variables:
	PatternT  _ActivePattern;  // which pattern is running
	DirectionT _Direction;     // direction to run the pattern
	
	CRGB	_Color1, _Color2;  // What colors are in use
	uint16_t _TotalSteps;  // total number of steps in the pattern
	uint16_t _Index;  // current step within the pattern
	
public:
	DEFINE_TASK(FastLedTask);

	// Constructor - calls base-class constructor to initialize strip
	FastLedTask(CLEDController& controller,
					unsigned long liftimeMs,
					PatternT  pattern0, uint32_t color1, uint32_t color2, DirectionT dir);
				 
	virtual ~FastLedTask();
	
	void InitTheaterChase(uint32_t color1, uint32_t color2, DirectionT dir);
	void InitRainbowCycle(DirectionT dir);
	void InitColorWipe(uint32_t color, DirectionT dir);
	void InitFade(uint32_t color1, uint32_t color2, uint16_t steps, DirectionT dir);
	void InitScanner(uint32_t color1, DirectionT dir);

	void Reverse();
	
public:
	static uint32_t Wheel(byte WheelPos, SongModeT songMode);

protected:
	virtual void execute();

	void Increment();

	void TheaterChaseUpdate();
	void RainbowCycleUpdate();
	void ColorWipeUpdate();
	void ScannerUpdate();
	void FadeUpdate();
	
	void ColorSet(uint32_t color);
};

#endif //__NeoPatterns_H_