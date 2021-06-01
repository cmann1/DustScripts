#include 'Shape.cpp';

namespace Cross { const string TYPE_NAME = 'CrossShape'; }

class Cross : Shape
{
	
	float thickness = 1.5;
	
	Cross(UI@ ui)
	{
		super(ui);
	}
	
	Cross(UI@ ui, const uint colour)
	{
		super(ui, colour);
	}
	
	string element_type { get const override { return Cross::TYPE_NAME; } }
	
	protected void init() override
	{
		_width  = _set_width  = 12;
		_height = _set_height = 12;
		_graphic_width  = 12;
		_graphic_height = 12;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_draw_colour();
		
		const float draw_scale_x = is_transposed ? this.draw_scale_y : this.draw_scale_x;
		const float draw_scale_y = is_transposed ? this.draw_scale_x : this.draw_scale_y;
		
		const float size_x = _graphic_width  * draw_scale_x * 0.5;
		const float size_y = _graphic_height * draw_scale_y * 0.5;
		const float x = ui._pixel_floor(x1 + draw_x + size_x);
		const float y = ui._pixel_floor(y1 + draw_y + size_y);
		
		style.draw_rectangle(
			x - size_x - 1, y - thickness * draw_scale_x,
			x + size_x + 1, y + thickness * draw_scale_y,
			_rotation + 45, clr);
		style.draw_rectangle(
			x - size_x - 1, y - thickness * draw_scale_x,
			x + size_x + 1, y + thickness * draw_scale_y,
			_rotation - 45, clr);
	}
	
}
