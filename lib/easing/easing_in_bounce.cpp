#include 'easing_out_bounce.cpp';

float easing_in_bounce(float t)
{
	return 1 - easing_out_bounce(1 - t);
}