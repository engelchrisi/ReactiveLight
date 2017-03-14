/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
/*End of auto generated code by Atmel studio */

#include <avr/io.h> // _BV
#include "DipSwitch.h"
#include "pins.h"
#include "CachedValue.h"
#include "Logging.h"

#define AmpMax		(1024 / 2)
#define MicSamples	(1024*2) // Three of these time-weightings have been internationally standardized, 'S' (1 s) originally called Slow, 'F' (125 ms) originally called Fast and 'I' (35 ms) originally called Impulse.

// modes
#define Use3.3		// use 3.3 voltage. the 5v voltage from usb is not regulated, this is much more stable.
#define ADCReClock	// switch to higher clock, not needed if we are OK with freq between 0 and 4Khz.
#define ADCFlow		// read data from adc with free-run (not interrupt). much better data, dc low. hard coded for A0.

// macros
// http://yaab-arduino.blogspot.co.il/2015/02/fast-sampling-from-analog-input.html
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

DipSwitch	s_DipSwitch(DPIN_DIP_0, DPIN_DIP_2);

#include <FastLED.h>

#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))


static char bufSerial[256]= "";

#if LOGGING

	#if LOG_TIMESTAMP
		#define LOGVAL_SENSOR(format, songModeCh, sensor_value, mappedSensorVal, avgSensorValue, longAvgValue, songAvg) \
		{ \
			sprintf(bufSerial, "%i:" format, millis(), songModeCh, sensor_value, mappedSensorVal, avgSensorValue, longAvgValue, songAvg); \
			Serial.println(bufSerial); \
		}
	#else
		#define LOGVAL_SENSOR(format, songModeCh, sensor_value, mappedSensorVal, avgSensorValue, longAvgValue, songAvg) \
		{ \
			sprintf(bufSerial, format, songModeCh, sensor_value, mappedSensorVal, avgSensorValue, longAvgValue, songAvg); \
			Serial.println(bufSerial); \
		}
	#endif
#else
		#define LOGVAL_SENSOR(format, songModeCh, sensor_value, mappedSensorVal, avgSensorValue, longAvgValue, songAvg) 
#endif

#define ADAFRUIT_MAX4466    1
#define ADAFRUIT_MAX9814    0
#define SENSOR_SEED         0

#if SENSOR_SEED
	static const int NOICE_MAX= 				80+30;
	static const int MAX_MIC_LEVEL=				500;
#elif ADAFRUIT_MAX9814
	static const int NOICE_MAX= 				260+30;
	static const int MAX_MIC_LEVEL=				550;
#elif ADAFRUIT_MAX4466
	static const int VolumeGainFactorBits= 		3;
	static const int NOICE_MAX= 				14 << VolumeGainFactorBits;
	static const int MAX_MIC_LEVEL= 			40 << VolumeGainFactorBits;
#else
	static const int NOICE_MAX= 			1;  // Noise/hum/interference in mic signal
	static const int MAX_MIC_LEVEL=     1024;
#endif

//The amount of LEDs in the setup
static int NUM_LEDS= 0;


//Arduino loop delay
#define DELAY_MS				50	

//How long do we keep the "current average" sound, before restarting the measuring
#define AVG_KEEP_CYCLES ((30 * 1000) / DELAY_MS)

#define HIGH_LIMIT	((30 * 5) / DELAY_MS)

/*Sometimes readings are wrong or strange. How much is a reading allowed
to deviate from the average to not be discarded? **/
#define DEV_THRESH 0.8


float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);
void visualize_music();

enum SongModeT {
	SM_NONE= 0,
	SM_NORMAL= 2,
	SM_HIGH=3
};


//Showing different colors based on the mode.
SongModeT s_songMode = SM_NORMAL;

//Average sound measurement the last CYCLES
unsigned long s_song_avg=0;

//The amount of iterations since the song_avg was reset
unsigned long s_iter = 0;

//Led array
static CRGB *s_leds= NULL;

template<class T, size_t N, T DEFAULT_VALUE=0> class FifoQueue 
{
	private:
		int _offset;
		int _counter;
	public:
		T _array[N];
		
	FifoQueue()
	{
		_counter= 0;
		for (int i= 0; i < N; ++i)
		{
			_array[i]= DEFAULT_VALUE;
		}
	}
	
	void add(T value)
	{
		_array[_offset]= value;
		++_offset;
		_offset %= N; // wrap around at eof array
		
		++_counter;
		_counter= min(N, _counter);
	}
	
	T calcAverage() {
		unsigned long sum = 0;
		
		for (int i= 0; i < _counter; ++i)
		{
			sum += _array[i];
		}
		
		return (T)(sum / N);
	}
};

