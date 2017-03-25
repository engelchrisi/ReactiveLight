#ifndef SOUNDUTILS_H_
#define SOUNDUTILS_H_

#ifdef ADCFlow

#define analogRead	adc_read

#endif

#define SENSOR_SEED         0
#define ADAFRUIT_MAX9814    0
#define ADAFRUIT_MAX4466    1

#if SENSOR_SEED
	#define VolumeGainFactorBits 	0
	#define NOICE_MAX 				(40 << VolumeGainFactorBits)
	#define MAX_MIC_LEVEL			(250 << VolumeGainFactorBits)
#elif ADAFRUIT_MAX9814
	#define NOICE_MAX 				(260+30)
	#define MAX_MIC_LEVEL				(550)
#elif ADAFRUIT_MAX4466
	#define VolumeGainFactorBits 	1
	#define NOICE_MAX 				130
	#define MAX_MIC_LEVEL 			500
#else
	#define NOICE_MAX 			(1)  // Noise/hum/interference in mic signal
	#define MAX_MIC_LEVEL     (1024)
#endif

//Showing different colors based on the mode.
enum SongModeT {
	SM_NONE= 0,
	SM_NORMAL= 2,
	SM_HIGH=3
};


//Keeping track how often, and how long times we hit a certain mode
struct ModeStatisticsT 
{
	SongModeT		songMode;
	unsigned long	lastMillis;
	short			times;
	
	ModeStatisticsT() {
		songMode= SM_NONE;
		times= 0;
		lastMillis= 0;
	}
};

struct RawSoundStatisticsT {
	int soundVolMax;
	int soundVolAvg;
	
	long soundVolRMS;
	float soundVolRMSflt;
	
	float dB;
	
	int maxMicLevel;
	int noiceLevel;
	
	RawSoundStatisticsT() {
		soundVolMax= soundVolAvg= 0;
		soundVolRMS= 0;
		soundVolRMSflt= dB= 0;
		maxMicLevel= noiceLevel= 0;
	}

};

enum DirecT {
	DT_SAME=0,
	DT_UP,	 // >=
	DT_DOWN, // <
};

struct SongStatisticsT {
	RawSoundStatisticsT	rawSoundStats;
	
	//Average sound measurement the last CYCLES
	unsigned long songAvgSum;
	//The amount of iterations since songAvgSum was reset
	unsigned long songAvgCounter;
	// songAvgSum / songAvgIter
	float songAvg; 
	DirecT	songAvgDirection;
	
	int sensorValue;
	
	SongStatisticsT() {
		songAvgSum= 0;
		songAvgCounter= 0;
		songAvg= 0;
		sensorValue= 0;
	}


};


class SoundBase
{
public:
	SoundBase(ModeStatisticsT& modeStats, SongStatisticsT& songStats);

protected:
	bool updateSoundStatistics(int log2Samples);

private:
	void updateRawSoundStatistics(int log2Samples);
	void detectHighMode(int sensorValue);
	int readNoiceLevel();
	int readMaxMicLevel();

protected:
	ModeStatisticsT&	_modeStats;
	SongStatisticsT&	_songStats;

};

extern void adc_init();
extern uint16_t adc_read(uint8_t analogPin);

#endif /* SOUNDUTILS_H_ */