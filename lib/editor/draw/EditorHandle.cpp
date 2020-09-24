class EditorHandle
{
	
	float x, y;
	int layer;
	uint colour;
	float rotation;
	bool circle;
	
	void draw(BaseEditorScript@ script)
	{
		float x, y;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x, this.y, layer, 22, x, y);
		
		if(!circle)
		{
			script.g.draw_rectangle_world(22, 22,
				x - script.ed_handle_size, y - script.ed_handle_size,
				x + script.ed_handle_size, y + script.ed_handle_size, rotation, colour);
		}
		else
		{
			fill_circle(script.g, 22, 22, x, y, script.ed_handle_size, 16, colour, colour);
		}
	}
	
}