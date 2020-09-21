class EditorBox
{
	
	float x1, y1;
	float x2, y2;
	bool draw_snap_tiles;
	float thickness;
	int layer;
	uint colour;
	
	void draw(scene@ g, const float ed_view_x, const float ed_view_y, const float ed_zoom)
	{
		float x1, y1;
		float x2, y2;
		transform_layer_position(g, ed_view_x, ed_view_y, this.x1, this.y1, layer, 22, x1, y1);
		transform_layer_position(g, ed_view_x, ed_view_y, this.x2, this.y2, layer, 22, x2, y2);
		
		if(draw_snap_tiles)
		{
			x1 = floor(x1 / 48) * 48;
			y1 = floor(y1 / 48) * 48;
			x2 = floor(x2 / 48 + 1) * 48;
			y2 = floor(y2 / 48 + 1) * 48;
		}
		
		outline_rect(g, 22, 20, x1, y1, x2, y2, thickness * ed_zoom, colour);
	}
	
}