#include '../std.cpp';
#include 'TileShape.cpp';
#include 'TileEdge.cpp';

bool point_in_tile(float x, float y, int tile_ix, int tile_iy, int type, float &out normal_x, float &out normal_y, int layer=19)
{
	const float tile_x = (x - float(tile_ix * TILE2PIXEL)) * PIXEL2TILE;
	const float tile_y = (y - float(tile_iy * TILE2PIXEL)) * PIXEL2TILE;
	
	if(tile_x < 0 || tile_x > 1 || tile_y < 0 || tile_y > 1)
		return false;
	
	// Right and down facing (1/1 gradient)
	const float n1 = 0.7071067811865475;
	// Right and down facing (2/1 gradient)
	const float n2x = 0.8944271909999159;
	const float n2y = 0.4472135954999579;
	
	switch(type)
	{
		case TileShape::Full:
			if(abs(tile_x - 0.5) > abs(tile_y - 0.5))
			{
				normal_x = tile_x < 0.5 ? -1 : 1;
				normal_y = 0;
			}
			else
			{
				normal_x = 0;
				normal_y = tile_y < 0.5 ? -1 : 1;
			}
			
			return true;
		
		case TileShape::Big1:
			normal_x = n2y;
			normal_y = -n2x;
			return tile_y >= tile_x * 0.5;
		case TileShape::Small1:
			normal_x = n2y;
			normal_y = -n2x;
			return tile_y >= 0.5 + tile_x * 0.5;
		
		case TileShape::Big2:
			normal_x = n2x;
			normal_y = n2y;
			return tile_x <= 1 - tile_y * 0.5;
		case TileShape::Small2:
			normal_x = n2x;
			normal_y = n2y;
			return tile_x <= 0.5 - tile_y * 0.5;
		
		case TileShape::Big3:
			normal_x = -n2y;
			normal_y = n2x;
			return tile_y <= 0.5 + tile_x * 0.5;
		case TileShape::Small3:
			normal_x = -n2y;
			normal_y = n2x;
			return tile_y <= tile_x * 0.5;
		
		case TileShape::Big4:
			normal_x = -n2x;
			normal_y = -n2y;
			return tile_x >= 0.5 - tile_y * 0.5;
		case TileShape::Small4:
			normal_x = -n2x;
			normal_y = -n2y;
			return tile_x >= 1 - tile_y * 0.5;
			
		case TileShape::Big5:
			normal_x = -n2y;
			normal_y = -n2x;
			return tile_y >= 0.5 - tile_x * 0.5;
		case TileShape::Small5:
			normal_x = -n2y;
			normal_y = -n2x;
			return tile_y >= 1 - tile_x * 0.5;
		
		case TileShape::Big6:
			normal_x = -n2x;
			normal_y = n2y;
			return tile_x >= tile_y * 0.5;
		case TileShape::Small6:
			normal_x = -n2x;
			normal_y = n2y;
			return tile_x >= 0.5 + tile_y * 0.5;
		
		case TileShape::Big7:
			normal_x = n2y;
			normal_y = n2x;
			return tile_y <= 1 - tile_x * 0.5;
		case TileShape::Small7:
			normal_x = n2y;
			normal_y = n2x;
			return tile_y <= 0.5 - tile_x * 0.5;
		
		case TileShape::Big8:
			normal_x = n2x;
			normal_y = -n2y;
			return tile_x <= 0.5 + tile_y * 0.5;
		case TileShape::Small8:
			normal_x = n2x;
			normal_y = -n2y;
			return tile_x <= tile_y * 0.5;
		
		case TileShape::HalfA:
			normal_x = n1;
			normal_y = -n1;
			return tile_x <= tile_y;
		case TileShape::HalfB:
			normal_x = n1;
			normal_y = n1;
			return tile_x <= 1 - tile_y;
		case TileShape::HalfC:
			normal_x = -n1;
			normal_y = n1;
			return tile_x >= tile_y;
		case TileShape::HalfD:
			normal_x = -n1;
			normal_y = -n1;
			return tile_x >= 1 - tile_y;
	}
	
	return false;
}

