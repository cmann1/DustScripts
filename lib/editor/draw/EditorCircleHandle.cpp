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
		
		drawing::circle(script.g, 22, 20, x, y, radius, script.ed_circle_segments(radius), thickness * script.ed_zoom, colour);
	}
	
}