////How many previous sensor values decides if we are on a peak/HIGH (e.g. in a song)
//#define LONG_SECTOR 20
////Longer sound avg
//FifoQueue<int, LONG_SECTOR> s_long_avg;


//Keeping track how often, and how long times we hit a certain mode
struct TimeKeepingT {
	unsigned long lastMillis;
	short times;
};

//How much to increment or decrement each color every cycle
struct RgbColorT {
	int r;
	int g;
	int b;
};

struct TimeKeepingT s_high;
struct RgbColorT s_ColorOffset;

static bool _DEBUG= false;

template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2813_GRB : public WS2813Controller<DATA_PIN, GRB> {};
	
int readDipSwitchFunc() {
#if true
	return s_DipSwitch.getValue();
#else
	return 1;
#endif
}

static CachedValue<int, 10*1000/*ms*/>	s_DipSwitchValue(readDipSwitchFunc);

#ifdef ADCFlow

// http://maxembedded.com/2011/06/the-adc-of-the-avr/

#define analogRead	adc_read

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
#ifndef Use3.3
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


void setup() {
	Serial.begin(38400); // );115200
	while (!Serial);
	while (Serial.available()) {
		Serial.read();
	}
		
	const int nDipValue= s_DipSwitchValue.readValue();
	LOGF2("#DIP Value=", nDipValue);
		
	switch(nDipValue)
	{
		case 0: 	NUM_LEDS= 5; _DEBUG= true; break;
		case 1: 	NUM_LEDS= 50; _DEBUG= true; break;
		case 2: 	NUM_LEDS= 100; _DEBUG= false; break;
		case 3: 	NUM_LEDS= 150; _DEBUG= false; break;
		default:
		NUM_LEDS= 220;
		_DEBUG= false;
		break;
	}
	
#ifdef ADCFlow
	adc_init();
#else
	#ifdef Use3.3
		analogReference(EXTERNAL); // 3.3V to AREF
	#endif
#endif


#ifdef ADCReClock // change ADC freq divider. default is div 128 9.6khz (bits 111)
	// http://yaab-arduino.blogspot.co.il/2015/02/fast-sampling-from-analog-input.html
	// 1 0 0 = mode 4 = divider 16 = 76.8khz
	//sbi(ADCSRA, ADPS2);
	//cbi(ADCSRA, ADPS1);
	//cbi(ADCSRA, ADPS0);
	// 1 0 1 = mode 5 = divider 32 = 38.4Khz
	sbi(ADCSRA, ADPS2);
	cbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
#endif
		
	s_leds= new CRGB[NUM_LEDS];
	
	//Set all lights to make sure all are working as expected
	//FastLED.addLeds<NEOPIXEL, DPIN_LED>(s_leds, NUM_LEDS);
	FastLED.addLeds<WS2813_GRB, DPIN_LED>(s_leds, NUM_LEDS);
	
	CRGB	initValue= CRGB(0, 0, 255);

	for (int i = 0; i < NUM_LEDS; i++)
	{
		s_leds[i] = initValue;
	}
	
	FastLED.show();
		
	if (_DEBUG)
	{
		delay(1000);
	}
	else 
	{
		delay(1000);
	}

	//Initial values
	s_high.times = 0;
	s_high.lastMillis = millis();
	s_ColorOffset.r = 0;
	s_ColorOffset.g = 0;
	s_ColorOffset.b = 1;
}

long readRawSensorValue();

/*With this we can change the mode if we want to implement a general
lamp feature, with for instance general pulsing. Maybe if the
sound is low for a while? */
void loop() {
	visualize_music();
	//delay(DELAY_MS);       // delay in between reads for stability
}


