// Acceleration until halfway, then deceleration
float easing_in_out_quart(float t)
{
	return t < .5 ? 8 * t * t * t * t : 1 - 8 * (--t) * t * t * t;
}