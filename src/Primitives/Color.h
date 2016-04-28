#pragma once
#include "../Math/Vec3.h"
namespace Graphics
{
	//Represents an integer color.
	class Color
	{
	public:
#pragma region Predefined Colors
		static const Color Black;
		static const Color White;
		static const Color Gray75;
		static const Color Gray50;
		static const Color Gray25;

		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Orange;
#pragma endregion

		float R, G, B;

		Color();
		Color(float pR, float pG, float pB);
	};
}

