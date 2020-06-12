#include '../math/math.cpp';
#include '../math/Line.cpp';
#include 'common.cpp';
#include 'get_tile_edge_points.cpp';

bool closest_point_on_tile(float x, float y, int tile_x, int tile_y, int type, tileinfo@ tile, float &out out_x, float &out out_y, float &out normal_x, float &out normal_y)
{
	const float tx = tile_x * TILE2PIXEL;
	const float ty = tile_y * TILE2PIXEL;
	const float local_x = x - tx;
	const float local_y = y - ty;
	Line line;
	bool result = false;
	float closest_dist = MAX_FLOAT;
	
	for(int side = 0; side < 4; side++)
	{
		if(!is_valid_edge(type, side))
			continue;
		
		if(get_tile_edge(tile, side) & 0x8 == 0)
			continue;
		
		switch(side)
		{
			case 0:
				get_tile_top_edge_points(type, line.x1, line.y1, line.x2, line.y2);
				break;
			case 1:
				get_tile_bottom_edge_points(type, line.x1, line.y1, line.x2, line.y2);
				break;
			case 2:
				get_tile_left_edge_points(type, line.x1, line.y1, line.x2, line.y2);
				break;
			case 3:
				get_tile_right_edge_points(type, line.x1, line.y1, line.x2, line.y2);
				break;
		}
		
		float check_x, check_y;
		line.closest_point(local_x, local_y, check_x, check_y);
		
		const float dist = dist_sqr(x, y, tx + check_x, ty + check_y);
		
		if(dist < closest_dist)
		{
			out_x = tx + check_x;
			out_y = ty + check_y;
			closest_dist = dist;
			normal_x = line.y2 - line.y1;
			normal_y = -(line.x2 - line.x1);
			result = true;
			
//			get_scene().draw_line_world(22, 22, tx + line.x1, ty + line.y1, tx + line.x2, ty + line.y2, 2, 0x8800ff00);
		}
//		else
//		{
//			get_scene().draw_line_world(22, 22, tx + line.x1, ty + line.y1, tx + line.x2, ty + line.y2, 2, 0x88ff0000);
//		}
	}
	
	if(result)
	{
		const float l = sqrt(normal_x * normal_x + normal_y * normal_y);
		normal_x /= l;
		normal_y /= l;
	}
	
	return result;
}