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

uint rgba(float r, float g, float b, float a = 1)
{
	return (uint(round(a * 255)) << 24) + (uint(round(r * 255)) << 16) + (uint(round(g * 255)) << 8) + uint(round(b * 255));
}

uint set_alpha(uint colour, float a)
{
	colour &= 0x00FFFFFF;
	return colour | (uint(round(a * 255)) << 24);
}