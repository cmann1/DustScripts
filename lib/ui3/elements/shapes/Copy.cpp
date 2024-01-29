#include 'Shape.cpp';

namespace CopyShape { const string TYPE_NAME = 'PlayShape'; }

class CopyShape : Shape
{
	
	CopyShape(UI@ ui)
	{
		super(ui);
	}
	
	CopyShape(UI@ ui, const uint colour)
	{
		super(ui, colour);
	}
	
	string element_type { get const override { return CopyShape::TYPE_NAME; } }
	
	protected void init() override
	{
		_width  = _set_width  = 10;
		_height = _set_height = 12;
		_graphic_width  = 10;
		_graphic_height = 12;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_draw_colour();
		
		float x, y, size_x, size_y, draw_scale_x, draw_scale_y;
		draw_init(x, y, size_x, size_y, draw_scale_x, draw_scale_y);
		
		style.draw_rectangle(x - 5, y - 3, x + 2, y + 6, 0, clr);
		style.draw_rectangle(x - 2, y - 6, x + 5, y - 4, 0, clr);
		style.draw_rectangle(x + 3, y - 4, x + 5, y + 4, 0, clr);
	}
	
}
