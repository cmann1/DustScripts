// Decelerating to zero velocity
float easing_out_quart(float t)
{
	return 1 - (--t) * t * t * t;
}