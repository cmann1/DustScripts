const float PARALLAX_LAYER_SCALE = 0.05;
const float PARALLAX_LAYER_MAX = 1 / (1 - PARALLAX_LAYER_SCALE * 6) - 1;

/**
 * Takes a position relative to **from_layer** and returns the matching position in **to_layer** based on the view/camera position.
 * Layers 0-5 are not supported
 */
void transform_layer_position(float view_x, float view_y, float x, float y, int from_layer, int to_layer, float &out out_x, float &out out_y)
{
	float scale = get_layer_scale(from_layer, to_layer);
	
	float dx = (x - view_x) * scale;
	float dy = (y - view_y) * scale;
	
	out_x = view_x + dx;
	out_y = view_y + dy;
}

/**
 * Gets the scaling factor from one layer to another
 */
float get_layer_scale(int from_layer, int to_layer)
{
	from_layer = clamp(from_layer, 0, 22);
	to_layer = clamp(to_layer, 0, 22);
	
	if(to_layer > 12 && from_layer < 12)
	{
		to_layer = 12;
	}
	
	if(from_layer > 12 && to_layer < 12)
	{
		from_layer = 12;
	}
	
	int diff = to_layer - from_layer;
	int dir = sign(diff);
	
	if(dir == 0 || from_layer >= 12 && to_layer >= 12)
	{
		return 1;
	}
	
	return diff > 0
		? 1 - PARALLAX_LAYER_SCALE * diff
		: 1 - PARALLAX_LAYER_MAX * (diff / 6.0);
}