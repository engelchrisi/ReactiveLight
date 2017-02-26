#ifndef __DIPSWITCH_H__
#define __DIPSWITCH_H__

class DipSwitch 
{
private:
	int _startPin;
	int _endPin;
	
public:
	DipSwitch(int startPin, int endPin)
	{
		_startPin= startPin;
		_endPin= endPin;
		
		for (int pin= _startPin; pin <= _endPin; ++pin)
		{
			/* Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
			 20K-ohm resistor is pulled to 5V. This configuration causes the input to
			 read HIGH when the switch is open, and LOW when it is closed.*/
			 pinMode(pin, INPUT_PULLUP);
		}
	}	
	
	int getValue() const 
	{
		int value= 0;
		
		for (int pin= _startPin; pin <= _endPin; ++pin)
		{
			if (LOW == digitalRead(pin)) // LOW => closed, s. INPUT_PULLUP
			{
				value |= 1 << (pin-_startPin);
				
			}
		}
		
		return value;
	}
	
};



#endif // __DIPSWITCH_H__
