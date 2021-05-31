#include '../std.cpp';
#include '../string.cpp';

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
		const float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		const float p = 2 * l - q;
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

/// https://stackoverflow.com/a/17243070/153844
uint hsv_to_rgb(const float h, const float s, const float v)
{
	float r, g, b;
	
	const float i = floor(h * 6);
	const float f = h * 6 - i;
	const float p = v * (1 - s);
	const float q = v * (1 - f * s);
	const float t = v * (1 - (1 - f) * s);
	
	switch(int(i) % 6)
	{
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
	}
	
	return
		(uint(round(r * 255)) << 16) |
		(uint(round(g * 255)) << 8) |
		(uint(round(b * 255)));
}

/// https://stackoverflow.com/a/17243070/153844
void rgb_to_hsv(const int r, const int g, const int b, float &out h, float &out s, float &out v)
{
	const int rgb_min = min(min(r, g), b);
	const int rgb_max = max(max(r, g), b);
	const int d = rgb_max - rgb_min;
	h = 0;
	s = (rgb_max == 0 ? 0 : float(d) / rgb_max);
	v = rgb_max / 255.0;
	
	if(rgb_max == rgb_min)
	{
		h = 0;
	}
	else if(rgb_max == r)
	{
		h = (g - b) + d * (g < b ? 6.0 : 0.0);
		h /= 6 * d;
	}
	else if(rgb_max == g)
	{
		h = (b - r) + d * 2;
		h /= 6 * d;
	}
	else if(rgb_max == b)
	{
		h = (r - g) + d * 4;
		h /= 6 * d;
	}
}

/// https://stackoverflow.com/a/17243070/153844
void hsv_to_hsl(const float h, const float s, const float v, float &out out_h, float &out out_s, float &out out_l)
{
	out_h = h;
	out_s = s * v;
	out_l = (2 - s) * v;
	out_s /= (out_l <= 1) ? out_l : 2 - out_l;
	out_l /= 2;
}

/// https://stackoverflow.com/a/17243070/153844
void hsl_to_hsv(const float h, float s, float l, float &out out_h, float &out out_s, float &out out_v)
{
	out_h = h;

	l *= 2;
	s *= (l <= 1) ? l : 2 - l;
	out_v = (l + s) / 2;
	out_s = (2 * s) / (l + s);
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
	a = (colour >> 24) & 0xff;
	r = (colour >> 16) & 0xff;
	g = (colour >> 8) & 0xff;
	b = (colour) & 0xff;
}

void int_to_rgba(uint colour, float &out r, float &out g, float &out b, float &out a)
{
	a = ((colour >> 24) & 0xff) / 255;
	r = ((colour >> 16) & 0xff) / 255;
	g = ((colour >> 8) & 0xff) / 255;
	b = ((colour) & 0xff) / 255;
}

uint adjust_lightness(const uint colour, const float &in amount)
{
	float h, s, l;
	
	rgb_to_hsl(
		(colour >> 16) & 0xFF, (colour >> 8) & 0xFF, (colour) & 0xFF,
		h, s, l);
	
	l = clamp(l + amount, 0.0, 1.0);
	
	return hsl_to_rgb(h, s, l) | (colour & 0xff000000);
}

uint scale_lightness(const uint &in colour, const float &in amount)
{
	float h, s, l;
	
	rgb_to_hsl(
		(colour >> 16) & 0xFF, (colour >> 8) & 0xFF, (colour) & 0xFF,
		h, s, l);
	
	l = clamp(l * (1 + amount), 0.0, 1.0);
	
	return hsl_to_rgb(h, s, l) | (colour & 0xff000000);
}

uint adjust(const uint colour, const float hue, const float saturation, const float lightness)
{
	float h, s, l;
	
	rgb_to_hsl(
		(colour >> 16) & 0xFF, (colour >> 8) & 0xFF, (colour) & 0xFF,
		h, s, l);
	
	h = (h + hue) % 1;
	s = clamp(s + saturation, 0.0, 1.0);
	l = clamp(l + lightness, 0.0, 1.0);
	
	if(h < 0)
		h += 1;
	
	return hsl_to_rgb(h, s, l) | (colour & 0xff000000);
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

uint unique_colour(const string id, const uint alpha=0xff)
{
	const float hash = float(string::hash(id));
	return hsl_to_rgb(
		sin(hash) * 0.5 + 0.5,
		map(sin(hash) * 0.5 + 0.5, 0, 1, 0.8, 0.9),
		map(sin(hash) * 0.5 + 0.5, 0, 1, 0.65, 0.75)
		) | (alpha << 24);
}

uint lerp_colour(const uint start, const uint end, const float t)
{
	const float sa = (start >> 24) & 0xff;
	const float sr = (start >> 16) & 0xff;
	const float sg = (start >>  8) & 0xff;
	const float sb = (start      ) & 0xff;

	const float ea = (end >> 24) & 0xff;
	const float er = (end >> 16) & 0xff;
	const float eg = (end >>  8) & 0xff;
	const float eb = (end      ) & 0xff;

	return
		(uint(sa + (ea - sa) * t) << 24) |
		(uint(sr + (er - sr) * t) << 16) |
		(uint(sg + (eg - sg) * t) << 8) |
		 uint(sb + (eb - sb) * t);
}
