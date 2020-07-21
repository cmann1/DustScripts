/**
 * @class Graphics
 * @brief Needed because hud drawing is currently broken in the built in canvas class
 */
class Graphics
{
	
	scene@ g;
	
	uint layer;
	uint sub_layer;
	
	Graphics(scene@ g, uint layer, uint sub_layer)
	{
		@this.g = g;
		this.layer = layer;
		this.sub_layer = sub_layer;
	}
	
	void draw_rectangle(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in rotation, const uint &in colour) const {}
	
	void draw_glass(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in rotation, const uint &in colour) const {}

	void draw_gradient(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const uint &in c00, const uint &in c10, const uint &in c11, const uint &in c01) const {}

	void draw_line(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in width, uint colour) const {}

	void draw_quad(
		const bool &in is_glass,
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in x3, const float &in y3, const float &in x4, const float &in y4,
		const uint &in c1, const uint &in c2, const uint &in c3, const uint &in c4) const {}

	void draw_sprite(
		sprites@ sprite,
		const string &in sprite_name, const uint &in frame, const uint &in palette,
		const float &in x, const float &in y, const float &in rotation,
		const float &in scale_x, const float &in scale_y,
		const uint &in colour) const {}
	
}