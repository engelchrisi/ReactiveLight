#include "stdafx.h"

#include <avr/io.h> // _BV
#include "DipSwitch.h"
#include "pins.h"
#include "CachedValue.h"
#include "Logging.h"
#include "UpDownAnimation.h"
#include "TheaterChaseAnimation.h"
#include "SophiaAnimation.h"
#include "RainbowAnimation.h"
#include "ColorWipeAnimation.h"
#include "ScannerAnimation.h"
#include "FadeAnimation.h"

// macros
// http://yaab-arduino.blogspot.co.il/2015/02/fast-sampling-from-analog-input.html
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

static DipSwitch<DPIN_DIP_0, DPIN_DIP_2>	s_DipSwitch;

static CLEDController* s_pController= NULL;

#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))

static IAnimation* NewAnimation();

/*Sometimes readings are wrong or strange. How much is a reading allowed
to deviate from the average to not be discarded? **/
#define DEV_THRESH 0.8


float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);


//Led array
#define		MAX_NUMLEDS		220
static CRGB s_leds[MAX_NUMLEDS]= {0};

bool _DEBUG= false;

template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2813_GRB : public WS2813Controller<DATA_PIN, GRB> {};
	
uint8_t readDipSwitchFunc();
static CachedValue<uint8_t, 10*1000/*ms*/>	s_DipSwitchValue(readDipSwitchFunc);

static Scheduler s_scheduler; // Create a global Scheduler object

// ------------------------------------------------------------


uint8_t readDipSwitchFunc() {
	return s_DipSwitch.getValue();
}


void TestLEDs()
{
	CRGB	initValue= CRGB(0, 0, 255);

	CRGB* leds= s_pController->leds();
	const int numLeds= s_pController->size();
	for (int i = 0; i < numLeds; i++)
	{
		leds[i] = initValue;
	}
	
	FastLED.show();
	
	float fadeScale= 1.3;
	for (int j= 0; j <20; ++j)
	{
		for (int i = 0; i < numLeds; i++)
		{
			fade(leds[i], fadeScale);
		}
		FastLED.show();

		Utils::delayMs(100);
	}
	
}



class AnimationChanger : public Process
{
public:
	AnimationChanger(Scheduler &manager, ProcPriority pr, uint32_t period) : Process(manager, pr, period)
	{
		_pCurrentAnimation= NULL;
	}
		
	virtual ~AnimationChanger()
	{
		_pCurrentAnimation->Disable();
		delete _pCurrentAnimation;
	}
	
protected:
	virtual void service()
	{
		IAnimation* pLastAnimation= _pCurrentAnimation;
		if (pLastAnimation != NULL)
		{
			pLastAnimation->Disable();
			delete pLastAnimation;
			pLastAnimation= NULL;
		}

		_pCurrentAnimation= NewAnimation();
		_pCurrentAnimation->Enable();
	}
	
private:
	IAnimation* _pCurrentAnimation;
};

static AnimationChanger	s_AnimationChanger(s_scheduler, LOW_PRIORITY, 1 * 60 * 1000);

void setup() 
{
	int numLeds= 0;
	
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(APIN_UNCONNECTED_FOR_RND));
#if LOGGING
	Serial.begin(38400); 
	while (!Serial);
	while (Serial.available()) {
		Serial.read();
	}
#endif
	const uint8_t nDipValue= s_DipSwitchValue.readValue();
	LOGF2("#DIP Value=", nDipValue); 
	
	switch(nDipValue)
	{
		case 0: 	numLeds= 30; _DEBUG= true; break;
		case 1: 	numLeds= 50; _DEBUG= true; break;
		case 2: 	numLeds= 100; _DEBUG= false; break;
		case 3: 	numLeds= 150; _DEBUG= false; break;
		default:	numLeds= MAX_NUMLEDS;
					_DEBUG= false;
					break;
	}
	
#ifdef ADCFlow
	adc_init();
#else
	#ifdef EXTERNAL_VOL_REF
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
		
	//Set all lights to make sure all are working as expected
	s_pController= &FastLED.addLeds<WS2813_GRB, DPIN_LED>(s_leds, numLeds);
	
	//TestLEDs();
	
	s_AnimationChanger.add(true);
	// run once now
	s_AnimationChanger.force();

}  // setup

static IAnimation* NewAnimation()
{
	IAnimation* pNew= NULL;
	const int rnd= random(140);
	if (rnd < 20)
	{
		pNew= new TheaterChaseAnimation(s_scheduler, s_pController,
										Wheel(random(255), SM_NORMAL), Wheel(random(255), SM_HIGH));
	}
	else if (rnd < 40)
	{
		pNew= new RainbowAnimation(s_scheduler, s_pController);
	}
	else if (rnd < 60)
	{
		pNew= new ColorWipeAnimation(s_scheduler, s_pController, Wheel(random(255), SM_NORMAL));
	}
	else if (rnd < 80)
	{
		pNew= new ScannerAnimation(s_scheduler, s_pController, Wheel(random(255), SM_NORMAL));
	}
	else if (rnd < 100)
	{
		pNew= new FadeAnimation(s_scheduler, s_pController);
	}
	else if (rnd < 120)
	{
		pNew= new SophiaAnimation(s_scheduler, s_pController,
				CRGB::Yellow, CRGB::Blue);
	}
	else
	{
		pNew= new UpDownAnimation(s_scheduler, s_pController);
	}
	
	return pNew;
}
	
//
// LOOP
//
void loop() 
{
    s_scheduler.run();
}