/**Function to check if the lamp should either enter a SM_HIGH mode,
or revert to SM_NORMAL if already in SM_HIGH. If the sensors report values
that are higher than 1.1 times the average values, and this has happened
more than HIGH_LIMIT times the last few milliseconds, it will enter SM_HIGH mode.
TODO: Not very well written, remove hard coded values, and make it more
reusable and configurable.  */
void detectHighMode(int sensorValue, float compareValue)
{
	const unsigned long currenMillis= millis();
	const unsigned long elapsedSinceLastHigh= currenMillis - s_high.lastMillis;
	//const float compareValue= (float)s_song_avg/(float)s_iter;
	
	if (float(sensorValue) > (float(compareValue)  * 1.1 ))
	{
		//LOGF2("STRIKE: ", s_high.times);
		if (s_high.times != 0)
		{
			if (elapsedSinceLastHigh > 200)
			{
				s_high.times = 0;
				s_songMode = SM_NORMAL;
				if (_DEBUG)
				{
					LOGF("#NORMAL mode 1");
				}
			}
			else
			{
				s_high.lastMillis = currenMillis;
				s_high.times++;
			}
		}
		else
		{
			s_high.times++;
			s_high.lastMillis = currenMillis;

		}
	}
	
	if (s_high.times > 0 && elapsedSinceLastHigh > 200) // longer break than 200ms - how can this happen?
	{
		//LOGF2("UNSTRIKE=", elapsedSinceLastHigh);

		s_high.times = 0;
		if (s_songMode != SM_NORMAL)
		{
			s_songMode = SM_NORMAL;
			if (_DEBUG)
			{
				LOGF("#NORMAL mode");
			}
		}
	}
	else if (s_high.times > HIGH_LIMIT) /// && elapsedSinceLastHigh < 50) 
	{
		if (s_songMode != SM_HIGH)
		{
			s_songMode = SM_HIGH;
			if (_DEBUG)
			{
				LOGF("#HIGH mode");
			}
		}
	}

}

void addOffset(uint8_t& color, int colOffset)
{
	if ((int)color + colOffset > 255)
		color = 255;
	else if ((int)color + colOffset < 0)
		color = 0;
	else
		color = color + colOffset;
}

void addOffset(CRGB& rgb, const RgbColorT& offset)
{
	addOffset(rgb.r, offset.r);
	addOffset(rgb.g, offset.g);
	addOffset(rgb.b, offset.b);
}

void fade(CRGB& rgb, float fade_scale)
{
	rgb.r = uint8_t(float(rgb.r) / fade_scale);
	rgb.g = uint8_t(float(rgb.g) / fade_scale);
	rgb.b = uint8_t(float(rgb.b) / fade_scale);
}

#define MAX_POTI	800


int readNoiceLevelFunc() {
#ifdef APIN_POTI_NOICE_LEVEL
	int v= analogRead(APIN_POTI_NOICE_LEVEL);

	if (_DEBUG) {
		LOGF2("#readNoiceLevel V=", v);
	}

	return v;
#else
	return MAX_POTI;
#endif
}


int readNoiceLevel() {
	static CachedValue<int, 1000/*ms*/>	s_NoiceLevelValue(readNoiceLevelFunc);
	int v= s_NoiceLevelValue.readValue();
	
	int level= map(v, 0, MAX_POTI, 0, NOICE_MAX);
	return level;
}


int readMaxMicLevelFunc() {
#ifdef APIN_POTI_MAX_MIC_LEVEL
	int v= analogRead(APIN_POTI_MAX_MIC_LEVEL);
	
	if (_DEBUG) {
		LOGF2("#readMaxMicLevel V=", v);
	}
	
	return v;
#else
	return MAX_POTI;
#endif
}


int readMaxMicLevel() {
	static CachedValue<int, 1000/*ms*/>	s_MaxMicLevelSensorValue(readMaxMicLevelFunc);
	int v= s_MaxMicLevelSensorValue.readValue();
	
	int level= map(v, 0, MAX_POTI, 0, MAX_MIC_LEVEL);
	
	return level;
}



// calculate volume level of the signal and print to serial and LCD
long readRawSensorValue()
{
	long soundVolAvg = 0;
	//long soundVolMax = 0;
	long soundVolRMS = 0;
	//cli();  // UDRE interrupt slows this way down on arduino1.0
	for (int i = 0; i < MicSamples; i++)
	{
		int k = analogRead(APIN_MIC_SENSOR);
		int amp = abs(k - AmpMax);
		amp <<= VolumeGainFactorBits;
		//soundVolMax = max(soundVolMax, amp);
		soundVolAvg += amp;
		soundVolRMS += ((long)amp*amp);
	}
	
	soundVolAvg /= MicSamples;
	soundVolRMS /= MicSamples;
	float soundVolRMSflt = sqrt(soundVolRMS);
	//sei();

	float dB = 20.0*log10(soundVolRMSflt/AmpMax);

	// convert from 0 to 100
	soundVolAvg = 100 * soundVolAvg / AmpMax;
	//soundVolMax = 100 * soundVolMax / AmpMax;
	soundVolRMSflt = 100 * soundVolRMSflt / AmpMax;
	soundVolRMS = 10 * soundVolRMSflt / 7; // RMS to estimate peak (RMS is 0.7 of the peak in sin)
	
	//LOGVAL_SENSOR("0;%lu;%lu;%lu;%lu;%li", soundVolAvg, soundVolMax, (long)soundVolRMSflt, soundVolRMS,(long)dB,0);

	return soundVolRMSflt; // soundVolAvg;
}


