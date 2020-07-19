#include 'math/math.cpp';

/**
 * Gets the scaling factor from one layer to another
 */
float get_layer_scale(scene@ g, int from_layer, int to_layer)
{
	if(from_layer < 0) from_layer = 0;
	else if(from_layer > 12) from_layer = 12;
	
	if(to_layer < 0) to_layer = 0;
	else if(to_layer > 12) to_layer = 12;
	
	if(from_layer == to_layer)
		return 1;
	
	return g.layer_scale(from_layer) / g.layer_scale(to_layer);
}

/**
 * Takes a position in **from_layer** coordinates and returns the matching position in **to_layer** based on the view/camera position.
 */
void transform_layer_position(scene@ g, float view_x, float view_y, float x, float y, int from_layer, int to_layer, float &out out_x, float &out out_y)
{
	float scale = get_layer_scale(g, from_layer, to_layer);
	
	float dx = (x - view_x) * scale;
	float dy = (y - view_y) * scale;
	
	out_x = view_x + dx;
	out_y = view_y + dy;
}