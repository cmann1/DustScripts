class EditorArrow
{
	
	float x1, y1;
	float x2, y2;
	float thickness;
	float head_size;
	float head_position;
	int layer;
	uint colour;
	
	void draw(BaseEditorScript@ script)
	{
		float x1, y1;
		float x2, y2;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x1, this.y1, this.layer, 22, x1, y1);
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x2, this.y2, this.layer, 22, x2, y2);
		
		draw_arrow(script.g, 22, 20,
			x1, y1,
			x2, y2,
			thickness * script.ed_zoom, head_size * script.ed_zoom, head_position, colour);
	}
	
}