/// Acceleration until halfway, then deceleration
float ease_in_out_quart(float t)
{
	return t < .5 ? 8 * t * t * t * t : 1 - 8 * (--t) * t * t * t;
}

/// Decelerating to zero velocity
float ease_out_quart(float t)
{
	return 1 - (--t) * t * t * t;
}

/// Accelerating from zero velocity
float ease_in_quart(const float t)
{
	return t * t * t * t;
}