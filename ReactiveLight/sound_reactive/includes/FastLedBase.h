#ifndef __FASTLEDBASE_H__
#define __FASTLEDBASE_H__

class FastLedBase
{
public:
	typedef CRGB Color;

	FastLedBase(CLEDController* pController) : m_pController(pController)
	{
	}
	
	CRGB * getLEDs() {
		return getController()->leds();
	}

	int getNumLEDs() const {
		return const_cast<FastLedBase*>(this)->getController()->size();
	}

	CRGB getPixelColor(int i)
	{
		const CRGB* leds= getController()->leds();
	
		if (i < getNumLEDs())
		{
			return leds[i];
		}
		return CRGB(0,0,0);
	}

	void setPixelColor(int i, const CRGB& color)
	{
		if (i < getNumLEDs())
		{
			CRGB* leds= getLEDs();
			leds[i]= color;
		}
	}

	void show()
	{
		FastLED.show();
	}
	
protected:
	CLEDController* getController() 
	{ 
		if (m_pController == NULL)
		{
			LOGF("NPE controller");
			Utils::delayMs(100);
		}
		return m_pController; 
	}
	
	const CLEDController* getController() const
	{
		return const_cast<FastLedBase*>(this)->getController();
	}

private:
	CLEDController* m_pController;
}; //FastLedBase

#endif //__FASTLEDBASE_H__
