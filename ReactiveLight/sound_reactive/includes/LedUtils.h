
#ifndef LEDUTILS_H_
#define LEDUTILS_H_

//How much to increment or decrement each color every cycle
struct RgbColorT {
	int r;
	int g;
	int b;
};

CRGB MorpheColors(CRGB color1, CRGB color2, int current, int max);

uint32_t Wheel(byte WheelPos, SongModeT songMode);

void addOffset(uint8_t& color, int colOffset);
void addOffset(CRGB& rgb, const RgbColorT& offset);
void fade(CRGB& rgb, float fade_scale);

uint8_t Red(uint32_t color);
uint8_t Green(uint32_t color);
uint8_t Blue(uint32_t color);


CRGB DimColor(uint32_t color);


#endif /* LEDUTILS_H_ */