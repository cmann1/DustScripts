float ease_in_sine(const float x)
{
	return 1 - cos((x * 3.1415926535897932384626433832795) / 2);
}

float ease_out_sine(const float x)
{
	return sin((x * 3.1415926535897932384626433832795) / 2);
}

float ease_in_out_sine(const float x)
{
	return -(cos(3.1415926535897932384626433832795 * x) - 1) / 2;
}
