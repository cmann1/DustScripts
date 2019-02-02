// Decelerating to zero velocity
float easing_out_quint(float t)
{
	return 1 + (--t) * t * t * t * t;
}