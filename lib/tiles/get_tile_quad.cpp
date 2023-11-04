#include 'TileShape.cpp';

void get_tile_quad(int shape,
	float &out x1, float &out y1, float &out x2, float &out y2,
	float &out x3, float &out y3, float &out x4, float &out y4,
	const float x=0, const float y=0)
{
	switch(shape)
	{
		case TileShape::Full:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big1:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 24;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small1:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 48;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big2:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 24; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small2:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 0;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big3:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 24;
			break;
		case TileShape::Small3:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 24;
			x4 = 0; y4 = 0;
			break;
		case TileShape::Big4:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small4:
			x1 = 48; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 24; y4 = 48;
			break;
		case TileShape::Big5:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small5:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 24;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big6:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 24; y4 = 48;
			break;
		case TileShape::Small6:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 48; y4 = 48;
			break;
		case TileShape::Big7:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 24;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small7:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 0;
			x4 = 0; y4 = 24;
			break;
		case TileShape::Big8:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small8:
			x1 = 0; y1 = 0;
			x2 = 0; y2 = 0;
			x3 = 24; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::HalfA:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 48;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::HalfB:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 0;
			x4 = 0; y4 = 48;
			break;
		case TileShape::HalfC:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 0;
			break;
		case TileShape::HalfD:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
	}

	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;
	x3 += x;
	y3 += y;
	x4 += x;
	y4 += y;
}
