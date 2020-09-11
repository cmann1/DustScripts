/// Accelerating from zero velocity
float ease_in_quad(float t)
{
	return t * t;
}

/// Decelerating to zero velocity
float ease_out_quad(float t)
{
	return t * (2 - t);
}

/// Acceleration until halfway, then deceleration
float ease_in_out_quad(float t)
{
	return t < .5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}