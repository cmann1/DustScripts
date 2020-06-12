#include 'std.cpp';
#include 'TileShape.cpp';

bool point_in_tile(scene@ g, float x, float y, int tile_ix, int tile_iy, int type, float &out normal_x, float &out normal_y, int layer=19)
{
	const float tile_x = (x - float(tile_ix * TILE2PIXEL)) * PIXEL2TILE;
	const float tile_y = (y - float(tile_iy * TILE2PIXEL)) * PIXEL2TILE;
	
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
	if(shape == TileShape::Full)
		return true;
	
	switch(side)
	{
		case 0: // Top
			return	shape == TileShape::Big2 or
					shape == TileShape::Big3 or
					shape == TileShape::Big6 or
					shape == TileShape::Big7 or
					shape == TileShape::Small3 or
					shape == TileShape::Small7 or
					shape == TileShape::HalfB or
					shape == TileShape::HalfC;
		case 1: // Bottom
			return	shape == TileShape::Big1 or
					shape == TileShape::Big4 or
					shape == TileShape::Big5 or
					shape == TileShape::Big8 or
					shape == TileShape::Small1 or
					shape == TileShape::Small5 or
					shape == TileShape::HalfA or
					shape == TileShape::HalfD;
		case 2: // Left
			return	shape == TileShape::Big1 or
					shape == TileShape::Big2 or
					shape == TileShape::Big7 or
					shape == TileShape::Big8 or
					shape == TileShape::Small2 or
					shape == TileShape::Small8 or
					shape == TileShape::HalfA or
					shape == TileShape::HalfB;
		case 3: // Right
			return	shape == TileShape::Big3 or
					shape == TileShape::Big4 or
					shape == TileShape::Big5 or
					shape == TileShape::Big6 or
					shape == TileShape::Small4 or
					shape == TileShape::Small6 or
					shape == TileShape::HalfC or
					shape == TileShape::HalfD;
	}
	
	return false;
}