#include '../../drawing/circle.cpp';

class EditorArc
{
	
	float x, y;
	float radius;
	float angle_min, angle_max;
	float thickness;
	int layer;
	uint colour;
	
	void draw(BaseEditorScript@ script)
	{
		float x, y;
		transform_layer_position(script.g, script.ed_view_x, script.ed_view_y, this.x, this.y, this.layer, 22, x, y);
		
		drawing::arc(script.g, 22, 20,
			x, y, radius, radius,
			angle_min, angle_max, max(script.ed_circle_segments(radius), 3),
			thickness * script.ed_zoom, colour);
	}
	
}
