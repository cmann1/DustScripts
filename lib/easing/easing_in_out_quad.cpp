// Acceleration until halfway, then deceleration
float easing_in_out_quad(float t)
{
	return t < .5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}