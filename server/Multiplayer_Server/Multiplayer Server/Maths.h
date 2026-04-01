#pragma once
#include <cmath>


class Maths
{
	static float Dot2(float x1, float y1, float x2, float y2)
	{
		return x1 * x2 + y1 * y2;
	}

	static float Len2(float x, float z)
	{
		return std::sqrt(x * x + z * z);
	}
	static void Normalize2(float& x, float& z)
	{
		float l = Len2(x, z);
		if (l > 0.0001f) { x /= l; z /= l; }
	}
};
