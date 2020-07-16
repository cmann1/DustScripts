#include 'math/math.cpp';

const float PARALLAX_LAYER_SCALE = 0.05;
const float PARALLAX_LAYER_MAX = 1 / (1 - PARALLAX_LAYER_SCALE * 6) - 1;
const float BACKDROP_LAYER_0_SCALE = 0.4;
const float PARALLAX_SCALE_DIFF = 1 / 0.7;
const float BACKDROP_TO_FOREGROUND_SCALE = 4;
const float BACKDROP_TO_PARALLAX_SCALE = BACKDROP_TO_FOREGROUND_SCALE / PARALLAX_SCALE_DIFF;

/**
 * Takes a position in **from_layer** coordinates and returns the matching position in **to_layer** based on the view/camera position.
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
	from_layer = clamp(from_layer, 0, 12);
	to_layer = clamp(to_layer, 0, 12);
	
	if(from_layer == to_layer)
		return 1;
	
	float scale = 1;
	
	/*
	 * Convert between backdrop layers
	 */
	
	if(from_layer < 6 || to_layer < 6)
	{
		/*
		 * If one of the layers is not a backdrop layer (0-5) first convert to layer 5
		 */
		 
		const int backdrop_from_layer = from_layer > 5 ? 5 : from_layer;
		const int backdrop_to_layer = to_layer > 5 ? 5 : to_layer;
		
		scale = (
			(backdrop_from_layer == 0 ? BACKDROP_LAYER_0_SCALE : backdrop_from_layer) /
			(backdrop_to_layer == 0 ? BACKDROP_LAYER_0_SCALE : backdrop_to_layer)
		);
		
		// Conversion between backdrop layers, nothing more to do.
		if(from_layer < 6 && to_layer < 6)
			return scale;
		
		/*
		 * Conversion to/from foreground layers.
		 * The scale difference from layer 5 to foreground layers is 4 (BACKDROP_TO_FOREGROUND_SCALE)
		 */
		if(from_layer > 11)
			return scale * BACKDROP_TO_FOREGROUND_SCALE;
		
		/*
		 * Conversion to/from parallax layers
		 * Take the backdrop>forground and paralax>foreground scale into account to convert from layer 5 to 6
		 */
		 
		if(from_layer > 5)
			scale *= BACKDROP_TO_PARALLAX_SCALE;
		else
			scale /= BACKDROP_TO_PARALLAX_SCALE;
		
		if(from_layer < 6)
			from_layer = 6;
		else
			to_layer = 6;
		
		/*
		 * Now that both layers are parallax, continue below normally.
		 */
	}
	
	/*
	 * Convert to and from foreground to parallax layers
	 */
	
	const int diff = to_layer - from_layer;
	const int dir = sign(diff);
	
	return scale * (diff > 0
		? 1 - PARALLAX_LAYER_SCALE * diff
		: 1 - PARALLAX_LAYER_MAX * (diff / 6.0));
}