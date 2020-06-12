#include '../std.cpp';
#include '../math/math.cpp';
#include '../drawing/common.cpp';
#include 'closest_point_on_tile.cpp';

bool closest_tile(scene@ g, float x, float y, float radius, uint layer, int &out tile_x, int &out tile_y, float &out out_x, float &out out_y, float &out normal_x, float &out normal_y)
{
	int start_x = floor_int((x - radius - 1) * PIXEL2TILE);
	int start_y = floor_int((y - radius - 1) * PIXEL2TILE);
	int end_x = ceil_int((x + radius + 1) * PIXEL2TILE);
	int end_y = ceil_int((y + radius + 1) * PIXEL2TILE);
	
	outline_rect(g,
		start_x * TILE2PIXEL, start_y * TILE2PIXEL,
		end_x * TILE2PIXEL, end_y * TILE2PIXEL,
		layer, 24, 1);
	
	float closest_dist = MAX_FLOAT;
	bool found_tile = false;
	
	for(int tx = start_x; tx < end_x; tx++)
	{
		for(int ty = start_y; ty < end_y; ty++)
		{
			tileinfo@ tile = g.get_tile(tx, ty, layer);
			
			if(!tile.solid())
				continue;
			
			const int tile_type = tile.type();
			float check_normal_x, check_normal_y;
			
//			if(point_in_tile(g, x, y, tx, ty, tile_type, check_normal_x, check_normal_y, layer))
//				continue;
				
			float check_x, check_y;
			
			if(!closest_point_on_tile(x, y, tx, ty, tile_type, @tile, check_x, check_y, check_normal_x, check_normal_y))
				continue;
			
			float check_dist = dist_sqr(x, y, check_x, check_y);
			
			if(check_dist < closest_dist)
			{
				tile_x = tx;
				tile_y = ty;
				out_x = check_x;
				out_y = check_y;
				normal_x = check_normal_x;
				normal_y = check_normal_y;
				closest_dist = check_dist;
				found_tile = true;
			}
		}
	}
	
	return found_tile;
}