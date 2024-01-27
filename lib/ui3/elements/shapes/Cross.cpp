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
		
		float x, y, size_x, size_y, draw_scale_x, draw_scale_y;
		draw_init(x, y, size_x, size_y, draw_scale_x, draw_scale_y);
		
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
