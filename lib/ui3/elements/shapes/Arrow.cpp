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
		_width  = _set_width  = 8;
		_height = _set_height = 8;
		_graphic_width  = 4;
		_graphic_height = 8;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = _use_custom_colour ? _colour : style.text_clr;
		const float x = draw_x;
		const float y = draw_y;
		const float angle = rotation * DEG2RAD;
		
		float
			x2 = 4 * draw_scale_x,
			y2 = 4 * draw_scale_y,
			x4 = 0 * draw_scale_x,
			y4 = 8 * draw_scale_y;
		rotate(x2, y2, angle, x2, y2);
		rotate(x4, y4, angle, x4, y4);
		
		style.draw_quad(false,
			x, y,
			x + x2, y + y2,
			x + x2, y + y2,
			x + x4, y + y4, clr, clr, clr, clr);
	}
	
}