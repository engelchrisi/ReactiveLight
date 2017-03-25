#include "stdafx.h"

#include <avr/io.h> // _BV
#include "DipSwitch.h"
#include "pins.h"
#include "CachedValue.h"
#include "Logging.h"
#include "UpDownAnimation.h"
#include "TheaterChaseAnimation.h"

// macros
// http://yaab-arduino.blogspot.co.il/2015/02/fast-sampling-from-analog-input.html
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

static DipSwitch	s_DipSwitch(DPIN_DIP_0, DPIN_DIP_2);

static CLEDController* s_pController= NULL;

#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))

static IAnimation* NewAnimation();

char g_bufLogging[256]= "";

/*Sometimes readings are wrong or strange. How much is a reading allowed
to deviate from the average to not be discarded? **/
#define DEV_THRESH 0.8


float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);


//Led array
static CRGB *s_leds= NULL;

bool _DEBUG= false;

template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2813_GRB : public WS2813Controller<DATA_PIN, GRB> {};
	
int readDipSwitchFunc();
static CachedValue<int, 10*1000/*ms*/>	s_DipSwitchValue(readDipSwitchFunc);

static Scheduler s_scheduler; // Create a global Scheduler object

static IAnimation* s_pCurrentAnimation= NULL;

// ------------------------------------------------------------


int readDipSwitchFunc() {
	return s_DipSwitch.getValue();
}


void TestLEDs()
{
	CRGB	initValue= CRGB(0, 0, 255);

	CRGB* leds= s_pController->leds();
	for (int i = 0; i < s_pController->size(); i++)
	{
		leds[i] = initValue;
	}
	
	FastLED.show();
	delay(1000);
}

void ChangeAnimation()
{
	IAnimation* pLastAnimation= s_pCurrentAnimation;

	s_pCurrentAnimation= NewAnimation();
	s_pCurrentAnimation->Enable();
	
	if (pLastAnimation != NULL)
	{
		pLastAnimation->Disable(); 
		delete pLastAnimation;
		pLastAnimation= NULL;
	}
}


class AnimationChanger : public Process
{
public:
	AnimationChanger(Scheduler &manager, ProcPriority pr, uint32_t period) : Process(manager, pr, period)
	{
	}

protected:
	virtual void service()
	{
		ChangeAnimation();
	}
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
		
	Serial.begin(38400); // );115200
	while (!Serial);
	while (Serial.available()) {
		Serial.read();
	}
		
	const int nDipValue= s_DipSwitchValue.readValue();
	LOGF2("#DIP Value=", nDipValue); 
	
	switch(nDipValue)
	{
		case 0: 	numLeds= 30; _DEBUG= true; break;
		case 1: 	numLeds= 50; _DEBUG= true; break;
		case 2: 	numLeds= 100; _DEBUG= false; break;
		case 3: 	numLeds= 150; _DEBUG= false; break;
		default:	numLeds= 220;
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
		
	s_leds= new CRGB[numLeds];
	
	//Set all lights to make sure all are working as expected
	//FastLED.addLeds<NEOPIXEL, DPIN_LED>(s_leds, NUM_LEDS);
	s_pController= &FastLED.addLeds<WS2813_GRB, DPIN_LED>(s_leds, numLeds);
	
	TestLEDs();
	
	s_AnimationChanger.add(true);
	// run once now
	s_AnimationChanger.force();

}  // setup

static IAnimation* NewAnimation()
{
	IAnimation* pNew= NULL;
	const int rnd= random(40);
	if (rnd < 20)
	{
		pNew= new TheaterChaseAnimation(s_scheduler, s_pController,
										CRGB(0,37,248), //Wheel(random(255), SM_NORMAL);
										CRGB(255,255,0) //Wheel(random(255), SM_NORMAL);
										);
	}
	//else if (rnd < 40)
	//{
	//}
	//else if (rnd < 60)
	//{
	//}
	//else if (rnd < 80)
	//{
	//}
	//else if (rnd < 100)
	//{
	//}
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
