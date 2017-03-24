#include "stdafx.h"

#include <avr/io.h> // _BV
#include "DipSwitch.h"
#include "pins.h"
#include "CachedValue.h"
#include "Logging.h"
#include "taskQueue.h"
#include "FastLedTask.h"
#include "SoundUtils.h"
#include "DiscoLightTask.h"


// macros
// http://yaab-arduino.blogspot.co.il/2015/02/fast-sampling-from-analog-input.html
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

static DipSwitch	s_DipSwitch(DPIN_DIP_0, DPIN_DIP_2);

static CLEDController* s_pController= NULL;

#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))

static void addNewVisualizationTask();


char g_bufLogging[256]= "";


//The amount of LEDs in the setup
static int NUM_LEDS= 0;


/*Sometimes readings are wrong or strange. How much is a reading allowed
to deviate from the average to not be discarded? **/
#define DEV_THRESH 0.8


float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);


//Led array
static CRGB *s_leds= NULL;

#if 0
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
#define LONG_SECTOR 20
//Longer sound avg
FifoQueue<int, LONG_SECTOR> s_long_avg;

#endif // 0

bool _DEBUG= false;

template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2813_GRB : public WS2813Controller<DATA_PIN, GRB> {};
	
int readDipSwitchFunc() {
#if true
	return s_DipSwitch.getValue();
#else
	return 1;
#endif
}

static CachedValue<int, 10*1000/*ms*/>	s_DipSwitchValue(readDipSwitchFunc);



void setup() {
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(APIN_UNCONNECTED_FOR_RND));
		
	Serial.begin(38400); // );115200
	while (!Serial);
	while (Serial.available()) {
		Serial.read();
	}
		
	const int nDipValue= s_DipSwitchValue.readValue();
	LOGF2("#DIP Value=", nDipValue);
		
	switch(nDipValue)
	{
		case 0: 	NUM_LEDS= 30; _DEBUG= true; break;
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
	s_pController= &FastLED.addLeds<WS2813_GRB, DPIN_LED>(s_leds, NUM_LEDS);
	
	CRGB	initValue= CRGB(0, 0, 255);

	CRGB* leds= s_pController->leds();
	for (int i = 0; i < s_pController->size(); i++)
	{
		leds[i] = initValue;
	}
	
	FastLED.show();
		
	delay(1000);

	addNewVisualizationTask();
}


class DiscoLightTaskEx : public DiscoLightTask
{
public:
	DiscoLightTaskEx(CLEDController& controller, unsigned long liftimeMs) : DiscoLightTask(controller, liftimeMs)
	{
	}
	
protected:
	virtual bool isFinished() const
	{
		if (DiscoLightTask::isFinished())
		{
			addNewVisualizationTask();
			return true;
		}
		return false;
	}
};

class FastLedTaskEx : public FastLedTask
{
	public:
	static RecurringTask* Factory(unsigned long liftimeMs)
	{
		uint32_t col1= CRGB(0,37,248); //Wheel(random(255), SM_NORMAL);
		RecurringTask* pNew= NULL;

		const int rnd= random(40);
		if (rnd < 20)
		{
			uint32_t col2= CRGB(255,255,0); //Wheel(random(255), SM_NORMAL);
		
			pNew= new FastLedTaskEx(liftimeMs, PT_THEATER_CHASE, col1, col2);
		}
		//else if (rnd < 40)
		//{
			//uint32_t col2= CRGB(0,0,50);
			//pNew= new FastLedTaskEx(liftimeMs, PT_FADE, col1, col2);
		//}
		//else if (rnd < 60)
		//{
			//pNew= new FastLedTaskEx(liftimeMs, PT_COLOR_WIPE, col1);
		//}
		//else if (rnd < 80)
		//{
			//pNew= new FastLedTaskEx(liftimeMs, PT_RAINBOW_CYCLE);
		//}
		//else if (rnd < 100)
		//{
			//pNew= new FastLedTaskEx(liftimeMs, PT_SCANNER, col1);
		//}
		else
		{
			pNew= new DiscoLightTaskEx(*s_pController, liftimeMs);

		}

		return pNew;
	}
	
protected:
	FastLedTaskEx(unsigned long liftimeMs, 
						PatternT  pattern0, uint32_t color1= 0, uint32_t color2= 0, DirectionT dir= DT_FORWARD)
			: FastLedTask(*s_pController, liftimeMs, pattern0, color1, color2, dir)
	{
	}
	
	virtual bool isFinished() const
	{
		if (FastLedTask::isFinished())
		{
			addNewVisualizationTask();
			return true;
		}
		return false;
	}
};

static void addNewVisualizationTask()
{
	const unsigned long liftimeMs= 3l * 60l * 1000l; // 3 min.
	TaskData* pNewTask= FastLedTaskEx::Factory(liftimeMs);
	g_TaskQueue.add(pNewTask);
}

//
// LOOP
//
void loop() 
{
	g_TaskQueue.waitAndExecuteAllTasks();
}


#if 0

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


#endif