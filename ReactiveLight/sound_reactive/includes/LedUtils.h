
#ifndef LEDUTILS_H_
#define LEDUTILS_H_

//How much to increment or decrement each color every cycle
struct RgbColorT {
	int r;
	int g;
	int b;
};


uint32_t Wheel(byte WheelPos, SongModeT songMode);

void addOffset(uint8_t& color, int colOffset);
void addOffset(CRGB& rgb, const RgbColorT& offset);
void fade(CRGB& rgb, float fade_scale);



#endif /* LEDUTILS_H_ */