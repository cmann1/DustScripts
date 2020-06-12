#include 'TileShape.cpp';

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