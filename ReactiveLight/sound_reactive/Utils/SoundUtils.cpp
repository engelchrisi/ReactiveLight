#include "stdafx.h"
#include "pins.h"

#include "CachedValue.h"
#include "SoundUtils.h"

#define AMP_MAX		(1024 / 2)


//Arduino loop delay
#define DELAY_MS				50

//How long do we keep the "current average" sound, before restarting the measuring
#define AVG_KEEP_CYCLES ((30 * 1000) / DELAY_MS)

#define HIGH_LIMIT	((30 * 5) / DELAY_MS)


SoundBase::SoundBase(ModeStatisticsT& modeStats, SongStatisticsT& songStats)
		: _modeStats(modeStats), _songStats(songStats)

{
}


bool SoundBase::updateSoundStatistics(int log2Samples)
{
	updateRawSoundStatistics(log2Samples);
	
	const int rawSensorValue =  _songStats.rawSoundStats.soundVolRMSflt; // soundVolAvg;

	//If 0, discard immediately. Probably not right and save CPU.
	if (rawSensorValue == 0) {
		LOGF6("#ZERO: soundVolAvg=",  _songStats.rawSoundStats.soundVolAvg, ", soundVolRMSflt=", _songStats.rawSoundStats.soundVolRMSflt,  ", soundVolRMS=", _songStats.rawSoundStats.soundVolRMS);
		return false;
	}

	const int noiceLevel =  _songStats.rawSoundStats.noiceLevel; ;
	const int sensorValue= (rawSensorValue > noiceLevel)? rawSensorValue - noiceLevel : 0;
	
	////Insert new avg. values
	//s_long_avg.add(sensor_value); // TODO comment out
	//const int longAvgValue= s_long_avg.calcAverage();
	
	//Compute the "song average" sensor value
	_songStats.songAvgSum += sensorValue;
	_songStats.songAvgCounter++;
	
	const int lastSongAvg= _songStats.songAvg;
	_songStats.songAvg= float(_songStats.songAvgSum) / float(_songStats.songAvgCounter);
	const int newSongAvg= _songStats.songAvg;
	_songStats.songAvgDirection= (newSongAvg == lastSongAvg)? DT_SAME : ((newSongAvg > lastSongAvg)? DT_UP : DT_DOWN);
	
	if (_songStats.songAvgCounter > AVG_KEEP_CYCLES) 
	{
		_songStats.songAvgSum = _songStats.songAvg;
		_songStats.songAvgCounter = 1;
	}
	

	//Check if we enter SM_HIGH mode
	detectHighMode(sensorValue);

	_songStats.sensorValue= sensorValue;
	
	LOGVAL_SENSOR((_modeStats.songMode == SM_NORMAL)? 0:10, 
				noiceLevel, sensorValue, rawSensorValue, _songStats.rawSoundStats.maxMicLevel, 
				long(_songStats.songAvg));

	return true;
}


// calculate volume level of the signal and print to serial and LCD
void SoundBase::updateRawSoundStatistics(int log2Samples)
{
	long soundVolAvg = 0;
	long soundVolMax = 0;
	long soundVolRMS = 0;
	int k, amp;
	int ampMax= 512;
	const int noOfSamples= 1 << log2Samples;
	
	{
		//cli();  // UDRE interrupt slows this way down on arduino1.0
		for (int i = 0; i < noOfSamples; i++)
		{
			k = analogRead(APIN_MIC_SENSOR);
			amp = abs(k - ampMax);
			amp <<= VolumeGainFactorBits;
			soundVolMax = max(soundVolMax, amp);
			soundVolAvg += amp;
			soundVolRMS += long(amp) * long(amp);
		}
		//sei();
	}

	soundVolAvg >>= log2Samples;
	soundVolRMS >>= log2Samples;
	if (soundVolRMS <= 0)
		LOGF2("!!!!!!!!!!!!!!!!!", soundVolRMS);
	double soundVolRMSflt = sqrt(soundVolRMS);

	//
	// Results
	//
	RawSoundStatisticsT& stats= _songStats.rawSoundStats;
	stats.dB = 20.0*log10(soundVolRMSflt/ampMax);

	// convert from 0 to 100
	ampMax= 100;
	stats.soundVolAvg =		100 * soundVolAvg / ampMax;
	stats.soundVolMax =		100 * soundVolMax / ampMax;
	stats.soundVolRMSflt =	100 * soundVolRMSflt / ampMax;
	stats.soundVolRMS =		10 * soundVolRMSflt / 7; // RMS to estimate peak (RMS is 0.7 of the peak in sin)
	
	stats.maxMicLevel= readMaxMicLevel();
	stats.noiceLevel= readNoiceLevel();
	
	//LOGVAL_SENSOR("0;%lu;%lu;%lu;%lu;%li", soundVolAvg, soundVolMax, (long)soundVolRMSflt, soundVolRMS,(long)dB,0);
}

