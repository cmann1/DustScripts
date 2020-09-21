class EditorArrow
{
	
	float x1, y1;
	float x2, y2;
	float thickness;
	float head_size;
	float head_position;
	int layer;
	uint colour;
	
	void draw(scene@ g, const float ed_view_x, const float ed_view_y, const float ed_zoom)
	{
		float x1, y1;
		float x2, y2;
		transform_layer_position(g, ed_view_x, ed_view_y, this.x1, this.y1, this.layer, 22, x1, y1);
		transform_layer_position(g, ed_view_x, ed_view_y, this.x2, this.y2, this.layer, 22, x2, y2);
		
		draw_arrow(g, 22, 20,
			x1, y1,
			x2, y2,
			thickness * ed_zoom, head_size * ed_zoom, head_position, colour);
	}
	
}