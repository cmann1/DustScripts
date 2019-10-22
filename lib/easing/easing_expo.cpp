float easing_in_expo(float t)
{
	return pow(2, 10 * t - 10);
}

float easing_out_expo(float t)
{
	return 1 - pow(2, -10 * t);
}

float easing_in_out_expo(float t)
{
	return ((t *= 2) <= 1 ? pow(2, 10 * t - 10) : 2 - pow(2, 10 - 10 * t)) / 2;
}