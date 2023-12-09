#include '../tiles/common.cpp';
#include '../tiles/get_tile_edge_points.cpp';
#include '../tiles/get_tile_quad.cpp';

void draw_tile_shape(int shape, scene@ g, int layer, int sub_layer, float x, float y, float scale_x, float scale_y, uint fill, uint outline)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
	get_tile_quad(shape, x1, y1, x2, y2, x3, y3, x4, y4);
	g.draw_quad_world(
		layer, sub_layer, false,
		x + scale_x * x1, y + scale_y * y1,
		x + scale_x * x2, y + scale_y * y2,
		x + scale_x * x3, y + scale_y * y3,
		x + scale_x * x4, y + scale_y * y4,
		fill, fill, fill, fill
	);
	
	for(int edge = 0; edge < 4; ++edge)
	{
		if(!get_edge_points(shape, edge, x1, y1, x2, y2))
			continue;
		
		g.draw_line_world(
			layer, sub_layer,
			x + scale_x * x1, y + scale_y * y1,
			x + scale_x * x2, y + scale_y * y2,
			2, outline
		);
	}
}

void fill_tile(scene@ g, const int layer, const int sub_layer, const int shape, const float x, const float y, const uint colour)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
	get_tile_quad(shape, x1, y1, x2, y2, x3, y3, x4, y4);
	g.draw_quad_world(
		layer, sub_layer, false,
		x + x1, y + y1,
		x + x2, y + y2,
		x + x3, y + y3,
		x + x4, y + y4,
		colour, colour, colour, colour);
}

void outline_tile(scene@ g, const int layer, const int sub_layer, const int shape, const float x, const float y, const float width, const uint colour)
{
	for(int edge = 0; edge < 4; edge++)
	{
		float x1, y1, x2, y2;
		if(!get_edge_points(shape, edge, x1, y1, x2, y2))
			continue;
		
		g.draw_line_world(
			layer, sub_layer,
			x + x1, y + y1,
			x + x2, y + y2,
			width, colour);
	}
}
