// Acceleration until halfway, then deceleration
float easing_in_out_quint(float t)
{
	return t < .5 ? 16 * t * t * t * t * t : 1 + 16 * (--t) * t * t * t * t;
}