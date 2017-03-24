/*
 * pins.h
 *
 * Created: 05.02.2017 20:58:29
 *  Author: D031288
 */ 


#ifndef PINS_H_
#define PINS_H_

//////////////////////////////////////////////////
// DIGITAL
//
// https://github.com/GreyGnome/EnableInterrupt/wiki/Usage#Summary
// Arduino Uno/Duemilanove/etc.
//
// Interrupt Type | Pins
// -------------- | --------------
// External       | 2 3
// Pin Change     | 2-13 and A0-A5
//
//////////////////////////////////////////////////////////////////
// PWMs
//
// ATMega
// PWM: 2 to 13 and 44 to 46. Provide 8-bit PWM output with the analogWrite() function.
// Nano:
// PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite() function.
// NOTE: the tone() function will interfere with PWM output on pins 3 and 11 (on boards other than the Mega).
// no other PWM on this pin

//The pin that we read sensor values from microphone
#define APIN_MIC_SENSOR					A0

// initialization of randomSeed
#define APIN_UNCONNECTED_FOR_RND		A5

// the poti to change the noice level
#define APIN_POTI_NOICE_LEVEL			A6

// the poti to change the maximum value
#define APIN_POTI_MAX_MIC_LEVEL			A7

#define DPIN_DIP_0				2
#define DPIN_DIP_1				3
#define DPIN_DIP_2			    4

//The pin that controls the LEDs
#define DPIN_LED				6



#endif /* PINS_H_ */