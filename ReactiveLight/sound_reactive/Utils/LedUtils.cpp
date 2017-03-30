#include "stdafx.h"
#include "LedUtils.h"


// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
	return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
	return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
	return color & 0xFF;
}

// Calculate linear interpolation between Color1 and Color2
// Optimize order of operations to minimize truncation error
CRGB MorpheColors(CRGB color1, CRGB color2, int current, int max)
{
	const uint16_t delta= (max - current);
	uint8_t red = ((Red(color1) * delta)     + (Red(color2)   * current)) / max;
	uint8_t green = ((Green(color1) * delta) + (Green(color2) * current)) / max;
	uint8_t blue = ((Blue(color1) * delta)   + (Blue(color2)  * current)) / max;
	
	CRGB color(red, green, blue);
	return color;
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
CRGB DimColor(uint32_t color)
{
	// Shift R, G and B components one bit to the right
	return CRGB(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
}


// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, SongModeT songMode)
{
	if (songMode == SM_NORMAL)
	{
		WheelPos = 255 - WheelPos;
		if (WheelPos < 85)
		{
			return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
		}
		else if (WheelPos < 170)
		{
			WheelPos -= 85;
			return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
		}
		else
		{
			WheelPos -= 170;
			return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
		}
	}
	else
	{
		WheelPos = 255 - WheelPos;
		if (WheelPos < 128)
		{
			return CRGB(255 - WheelPos, WheelPos, 0);
		}
		else
		{
			WheelPos -= 128;
			return CRGB(WheelPos, 255 - WheelPos, 0);
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