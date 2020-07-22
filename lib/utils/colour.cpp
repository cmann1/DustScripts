#include '../std.cpp';

// See https://gist.github.com/mjackson/5311256 for functions if needed

void rgb_to_hsl(uint r_in, uint g_in, uint b_in, float &out h, float &out s, float &out l)
{
	float r = r_in / 255.0;
	float g = g_in / 255.0;
	float b = b_in / 255.0;

	float _max = max(max(r, g), b);
	float _min = min(min(r, g), b);
	l = (_max + _min) / 2.0;

	if(_max == _min)
	{
		h = s = 0; // achromatic
	}
	else
	{
		float d = _max - _min;
		s = l > 0.5 ? d / (2 - _max - _min) : d / (_max + _min);

		if(_max == r)
			h = (g - b) / d + (g < b ? 6 : 0);
		else if(_max == g)
			h = (b - r) / d + 2;
		else if(_max == b)
			h = (r - g) / d + 4;

		h /= 6.0;
	}
}

uint hsl_to_rgb(float h, float s, float l)
{
	float r, g, b;

	if(s == 0)
	{
		r = g = b = l; // achromatic
	}
	else
	{
		float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		float p = 2 * l - q;
		r = hue_to_rgb(p, q, h + 1.0/3.0);
		g = hue_to_rgb(p, q, h);
		b = hue_to_rgb(p, q, h - 1.0/3.0);
	}
	
	return (uint(round(r * 255)) << 16) | (uint(round(g * 255)) << 8) | (uint(round(b * 255)));
}

float hue_to_rgb(float p, float q, float t)
{
	if(t < 0) t += 1;
	if(t > 1) t -= 1;
	if(t < 1/6.0) return p + (q - p) * 6 * t;
	if(t < 1/2.0) return q;
	if(t < 2/3.0) return p + (q - p) * (2/3.0 - t) * 6;
	return p;
}

uint rgba(int r, int g, int b, int a = 255)
{
	return (uint(a) << 24) + (uint(r) << 16) + (uint(g) << 8) + uint(b);
}

uint rgba(float r, float g, float b, float a = 1)
{
	return (uint(round(a * 255)) << 24) + (uint(round(r * 255)) << 16) + (uint(round(g * 255)) << 8) + uint(round(b * 255));
}

void int_to_rgba(uint colour, int &out r, int &out g, int &out b, int &out a)
{
	a = (colour >> 24) & 0xFF;
	r = (colour >> 16) & 0xFF;
	g = (colour >> 8) & 0xFF;
	b = (colour) & 0xFF;
}

void int_to_rgba(uint colour, float &out r, float &out g, float &out b, float &out a)
{
	a = ((colour >> 24) & 0xFF) / 255;
	r = ((colour >> 16) & 0xFF) / 255;
	g = ((colour >> 8) & 0xFF) / 255;
	b = ((colour) & 0xFF) / 255;
}

uint set_alpha(uint colour, float a)
{
	colour &= 0x00FFFFFF;
	return colour | (uint(round(a * 255)) << 24);
}

uint multiply_alpha(uint colour, float a)
{
	a *= ((colour >> 24) & 0xFF) / 255.0;
	colour &= 0x00FFFFFF;
	return colour | (uint(round(a * 255)) << 24);
}

uint random_colour_nice(float alpha_min = 1, float alpha_max = 1)
{
	return hsl_to_rgb(
		rand_range(0.0, 1.0),
		rand_range(0.8, 0.9),
		rand_range(0.65, 0.75)
	) | (int(round(rand_range(alpha_min, alpha_max) * 255)) << 24);
}