//Main function for visualizing the sounds in the lamp
void visualize_music()
{
	//Actual sensor value
	const int rawSensorValue = readRawSensorValue();

	//If 0, discard immediately. Probably not right and save CPU.
	if (rawSensorValue == 0) {
		return;
	}

	const int noiceLevel= readNoiceLevel();
	int sensorValue= (rawSensorValue > noiceLevel)? rawSensorValue - noiceLevel : 0;
	
	////Insert new avg. values
	//s_long_avg.add(sensor_value); // TODO comment out
	//const int longAvgValue= s_long_avg.calcAverage();
	
	//Compute the "song average" sensor value
	s_song_avg += sensorValue; 
	s_iter++;
	
	const float quotient= float(s_song_avg) / float(s_iter);
	if (s_iter > AVG_KEEP_CYCLES) {
		s_song_avg = quotient;
		s_iter = 1;
	}

	//Check if we enter SM_HIGH mode
	detectHighMode(sensorValue, quotient);
	
	const int rawMaxMicLevel= readMaxMicLevel(); 
	LOGVAL_SENSOR("%i;%i;%i;%i;%i;%lu", (s_songMode == SM_NORMAL)? 0:10, noiceLevel, sensorValue, rawSensorValue, rawMaxMicLevel, (unsigned long)quotient);
	//LOGVAL_SENSOR("#%lu;%lu", s_song_avg, s_iter, 0,0,0,0);

	//The speed the LEDs fade to black if not relit
	float fade_scale = 1.2;

	if (s_songMode == SM_NORMAL) {
		fade_scale = 2.0;
		s_ColorOffset.r = -1;
		s_ColorOffset.g = 1;
		s_ColorOffset.b = 2;
	}
	else if (s_songMode == SM_HIGH) {
		fade_scale = 3.0;
		s_ColorOffset.r =  5;
		s_ColorOffset.g =  3;
		s_ColorOffset.b = -1;
	}

	//Decides how many of the LEDs will be lit
	const int show_amount = map(sensorValue, 0, rawMaxMicLevel, 0, NUM_LEDS); //ceil(fscale(0.0, rawMaxMicLevel, 0.0, (float)NUM_LEDS, (float)sensorValue, -1));
	
	if (_DEBUG)
	{
		//LOGF2("#led=", show_amount);
	}

	/*Set the different leds. Control for too high and too low values.
	Fun thing to try: Dont account for overflow in one direction,
	some interesting light effects appear! */
	for (int i = 0; i < show_amount; i++)
	{
		addOffset(s_leds[i], s_ColorOffset);
	}
	
	//All the other LEDs begin their fading journey to eventual total darkness
	for (int i = show_amount; i < NUM_LEDS; i++)
	{
		fade(s_leds[i], fade_scale);
	}

	FastLED.show();
}

//Function imported from the arduino website.
//Basically map, but with a curve on the scale (can be non-uniform).
float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{
	float OriginalRange = 0;
	float NewRange = 0;
	float zeroRefCurVal = 0;
	float normalizedCurVal = 0;
	float rangedValue = 0;
	boolean invFlag = 0;

	// condition curve parameter
	// limit range

	if (curve > 10) curve = 10;
	if (curve < -10) curve = -10;

	curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - positive numbers give more weight to high end on output
	curve = pow(10, curve); // convert linear scale into logarithmic exponent for other pow function

	// Check for out of range inputValues
	if (inputValue < originalMin) {
		inputValue = originalMin;
	}
	if (inputValue > originalMax) {
		inputValue = originalMax;
	}

	// Zero Reference the values
	OriginalRange = originalMax - originalMin;

	if (newEnd > newBegin){
		NewRange = newEnd - newBegin;
	}
	else
	{
		NewRange = newBegin - newEnd;
		invFlag = 1;
	}

	zeroRefCurVal = inputValue - originalMin;
	normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

	// Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
	if (originalMin > originalMax ) {
		return 0;
	}

	if (invFlag == 0){
		rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

	}
	else     // invert the ranges
	{
		rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
	}

	return rangedValue;
}


