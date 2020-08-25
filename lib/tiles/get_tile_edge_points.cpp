#include 'TileShape.cpp';
#include 'TileEdge.cpp';

bool get_tile_top_edge_points(int shape, float &out x1, float &out y1, float &out x2, float &out y2, float x = 0, float y = 0)
{
	switch(shape)
	{
		case TileShape::Full:
		case TileShape::Big2:
		case TileShape::Big3:
		case TileShape::Small3:
		case TileShape::Big6:
		case TileShape::Big7:
		case TileShape::Small7:
		case TileShape::HalfB:
		case TileShape::HalfC:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 0;
			break;
		case TileShape::Big1:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 24;
			break;
		case TileShape::Small1:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 48;
			break;
		case TileShape::Small2:
		case TileShape::Big8:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 0;
			break;
		case TileShape::Big4:
		case TileShape::Small6:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 0;
			break;
		case TileShape::Small4:
			return false;
		case TileShape::Big5:
			x1 = 0; y1 = 24;
			x2 = 48; y2 = 0;
			break;
		case TileShape::Small5:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 24;
			break;
		case TileShape::Small8:
			return false;
		case TileShape::HalfA:
			x1 = 0; y1 = 0;
			x2 = 48; y2 = 48;
			break;
		case TileShape::HalfD:
			x1 = 0; y1 = 48;
			x2 = 48; y2 = 0;
			break;
	}
	
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;
	
	return true;
}

bool get_tile_bottom_edge_points(int shape, float &out x1, float &out y1, float &out x2, float &out y2, float x = 0, float y = 0)
{
	switch(shape)
	{
		case TileShape::Full:
		case TileShape::Big1:
		case TileShape::Small1:
		case TileShape::Small5:
		case TileShape::Big4:
		case TileShape::Big5:
		case TileShape::Big8:
		case TileShape::HalfA:
		case TileShape::HalfD:
			x1 = 48; y1 = 48;
			x2 = 0; y2 = 48;
			break;
		case TileShape::Big2:
		case TileShape::Small8:
			x1 = 24; y1 = 48;
			x2 = 0; y2 = 48;
			break;
		case TileShape::Small2:
			return false;
		case TileShape::Big3:
			x1 = 48; y1 = 48;
			x2 = 0; y2 = 24;
			break;
		case TileShape::Small3:
			x1 = 48; y1 = 24;
			x2 = 0; y2 = 0;
			break;
		case TileShape::Small4:
		case TileShape::Big6:
			x1 = 48; y1 = 48;
			x2 = 24; y2 = 48;
			break;
		case TileShape::Small6:
			return false;
		case TileShape::Big7:
			x1 = 48; y1 = 24;
			x2 = 0; y2 = 48;
			break;
		case TileShape::Small7:
			x1 = 48; y1 = 0;
			x2 = 0; y2 = 24;
			break;
		case TileShape::HalfB:
			x1 = 48; y1 = 0;
			x2 = 0; y2 = 48;
			break;
		case TileShape::HalfC:
			x1 = 48; y1 = 48;
			x2 = 0; y2 = 0;
			break;
	}
	
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;
	
	return true;
}

bool get_tile_left_edge_points(int shape, float &out x1, float &out y1, float &out x2, float &out y2, float x = 0, float y = 0)
{
	switch(shape)
	{
		case TileShape::Full:
		case TileShape::Big1:
		case TileShape::Big2:
		case TileShape::Small2:
		case TileShape::Big7:
		case TileShape::Big8:
		case TileShape::Small8:
		case TileShape::HalfA:
		case TileShape::HalfB:
			x1 = 0; y1 = 48;
			x2 = 0; y2 = 0;
			break;
		case TileShape::Small1:
		case TileShape::Big5:
			x1 = 0; y1 = 48;
			x2 = 0; y2 = 24;
			break;
		case TileShape::Big3:
			x1 = 0; y1 = 24;
			x2 = 0; y2 = 0;
			break;
		case TileShape::Small3:
			return false;
		case TileShape::Big4:
			x1 = 0; y1 = 48;
			x2 = 24; y2 = 0;
			break;
		case TileShape::Small4:
			x1 = 24; y1 = 48;
			x2 = 48; y2 = 0;
			break;
		case TileShape::Small5:
			return false;
		case TileShape::Big6:
			x1 = 24; y1 = 48;
			x2 = 0; y2 = 0;
			break;
		case TileShape::Small6:
			x1 = 48; y1 = 48;
			x2 = 24; y2 = 0;
			break;
		case TileShape::Small7:
			x1 = 0; y1 = 24;
			x2 = 0; y2 = 0;
			break;
		case TileShape::HalfC:
			return false;
		case TileShape::HalfD:
			return false;
	}
	
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;
	
	return true;
}

bool get_tile_right_edge_points(int shape, float &out x1, float &out y1, float &out x2, float &out y2, float x = 0, float y = 0)
{
	switch(shape)
	{
		case TileShape::Full:
		case TileShape::Big3:
		case TileShape::Big4:
		case TileShape::Small4:
		case TileShape::Big5:
		case TileShape::Big6:
		case TileShape::Small6:
		case TileShape::HalfC:
		case TileShape::HalfD:
			x1 = 48; y1 = 0;
			x2 = 48; y2 = 48;
			break;
		case TileShape::Big1:
			x1 = 48; y1 = 24;
			x2 = 48; y2 = 48;
			break;
		case TileShape::Small1:
			return false;
		case TileShape::Big2:
			x1 = 48; y1 = 0;
			x2 = 24; y2 = 48;
			break;
		case TileShape::Small2:
			x1 = 24; y1 = 0;
			x2 = 0; y2 = 48;
			break;
		case TileShape::Small3:
			x1 = 48; y1 = 0;
			x2 = 48; y2 = 24;
			break;
		case TileShape::Small5:
			x1 = 48; y1 = 24;
			x2 = 48; y2 = 48;
			break;
		case TileShape::Big7:
			x1 = 48; y1 = 0;
			x2 = 48; y2 = 24;
			break;
		case TileShape::Small7:
			return false;
		case TileShape::Big8:
			x1 = 24; y1 = 0;
			x2 = 48; y2 = 48;
			break;
		case TileShape::Small8:
			x1 = 0; y1 = 0;
			x2 = 24; y2 = 48;
			break;
		case TileShape::HalfA:
		case TileShape::HalfB:
			return false;
	}
	
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;
	
	return true;
}

void get_edge_points(int shape, int edge, float &out x1, float &out y1, float &out x2, float &out y2, float x = 0, float y = 0)
{
	switch(edge)
	{
		case TileEdge::Top:
			get_tile_top_edge_points(shape, x1, y1, x2, y2, x, y);
			break;
		case TileEdge::Bottom:
			get_tile_bottom_edge_points(shape, x1, y1, x2, y2, x, y);
			break;
		case TileEdge::Left:
			get_tile_left_edge_points(shape, x1, y1, x2, y2, x, y);
			break;
		case TileEdge::Right:
			get_tile_right_edge_points(shape, x1, y1, x2, y2, x, y);
			break;
	}
}