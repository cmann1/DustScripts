const float DT = 1.0 / 60;
const float PIXEL2TILE = 1.0 / 48;
const float TILE2PIXEL = 48;

const float SCREEN_LEFT = -800;
const float SCREEN_TOP = -450;
const float SCREEN_RIGHT = 800;
const float SCREEN_BOTTOM = 450;

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

void puts(bool x) { puts(x + ''); }
void puts(int8 x) { puts(x + ''); }
void puts(int x) { puts(x + ''); }
void puts(int64 x) { puts(x + ''); }
void puts(uint8 x) { puts(x + ''); }
void puts(uint x) { puts(x + ''); }
void puts(uint64 x) { puts(x + ''); }
void puts(float x) { puts(x + ''); }
void puts(double x) { puts(x + ''); }
void puts(int x, int y) { puts(x + ', ' + y); }
void puts(float x, float y) { puts(str(x) + ', ' + str(y)); }
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