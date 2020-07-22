class Graphics
{
	/**
	 * @class Graphics
	 * @brief Needed because hud drawing is currently broken in the built in canvas class
	 */
	
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
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, const uint colour) const {}
	
	void draw_glass(
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, const uint colour) const {}

	void draw_gradient(
		const float x1, const float y1, const float x2, const float y2,
		const uint c00, const uint c10, const uint c11, const uint c01) const {}

	void draw_line(
		const float x1, const float y1, const float x2, const float y2,
		const float width, uint colour) const {}

	void draw_quad(
		const bool is_glass,
		const float x1, const float y1, const float x2, const float y2,
		const float x3, const float y3, const float x4, const float y4,
		const uint c1, const uint c2, const uint c3, const uint c4) const {}

	void draw_sprite(
		sprites@ sprite,
		const string sprite_name, const uint frame, const uint palette,
		const float x, const float y, const float rotation,
		const float scale_x, const float scale_y,
		const uint colour) const {}

	void draw_text(
		textfield@ text_field,
		const float x, const float y,
		const float scale_x, const float scale_y,
		const float rotation) const {}
	
}