/**Function to check if the lamp should either enter a SM_HIGH mode,
or revert to SM_NORMAL if already in SM_HIGH. If the sensors report values
that are higher than 1.1 times the average values, and this has happened
more than HIGH_LIMIT times the last few milliseconds, it will enter SM_HIGH mode.
TODO: Not very well written, remove hard coded values, and make it more
reusable and configurable.  */
void SoundBase::detectHighMode(int sensorValue)
{
	const float compareValue= _songStats.songAvg;
	SongModeT songMode= _modeStats.songMode;
	const unsigned long currenMillis= millis();
	const unsigned long elapsedSinceLastHigh= currenMillis - _modeStats.lastMillis;
	//const float compareValue= (float)s_song_avg/(float)s_iter;
	
	if (float(sensorValue) > (float(compareValue)  * 1.1 ))
	{
		//LOGF2("STRIKE: ", s_high.times);
		if (_modeStats.times != 0)
		{
			if (elapsedSinceLastHigh > 200)
			{
				_modeStats.times = 0;
				songMode = SM_NORMAL;
				if (_DEBUG)
				{
					LOGF("#NORMAL mode 1");
				}
			}
			else
			{
				_modeStats.lastMillis = currenMillis;
				_modeStats.times++;
			}
		}
		else
		{
			_modeStats.times++;
			_modeStats.lastMillis = currenMillis;

		}
	}
	
	if (_modeStats.times > 0 && elapsedSinceLastHigh > 200) // longer break than 200ms - how can this happen?
	{
		//LOGF2("UNSTRIKE=", elapsedSinceLastHigh);

		_modeStats.times = 0;
		if (songMode != SM_NORMAL)
		{
			songMode = SM_NORMAL;
			if (_DEBUG)
			{
				LOGF("#NORMAL mode");
			}
		}
	}
	else if (_modeStats.times > HIGH_LIMIT) /// && elapsedSinceLastHigh < 50)
	{
		if (songMode != SM_HIGH)
		{
			songMode = SM_HIGH;
			if (_DEBUG)
			{
				LOGF("#HIGH mode");
			}
		}
	}
	
	_modeStats.songMode= songMode;
}


#define MAX_POTI	800


uint16_t readNoiceLevelFunc() {
	#ifdef APIN_POTI_NOICE_LEVEL
	uint16_t v= analogRead(APIN_POTI_NOICE_LEVEL);

	if (_DEBUG) {
		LOGF2("#readNoiceLevel V=", v);
	}

	return v;
	#else
	return MAX_POTI >> 1;
	#endif
}


int SoundBase::readNoiceLevel() 
{
	static CachedValue<uint16_t, 1000/*ms*/>	s_NoiceLevelValue(readNoiceLevelFunc);
	uint16_t v= s_NoiceLevelValue.readValue();
	
	int level= map(v, 0, MAX_POTI, 0, NOICE_MAX);
	return level;
}


uint16_t readMaxMicLevelFunc() {
	#ifdef APIN_POTI_MAX_MIC_LEVEL
	uint16_t v= analogRead(APIN_POTI_MAX_MIC_LEVEL);
	
	if (_DEBUG) {
		LOGF2("#readMaxMicLevel V=", v);
	}
	
	return v;
	#else
	return MAX_POTI >> 1;
	#endif
}


int SoundBase::readMaxMicLevel() 
{
	static CachedValue<uint16_t, 1000/*ms*/>	s_MaxMicLevelSensorValue(readMaxMicLevelFunc);
	uint16_t v= s_MaxMicLevelSensorValue.readValue();
	
	int level= map(v, 0, MAX_POTI, 0, MAX_MIC_LEVEL);
	
	return level;
}


#ifdef ADCFlow

// http://maxembedded.com/2011/06/the-adc-of-the-avr/

// initialize adc
void adc_init()
{
	#if 1
	//// ADC Enable and prescaler of 128
	//// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	#else
	// http://blog.yavilevich.com/2016/08/arduino-sound-level-meter-and-spectrum-analyzer/
	// set the adc to free running mode
	// register explanation: http://maxembedded.com/2011/06/the-adc-of-the-avr/
	// 5 => div 32. sample rate 38.4
	// 7 => switch to divider=128, default 9.6khz sampling
	ADCSRA = 0xe0+7; // "ADC Enable", "ADC Start Conversion", "ADC Auto Trigger Enable" and divider.
	#endif

	ADMUX = 0x0; // use adc0 (hardcoded, doesn't use MicPin). Use ARef pin for analog reference (same as analogReference(EXTERNAL)).
	#ifndef EXTERNAL_VOL_REF
	ADMUX |= (1<<REFS0); // AREF = AVcc
	//ADMUX |= 0x40; // Use Vcc for analog reference.
	#endif
	DIDR0 = 0x01; // turn off the digital input for adc0
}

// read adc value
uint16_t adc_read(uint8_t analogPin)
{
	uint8_t analogPinNo = analogPin - PIN_A0;
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'analogPinNo' between 0 and 7
	analogPinNo &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|analogPinNo;     // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

#endif //  ADCFlow

