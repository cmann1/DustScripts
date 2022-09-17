class EditorText
{
	
	float x, y;
	string text;
	float scale;
	float rotation;
	int layer;
	uint colour;
	int align_h;
	int align_v;
	
	void draw(BaseEditorScript@ script)
	{
		float x, y;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x, this.y, this.layer, 22, x, y);
		
		script.text_field.text(text);
		script.text_field.colour(colour);
		script.text_field.align_horizontal(align_h);
		script.text_field.align_vertical(align_v);
		
		shadowed_text_world(script.text_field, 22, 20,
			x, y,
			scale * script.ed_zoom, scale * script.ed_zoom, rotation, 0x77000000, 2 * script.ed_zoom, 2 * script.ed_zoom);
	}
	
}
