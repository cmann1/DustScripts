/// Accelerating from zero velocity
float ease_in_quint(const float t)
{
	return t * t * t * t * t;
}

/// Decelerating to zero velocity
float ease_out_quint(float t)
{
	return 1 + (--t) * t * t * t * t;
}

/// Acceleration until halfway, then deceleration
float ease_in_out_quint(float t)
{
	return t < .5 ? 16 * t * t * t * t * t : 1 + 16 * (--t) * t * t * t * t;
}