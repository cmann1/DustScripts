#include 'Shape.cpp';

namespace StarShape { const string TYPE_NAME = 'StarShape'; }

class StarShape : Shape
{
	
	int points;
	float inner_radius;
	float start_angle = -PI * 0.5;
	
	StarShape(UI@ ui, const int points = 5, float inner_radius = 0.5)
	{
		super(ui);
		this.points = points;
		this.inner_radius = inner_radius;
	}
	
	StarShape(UI@ ui, const int colour, const int points = 5, float inner_radius = 0.5)
	{
		super(ui, colour);
		this.points = points;
		this.inner_radius = inner_radius;
	}
	
	string element_type { get const override { return StarShape::TYPE_NAME; } }
	
	protected void init() override
	{
		_width  = _set_width  = 18;
		_height = _set_height = 18;
		_graphic_width  = 18;
		_graphic_height = 18;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_draw_colour();
		
		float x, y, size_x, size_y, draw_scale_x, draw_scale_y;
		draw_init(x, y, size_x, size_y, draw_scale_x, draw_scale_y);
		
		const int points = clamp(this.points, 4, 50);
		const float inner_angle = PI / points;
		for(int i = 0; i < points; i++)
		{
			const float angle = start_angle + (float(i) / points) * PI * 2;
			style.draw_quad(false,
				x, y,
				x + cos(angle + inner_angle) * size_x * inner_radius, y + sin(angle + inner_angle) * size_y * inner_radius,
				x + cos(angle) * size_x, y + sin(angle) * size_y,
				x + cos(angle - inner_angle) * size_x * inner_radius, y + sin(angle - inner_angle) * size_y * inner_radius,
				clr, clr, clr, clr);
		}
	}
	
}
