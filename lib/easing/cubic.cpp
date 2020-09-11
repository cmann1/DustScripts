/// Decelerating to zero velocity
float ease_in_cubic(const float t)
{
	return t * t * t;
}

/// Accelerating from zero velocity
float ease_out_cubic(float t)
{
	return (--t) * t * t + 1;
}

/// Acceleration until halfway, then deceleration
float ease_in_out_cubic(const float t)
{
	return t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}