#include 'Shape.cpp';

class ShapeCross : Shape
{
	
	float thickness;
	
	ShapeCross(UI@ ui, float thickness = 3, uint colour = 0xCCFFFFFF)
	{
		super(ui, colour);
		
		this.thickness = thickness;
	}
	
	void draw(scene@ g, Rect rect)
	{
		float centre_x = rect.centre_x;
		float centre_y = rect.centre_y;
		float w = thickness * 0.5;
		g.draw_rectangle_hud(22, 22, rect.x1, centre_y - w, rect.x2, centre_y + w, 45, colour);
		g.draw_rectangle_hud(22, 22, centre_x - w, rect.y1, centre_x + w, rect.y2, 45, colour);
	}
	
}