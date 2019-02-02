// Decelerating to zero velocity
float easing_out_cubic(float t)
{
	return (--t) * t * t + 1;
}