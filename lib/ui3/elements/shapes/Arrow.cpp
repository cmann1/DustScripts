#include '../../../math/math.cpp';
#include 'Shape.cpp';

namespace Arrow { const string TYPE_NAME = 'ArrowShape'; }

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
	
	string element_type { get const override { return Arrow::TYPE_NAME; } }
	
	protected void init() override
	{
		_width  = _set_width  = 10;
		_height = _set_height = 10;
		_graphic_width  = 5;
		_graphic_height = 10;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_draw_colour();
		const float x = x1 + draw_x;
		const float y = y1 + draw_y;
		const float angle = _rotation * DEG2RAD;
		
		const float draw_scale_x = is_transposed ? this.draw_scale_y : this.draw_scale_x;
		const float draw_scale_y = is_transposed ? this.draw_scale_x : this.draw_scale_y;
		
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
