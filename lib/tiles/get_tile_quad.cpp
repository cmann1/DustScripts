#include 'TileShape.cpp';

void get_tile_quad(int shape,
	float &out x1, float &out y1, float &out x2, float &out y2,
	float &out x3, float &out y3, float &out x4, float &out y4)
{
	switch(shape)
	{
		case TileShape::Full:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big_1:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 24;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_1:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 48;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big_2:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 24; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_2:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 0;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Big_3:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 24;
			break;
		case TileShape::Small_3:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 24;
			x4 = 48; y4 = 24;
			break;
		case TileShape::Big_4:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_4:
			x1 = 48; y1 = 0;
			x2 = 48; y2 = 48;
			x3 = 24; y3 = 48;
			x4 = 24; y4 = 48;
			break;
		case TileShape::Big_5:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_5:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 24;
			x3 = 48; y3 = 48;
			x4 = 48; y4 = 48;
			break;
		case TileShape::Big_6:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 24; y4 = 48;
			break;
		case TileShape::Small_6:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 48; y4 = 48;
			break;
		case TileShape::Big_7:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 24;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_7:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 0; y3 = 24;
			x4 = 0; y4 = 24;
			break;
		case TileShape::Big_8:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Small_8:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 48;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Half_A:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 48;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Half_B:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 0; y3 = 48;
			x4 = 0; y4 = 48;
			break;
		case TileShape::Half_C:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 48; y4 = 48;
			break;
		case TileShape::Half_D:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 0;
			x3 = 48; y3 = 48;
			x4 = 48; y4 = 48;
			break;
	}
}