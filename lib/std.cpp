const float DT = 1.0 / 60;
const float PIXEL2TILE = 1.0 / 48;
const float TILE2PIXEL = 48;

const float SCREEN_LEFT = -800;
const float SCREEN_TOP = -450;
const float SCREEN_RIGHT = 800;
const float SCREEN_BOTTOM = 450;
const float SCREEN_WIDTH_F = 1920.0 / 1080;

const float MAX_FLOAT =  3.402823466e+38;
const float MIN_FLOAT = -3.402823466e+38;
const float SMALLEST_FLOAT = 1.175494351e-38;

const float NAN = fpFromIEEE(0x7fc00000);
const float INFINITY = fpFromIEEE(0x7f800000);

int tile_coord(float v)
{
	return int(floor(v * PIXEL2TILE));
}

string str(float x)
{
	return formatFloat(x, '', 0, 3);
}
string str(float x, float y)
{
	return '<' + str(x) + ', ' + str(y) + '> ';
}

string bin(uint64 x, uint max_bits=32)
{
	string result = "";
	
	while(max_bits-- > 0)
	{
		result = ((x & 1 == 1) ? "1" : "0") + result;
		x >>= 1;
	}
	
	return result;
}

string hex(int x, uint width = 0)
{
	return formatInt(x, 'H', width);
}

string hex(uint x, uint width = 0)
{
	return formatUInt(x, 'H', width);
}

void puts(bool x) { puts(x + ''); }
void puts(bool x, bool y) { puts(x + ', ' + y); }
void puts(int8 x) { puts(x + ''); }
void puts(int x) { puts(x + ''); }
void puts(int64 x) { puts(x + ''); }
void puts(uint8 x) { puts(x + ''); }
void puts(uint x) { puts(x + ''); }
void puts(uint64 x) { puts(x + ''); }
void puts(float x) { puts(x + ''); }
void puts(double x) { puts(x + ''); }
void puts(int x, int y) { puts(x + ', ' + y); }
void puts(int x, int y, int z) { puts(x + ', ' + y + ', ' + z); }
void puts(int x, int y, int z, int w) { puts(x + ', ' + y + ', ' + z + ', ' + w); }
void puts(float x, float y) { puts(str(x) + ', ' + str(y)); }
void puts(float x, float y, float z) { puts(str(x) + ', ' + str(y) + ', ' + str(z)); }
void puts(float x, float y, float z, float w) { puts(str(x) + ', ' + str(y) + ', ' + str(z) + ', ' + str(w)); }
void puts(float x, int y) { puts(str(x) + ', ' + y); }
void puts(int x, float y) { puts(x + ', ' + str(y)); }
void puts(string x, string y) { puts(x + ', ' + y); }
void puts(rectangle@ r)
{
	if(@r != null)
		puts(str(r.left(), r.top()) + '' + str(r.right(), r.bottom()));
	else
		puts('null');
}
void puts(entity@ e, rectangle@ r)
{
	if(@r != null)
	{
		const float x = e is null ? 0 : e.x();
		const float y = e is null ? 0 : e.x();
		puts(str(x + r.left(), y + r.top()) + '' + str(x + r.right(), y + r.bottom()));
	}
	else
	{
		puts('null');
	}
}
void puts(controllable@ e, rectangle@ r) { puts(e.as_entity(), r); }
void puts(collision@ c) { if(@c != null) puts(c.rectangle()); else puts('null'); }

float frand()
{
	return float(rand()) / float(0x3fffffff);
}

int rand_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}
float rand_range(float min, float max)
{
	return min + (max - min) * frand();
}

bool is_nan(const float x)
{
	return fpToIEEE(x) == 0x7fc00000;
}

int round_int(float x)
{
	return int(round(x));
}

int floor_int(float x)
{
	return int(floor(x));
}

int ceil_int(float x)
{
	return int(ceil(x));
}