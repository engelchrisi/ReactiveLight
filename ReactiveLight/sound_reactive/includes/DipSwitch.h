#ifndef __DIPSWITCH_H__
#define __DIPSWITCH_H__

template <int START_PIN, int END_PIN>
class DipSwitch 
{

public:
	DipSwitch()
	{

		for (int pin= START_PIN; pin <= END_PIN; ++pin)
		{
			/* Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
			 20K-ohm resistor is pulled to 5V. This configuration causes the input to
			 read HIGH when the switch is open, and LOW when it is closed.*/
			 pinMode(pin, INPUT_PULLUP);
		}
	}	
	
	uint8_t getValue() const 
	{
		uint8_t value= 0;
		
		for (int pin= START_PIN; pin <= END_PIN; ++pin)
		{
			if (LOW == digitalRead(pin)) // LOW => closed, s. INPUT_PULLUP
			{
				value |= 1 << (pin-START_PIN);
				
			}
		}
		
		return value;
	}
	
};



#endif // __DIPSWITCH_H__
