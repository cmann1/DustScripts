#include 'Shape.cpp';

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
	
	string element_type { get const override { return 'CrossShape'; } }
	
	protected void init() override
	{
		_width  = _set_width  = 12;
		_height = _set_height = 12;
		_graphic_width  = 12;
		_graphic_height = 12;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_colour();
		
		const float size_x = _graphic_width  * draw_scale_x * 0.5;
		const float size_y = _graphic_height * draw_scale_y * 0.5;
		const float x = ui._pixel_floor(draw_x + size_x);
		const float y = ui._pixel_floor(draw_y + size_y);
		
		style.draw_rectangle(
			x - size_x - 1, y - thickness * draw_scale_x,
			x + size_x + 1, y + thickness * draw_scale_y,
			rotation + 45, clr);
		style.draw_rectangle(
			x - size_x - 1, y - thickness * draw_scale_x,
			x + size_x + 1, y + thickness * draw_scale_y,
			rotation - 45, clr);
	}
	
}