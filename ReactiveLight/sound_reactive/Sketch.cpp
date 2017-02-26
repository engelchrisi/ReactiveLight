/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include "DipSwitch.h"
#include "pins.h"
#include "CachedValue.h"

DipSwitch	s_DipSwitch(DPIN_DIP_0, DPIN_DIP_2);

#include <FastLED.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void detectHighMode();
//End of Auto generated function prototypes by Atmel Studio

#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))


#define ADAFRUIT_MAX4466    1
#define ADAFRUIT_MAX9814    0
#define SENSOR_SEED         0

#if SENSOR_SEED
	#define NOICE_MAX			30
	#define DC_OFFSET			80
	#define MAX_MIC_LEVEL    (500)
#elif ADAFRUIT_MAX9814
	#define NOICE_MAX			30
	#define DC_OFFSET			260
	#define MAX_MIC_LEVEL    (550)
#elif ADAFRUIT_MAX4466
	#define NOICE_MAX			50 // 18
	#define DC_OFFSET			360
	#define MAX_MIC_LEVEL    (740) 
#else
	#define NOICE_MAX			0  // Noise/hum/interference in mic signal
	#define DC_OFFSET			0  // DC offset in mic signal
	#define MAX_MIC_LEVEL    1024
#endif

//The amount of LEDs in the setup
static int NUM_LEDS= 0;


//Arduino loop delay
#define DELAY_MS 5

//How long do we keep the "current average" sound, before restarting the measuring
#define AVG_KEEP_CYCLES ((30 * 1000) / DELAY_MS)

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
SongModeT songmode = SM_NORMAL;

//Average sound measurement the last CYCLES
unsigned long s_song_avg=0;

//The amount of iterations since the song_avg was reset
int s_iter = 0;

//Led array
static CRGB *s_leds= NULL;

template<class T, size_t N, T DEFAULT_VALUE> class FifoQueue 
{
	private:
		int offset;
	public:
		T array[N];
		
	FifoQueue()
	{
		for (int i= 0; i < N; ++i)
		{
			array[i]= DEFAULT_VALUE;
		}
	}
	
	void add(T value)
	{
		array[offset]= value;
		++offset;
		offset %= N; // wrap around at eof array
	}
	
	T calcAverage() {
		unsigned long sum = 0;
		
		for (int i= 0; i < N; ++i)
		{
			sum += array[i];
		}
		
		return (int)(sum / N);
	}
};

//How many previous sensor values effects the operating average?
#define AVGLEN		5
/*Short sound avg used to "normalize" the input values.
We use the short average instead of using the sensor input directly */
FifoQueue<int, AVGLEN, 250> s_avgsMapped;

//How many previous sensor values decides if we are on a peak/HIGH (e.g. in a song)
#define LONG_SECTOR 20
//Longer sound avg
FifoQueue<int, LONG_SECTOR, -1> s_long_avg;


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

struct TimeKeepingT high;
struct RgbColorT s_ColorOffset;

int s_DipValue= -1;

#define _DEBUG (s_DipValue <= 1) 

template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2813_GRB : public WS2813Controller<DATA_PIN, GRB> {};

void setup() {
	Serial.begin(38400);
	while (Serial.available()) {
		Serial.read();
	}
	
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(APIN_UNCONNECTED_FOR_RND));
	
	s_DipValue= s_DipSwitch.getValue();
	Serial.print("DIP Value="); Serial.println(s_DipValue);
	
	switch(s_DipValue)
	{
		case 0: 	NUM_LEDS= 5; break;
		case 1: 	NUM_LEDS= 50; break;
		case 2: 	NUM_LEDS= 100; break;
		case 3: 	NUM_LEDS= 150; break;
		default: 	
			NUM_LEDS= 220; // too many
			break;
	}
		
	s_leds= new CRGB[NUM_LEDS];
	
	//Set all lights to make sure all are working as expected
	//FastLED.addLeds<NEOPIXEL, DPIN_LED>(s_leds, NUM_LEDS);
	FastLED.addLeds<WS2813_GRB, DPIN_LED>(s_leds, NUM_LEDS);
	
	CRGB	initValue;
	
	if (_DEBUG) {
		initValue= CRGB(255, 0, 0);
	} else if (s_DipValue >= 4) {
		initValue= CRGB(0, 0, 255);
	} else {
		initValue= CRGB(0, 255, 0);
	}
	
	for (int i = 0; i < NUM_LEDS; i++)
	{
		s_leds[i] = initValue;
	}
	FastLED.show();
	
	if (_DEBUG)
	{
		delay(3*1000);
	}
	else 
	{
		delay(1000);
	}

	//Initial values
	high.times = 0;
	high.lastMillis = millis();
	s_ColorOffset.r = 0;
	s_ColorOffset.g = 0;
	s_ColorOffset.b = 1;
}

