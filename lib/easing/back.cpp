float ease_in_back(const float x)
{
	const float c1 = 1.70158;
	const float c3 = c1 + 1;

	return c3 * x * x * x - c1 * x * x;
}

float ease_out_back(const float x)
{
	const float c1 = 1.70158;
	const float c3 = c1 + 1;
	
	return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

float ease_in_out_back(const float x)
{
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}