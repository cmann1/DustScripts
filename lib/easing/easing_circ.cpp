float easing_in_circ(float t)
{
	return 1 - sqrt(1 - t * t);
}

float easing_out_circ(float t)
{
	return sqrt(1 - --t * t);
}

float easing_in_out_circ(float t)
{
	return ((t *= 2) <= 1 ? 1 - sqrt(1 - t * t) : sqrt(1 - (t -= 2) * t) + 1) / 2;
}