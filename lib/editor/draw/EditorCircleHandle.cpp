class EditorCircleHandle
{
	
	float x, y;
	float radius;
	float thickness;
	int layer;
	uint colour;
	
	void draw(scene@ g, const float ed_view_x, const float ed_view_y, const float ed_zoom)
	{
		float x, y;
		transform_layer_position(g, ed_view_x, ed_view_y, this.x, this.y, layer, 22, x, y);
		
		draw_circle(g, x, y, radius, 32, 22, 20, thickness * ed_zoom, colour);
	}
	
}