float ease_in_bounce(const float t)
{
	return 1 - ease_out_bounce(1 - t);
}

float ease_out_bounce(float t)
{
	if(t < (1 / 2.75))
	{
		return 7.5625 * t * t;
	}
	else if(t < (2 / 2.75))
	{
		return 7.5625 * (t -= (1.5 / 2.75)) * t + .75;
	}
	else if(t < (2.5 / 2.75))
	{
		return 7.5625 * (t -= (2.25 / 2.75)) * t + .9375;
	}
	else
	{
		return 7.5625 * (t -= (2.625 / 2.75)) * t + .984375;
	}
}

float ease_in_out_bounce(const float x)
{
	return x < 0.5
		? (1 - ease_out_bounce(1 - 2 * x)) / 2
		: (1 + ease_out_bounce(2 * x - 1)) / 2;
}