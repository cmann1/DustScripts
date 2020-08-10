#include '../../../math/math.cpp';
#include 'Shape.cpp';

class Arrow : Shape
{
	
	Arrow(UI@ ui)
	{
		super(ui);
	}
	
	Arrow(UI@ ui, const uint colour)
	{
		super(ui, colour);
	}
	
	string element_type { get const override { return 'ArrowShape'; } }
	
	protected void init() override
	{
		_width  = _set_width  = 10;
		_height = _set_height = 10;
		_graphic_width  = 5;
		_graphic_height = 10;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_colour();
		const float x = draw_x;
		const float y = draw_y;
		const float angle = rotation * DEG2RAD;
		
		float
			x2 = 5 * draw_scale_x,
			y2 = 5 * draw_scale_y,
			x4 = 0 * draw_scale_x,
			y4 = 10 * draw_scale_y;
		rotate(x2, y2, angle, x2, y2);
		rotate(x4, y4, angle, x4, y4);
		
		style.draw_quad(false,
			ui._pixel_round(x),      ui._pixel_round(y),
			ui._pixel_round(x + x2), ui._pixel_round(y + y2),
			ui._pixel_round(x + x2), ui._pixel_round(y + y2),
			ui._pixel_round(x + x4), ui._pixel_round(y + y4), clr, clr, clr, clr);
	}
	
}