class EditorHandle
{
	
	float x, y;
	int layer;
	uint colour;
	float rotation;
	float size;
	bool circle;
	
	void draw(BaseEditorScript@ script)
	{
		float x, y;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x, this.y, layer, 22, x, y);
		const float size = this.size <= 0 ? script.ed_handle_size : this.size * script.ed_zoom;
		
		if(!circle)
		{
			script.g.draw_rectangle_world(22, 22,
				x - size, y - size,
				x + size, y + size, rotation, colour);
		}
		else
		{
			fill_circle(script.g, 22, 22, x, y, size, 16, colour, colour);
		}
	}
	
}