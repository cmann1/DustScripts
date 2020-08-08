#include 'Shape.cpp';

class Cross : Shape
{
	
	Cross(UI@ ui)
	{
		super(ui);
	}
	
	Cross(UI@ ui, const uint colour)
	{
		super(ui, colour);
	}
	
	string element_type { get const override { return 'CrossShape'; } }
	
	protected void init() override
	{
		_width  = _set_width  = 10;
		_height = _set_height = 10;
		_graphic_width  = 10;
		_graphic_height = 10;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_colour();
		const float x = draw_x;
		const float y = draw_y;
		
		style.draw_rectangle(x - 1, y + 4, x + 11, y + 6, rotation + 45, clr);
		style.draw_rectangle(x - 1, y + 4, x + 11, y + 6, rotation - 45, clr);
	}
	
}