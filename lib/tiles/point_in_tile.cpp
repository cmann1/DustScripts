#include '../std.cpp';

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