uint8 get_tile_edge(tileinfo@ tile, int side)
{
	switch(side)
	{
		case 0: // Top
			return tile.edge_top();
		case 1: // Bottom
			return tile.edge_bottom();
		case 2: // Left
			return tile.edge_left();
		case 3: // Right
			return tile.edge_right();
	}
	
	return 0;
}

void set_tile_edge(tileinfo@ tile, int side, uint8 edge_bits)
{
	switch(side)
	{
		case 0: // Top
			tile.edge_top(edge_bits);
			break;
		case 1: // Bottom
			tile.edge_bottom(edge_bits);
			break;
		case 2: // Left
			tile.edge_left(edge_bits);
			break;
		case 3: // Right
			tile.edge_right(edge_bits);
			break;
	}
}

bool is_valid_edge(int shape, int side)
{
	switch(shape)
	{
		// All edges
		case TileShape::Full:
			return true;
		
		// All but the right edge
		case TileShape::Big1:
		case TileShape::Big7:
		case TileShape::HalfA:
		case TileShape::HalfB:
			return side != TileEdge::Right;
		
		// The top and bottom edges
		case TileShape::Small1:
		case TileShape::Small3:
		case TileShape::Small5:
		case TileShape::Small7:
			return side == TileEdge::Top || side == TileEdge::Bottom;
		
		// All but the bottom edge
		case TileShape::Big2:
		case TileShape::Big6:
			return side != TileEdge::Bottom;
		
		// The left and right edges
		case TileShape::Small2:
		case TileShape::Small4:
		case TileShape::Small6:
		case TileShape::Small8:
			return side == TileEdge::Left || side == TileEdge::Right;
		
		// All but the left edge
		case TileShape::Big3:
		case TileShape::Big5:
		case TileShape::HalfC:
		case TileShape::HalfD:
			return side != TileEdge::Left;
		
		// All but the top edge
		case TileShape::Big4:
		case TileShape::Big8:
			return side != TileEdge::Top;
	}
	
	return false;
}

bool is_full_edge(int shape, int side)
{
	switch(side)
	{
		case TileEdge::Top:
			return	shape == TileShape::Full ||
					shape == TileShape::Big2 ||
					shape == TileShape::Big3 ||
					shape == TileShape::Big6 ||
					shape == TileShape::Big7 ||
					shape == TileShape::Small3 ||
					shape == TileShape::Small7 ||
					shape == TileShape::HalfB ||
					shape == TileShape::HalfC;
			
		case TileEdge::Bottom:
			return	shape == TileShape::Full ||
					shape == TileShape::Big1 ||
					shape == TileShape::Big4 ||
					shape == TileShape::Big5 ||
					shape == TileShape::Big8 ||
					shape == TileShape::Small1 ||
					shape == TileShape::Small5 ||
					shape == TileShape::HalfA ||
					shape == TileShape::HalfD;
			
		case TileEdge::Left:
			return	shape == TileShape::Full ||
					shape == TileShape::Big1 ||
					shape == TileShape::Big2 ||
					shape == TileShape::Big7 ||
					shape == TileShape::Big8 ||
					shape == TileShape::Small2 ||
					shape == TileShape::Small8 ||
					shape == TileShape::HalfA ||
					shape == TileShape::HalfB;
			
		case TileEdge::Right:
			return	shape == TileShape::Full ||
					shape == TileShape::Big3 ||
					shape == TileShape::Big4 ||
					shape == TileShape::Big5 ||
					shape == TileShape::Big6 ||
					shape == TileShape::Small4 ||
					shape == TileShape::Small6 ||
					shape == TileShape::HalfC ||
					shape == TileShape::HalfD;
	}
	
	return false;
}