/*With this we can change the mode if we want to implement a general
lamp feature, with for instance general pulsing. Maybe if the
sound is low for a while? */
void loop() {
	visualize_music();
	delay(DELAY_MS);       // delay in between reads for stability
}


/**Function to check if the lamp should either enter a SM_HIGH mode,
or revert to SM_NORMAL if already in SM_HIGH. If the sensors report values
that are higher than 1.1 times the average values, and this has happened
more than 30 times the last few milliseconds, it will enter SM_HIGH mode.
TODO: Not very well written, remove hard coded values, and make it more
reusable and configurable.  */
void detectHighMode()
{
	const int avg = s_long_avg.calcAverage();
	const unsigned long currenMillis= millis();
	const unsigned long elapsedSinceLastHigh= currenMillis - high.lastMillis;
	
	if ((float)avg > ((float)s_song_avg * 1.1 /(float)s_iter ))
	{
		if (high.times != 0)
		{
			if (elapsedSinceLastHigh > 200.0)
			{
				high.times = 0;
				songmode = SM_NORMAL;
			}
			else
			{
				high.lastMillis = currenMillis;
				high.times++;
			}
		}
		else
		{
			high.times++;
			high.lastMillis = currenMillis;

		}
	}
	if (high.times > 30 && elapsedSinceLastHigh < 50.0) 
	{
		if (songmode != SM_HIGH)
		{
			songmode = SM_HIGH;
			if (_DEBUG)
			{
				Serial.println(F("HIGH mode"));
			}
		}
	}
	else if (elapsedSinceLastHigh > 200) // longer break than 200ms - how can this happen?
	{
		high.times = 0;
		if (songmode != SM_NORMAL)
		{
			songmode = SM_NORMAL;
			if (_DEBUG)
			{
				Serial.println(F("NORMAL mode"));
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



int readNoiceLevelFunc() {
	return analogRead(APIN_POTI_NOICE_LEVEL);
}

#define MAX_POTI	800


int readNoiceLevel() {
	static CachedValue<int, 1000/*ms*/>	s_NoiceLevelValue(readNoiceLevelFunc);
	int v= s_NoiceLevelValue.readValue();
		
	//pinMode(DPIN_PWM_RED, OUTPUT);
	//pinMode(DPIN_PWM_RED,INPUT_PULLUP);
	//delay(50);
	
	int level= map(v, 0, MAX_POTI, 0, NOICE_MAX);
	
	if (_DEBUG) {
		Serial.print("readNoiceLevel="); Serial.print(v); Serial.print("V=>"); Serial.println(level);
	}
	return level;
}


int readMicLevelFunc() {
	return analogRead(APIN_POTI_MAX_MIC_LEVEL);
}


int readMaxMicLevel() {
	static CachedValue<int, 1000/*ms*/>	s_MaxMicLevelSensorValue(readMicLevelFunc);
	int v= s_MaxMicLevelSensorValue.readValue();
	
	// pinMode(DPIN_PWM_RED, OUTPUT);
	//pinMode(DPIN_PWM_RED,INPUT_PULLUP);
	//delay(50);
	
	int n= map(v, 0, MAX_POTI, 0, MAX_MIC_LEVEL);
	const int noiceLevel= readNoiceLevel();
	const int offset= DC_OFFSET + noiceLevel;
	int level= (n > offset)? n - offset : 0;
	
	if (_DEBUG) {
		Serial.print("readMaxMicLevel="); Serial.print(v); Serial.print("V=>"); Serial.println(level);
	}
	return level;
}

int readMic()
{
	const int noiceLevel= readNoiceLevel();
	const int offset= DC_OFFSET + noiceLevel;
	int n= analogRead(APIN_MIC_SENSOR);                 // Raw reading from mic

	return (n > offset)? n - offset : 0;
}

int sensor_value_max= 0;
int sensor_value_min= 2048;

//Main function for visualizing the sounds in the lamp
void visualize_music()
{
	const int maxMicLevel= readMaxMicLevel(); // noice level is subtracted already
	//Actual sensor value
	const int sensor_value = readMic();
	
	//If 0, discard immediately. Probably not right and save CPU.
	if (sensor_value == 0) {
		return;
	}

	const int mapped = (float)fscale(0.0, maxMicLevel, 0.0, maxMicLevel, (float)sensor_value, 2.0);
	const int avgMapped = s_avgsMapped.calcAverage();

	//Discard readings that deviates too much from the past avg.
	//if ((abs(avgMapped - mapped) > avgMapped*DEV_THRESH))
	//{
		//if (_DEBUG)
		//{
			//Serial.print("avg - mapped="); Serial.println(avgMapped - mapped);
		//}
		//return;
	//}

	if (_DEBUG)
	{
		Serial.print("sensor_value="); Serial.print(sensor_value);
		if (sensor_value > sensor_value_max)
		{
			sensor_value_max= sensor_value;
			Serial.print(", MAX="); Serial.print(sensor_value_max);
		}
		else if (sensor_value < sensor_value_min)
		{
			sensor_value_min= sensor_value;
			Serial.print(", MIN="); Serial.print(sensor_value_min);
		}
		Serial.print(", mapped="); Serial.println(mapped);
	}
	
	//Insert new avg. values
	s_avgsMapped.add(mapped);
	s_long_avg.add(avgMapped);

	//Compute the "song average" sensor value
	s_song_avg += avgMapped;
	s_iter++;
	if (s_iter > AVG_KEEP_CYCLES) {
		s_song_avg = s_song_avg / s_iter;
		s_iter = 1;
	}
	
	//Check if we enter SM_HIGH mode
	detectHighMode();

	//The speed the LEDs fade to black if not relit
	float fade_scale = 1.2;

	if (songmode == SM_HIGH) {
		fade_scale = 3.0;
		s_ColorOffset.r = 5;
		s_ColorOffset.g = 3;
		s_ColorOffset.b = -1;
	}
	else if (songmode == SM_NORMAL) {
		fade_scale = 2.0;
		s_ColorOffset.r = -1;
		s_ColorOffset.b = 2;
		s_ColorOffset.g = 1;
	}

	//Decides how many of the LEDs will be lit
	//const int show_amount = ceil(fscale(0.0, MAX_MIC_LEVEL, 0.0, (float)NUM_LEDS, (float)avgMapped, -1));
	const int show_amount = ceil(fscale(0.0, maxMicLevel, 0.0, (float)NUM_LEDS, (float)mapped, -1));
	
	if (_DEBUG)
	{
		Serial.print("#led="); Serial.println(show_amount);
	}

	/*Set the different leds. Control for too high and too low values.
	Fun thing to try: Dont account for overflow in one direction,
	some interesting light effects appear! */
	for (int i = 0; i < NUM_LEDS; i++)
	{
		CRGB&	currLed= s_leds[i];
			
		//The leds we want to show
		if (i < show_amount)
		{
			addOffset(currLed.r, s_ColorOffset.r);
			addOffset(currLed.g, s_ColorOffset.g);
			addOffset(currLed.b, s_ColorOffset.b);
		}
		else
		{
			//All the other LEDs begin their fading journey to eventual total darkness
			currLed.r = (float) currLed.r / fade_scale; 
			currLed.g = (float) currLed.g / fade_scale; 
			currLed.b = (float) currLed.b / fade_scale;
		}
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

	curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
	curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

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


