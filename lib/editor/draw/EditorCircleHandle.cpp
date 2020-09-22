class EditorCircleHandle
{
	
	float x, y;
	float radius;
	float thickness;
	int layer;
	uint colour;
	
	void draw(BaseEditorScript@ script)
	{
		float x, y;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x, this.y, layer, 22, x, y);
		
		draw_circle(script.g, x, y, radius, script.ed_circle_segments(radius), 22, 20, thickness * script.ed_zoom, colour);
	}
	
}