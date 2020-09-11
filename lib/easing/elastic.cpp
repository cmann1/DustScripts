#include '../math/math.cpp';

float ease_in_elastic(const float x)
{
	const float c4 = (2.0 * PI) / 3.0;

	return x == 0.0
		? 0.0 : x == 1.0
			? 1.0
			: -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
}

float ease_out_elastic(const float x)
{
	const float c4 = (2 * PI) / 3;

	return x == 0.0
		? 0.0 : x == 1.0
			? 1.0
			: pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
}

float ease_in_out_elastic(const float x)
{
	const float c5 = (2 * PI) / 4.5;

	return x == 0.0
	? 0.0 : x == 1.0
		? 1.0
		: x < 0.5
			? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
			: (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
}