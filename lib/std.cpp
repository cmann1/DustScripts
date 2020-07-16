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
void puts(float x, int y) { puts(str(x) + ', ' + y); }
void puts(int x, float y) { puts(x + ', ' + str(y)); }
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

// Can be removed if the values returned by mouse_hud are fixed
float calibrated_mouse_hud(scene@ g, float &out mouse_x, float &out mouse_y, float mouse_scale, int player = 0)
{
	const float x = g.mouse_x_hud(player);
	const float y = g.mouse_y_hud(player);
	
	mouse_x = x * mouse_scale;
	mouse_y = y * mouse_scale;
	
	// If the mouse coordinates ever go outside of the bounds, reduce the scale until they're inside the bounds.
	// Use a bound greater than the real bound in order to avoid floating point errors causing problems
	if(abs(mouse_x) > SCREEN_RIGHT + 0.1)
	{
		mouse_scale = SCREEN_RIGHT / abs(x);
		
		// Readjust the scaled values, so that we can check the Y against the new scale, and use the correct values immediately
		mouse_x = x * mouse_scale;
		mouse_y = y * mouse_scale;
	}
	
	if(abs(mouse_y) > SCREEN_BOTTOM + 0.1)
	{
		mouse_scale = SCREEN_BOTTOM / abs(y);
		
		// Readjust the scaled values, so that we can use the correct values immediately
		mouse_x = x * mouse_scale;
		mouse_y = y * mouse_scale;
	}
	
	return mouse_scale;
}