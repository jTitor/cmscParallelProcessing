#include "Color.h"

using namespace Graphics;

#pragma region Predefined Colors
const Color Color::Black = Color(0, 0, 0);
const Color Color::White = Color(1, 1, 1);
const Color Color::Gray75 = Color(0.75f, 0.75f, 0.75f);
const Color Color::Gray50 = Color(0.5f, 0.5f, 0.5f);
const Color Color::Gray25 = Color(0.25f, 0.25f, 0.25f);

const Color Color::Red = Color(1, 0, 0);
const Color Color::Green = Color(0, 1, 0);
const Color Color::Blue = Color(0, 0, 1);
const Color Color::Yellow = Color(1, 1, 0);
const Color Color::Magenta = Color(1, 0, 1);
const Color Color::Cyan = Color(0, 1, 1);
const Color Color::Orange = Color(1, 0.5f, 0);
#pragma endregion

void init(Color& c, float pR, float pG, float pB)
{
	c.R = pR;
	c.G = pG;
	c.B = pB;
}

Color::Color()
{
	init(*this, 0, 0, 0);
}

Color::Color(float pR, float pG, float pB)
{
	init(*this, pR, pG, pB);
}