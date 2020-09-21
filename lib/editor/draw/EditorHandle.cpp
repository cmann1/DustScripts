class EditorHandle
{
	
	float x, y;
	int layer;
	uint colour;
	float rotation;
	
	void draw(scene@ g, const float ed_view_x, const float ed_view_y, const float ed_handle_size)
	{
		float x, y;
		transform_layer_position(g, ed_view_x, ed_view_y, this.x, this.y, layer, 22, x, y);
		
		g.draw_rectangle_world(22, 22,
			x - ed_handle_size, y - ed_handle_size,
			x + ed_handle_size, y + ed_handle_size, rotation, colour);
	}
	
}