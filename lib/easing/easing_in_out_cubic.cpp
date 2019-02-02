// Acceleration until halfway, then deceleration
float easing_in_out_cubic(float t)
{
	return t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}