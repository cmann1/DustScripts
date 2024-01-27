#include 'Shape.cpp';

namespace PlayShape { const string TYPE_NAME = 'PlayShape'; }

class PlayShape : Shape
{
	
	int shape = PlayShape::Play;
	
	PlayShape(UI@ ui, const int shape = PlayShape::Play)
	{
		super(ui);
		this.shape = shape;
	}
	
	PlayShape(UI@ ui, const uint colour, const int shape = PlayShape::Play)
	{
		super(ui, colour);
		this.shape = shape;
	}
	
	string element_type { get const override { return Cross::TYPE_NAME; } }
	
	protected void init() override
	{
		_width  = _set_width  = 12;
		_height = _set_height = 14;
		_graphic_width  = 12;
		_graphic_height = 12;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const uint clr = get_draw_colour();
		
		float x, y, size_x, size_y, draw_scale_x, draw_scale_y;
		draw_init(x, y, size_x, size_y, draw_scale_x, draw_scale_y);
		
		switch(shape)
		{
			case PlayShape::Pause:
			{
				style.draw_rectangle(x - 6, y - 6, x - 1, y + 6, 0, clr);
				style.draw_rectangle(x + 1, y - 6, x + 6, y + 6, 0, clr);
			} break;
			case PlayShape::Stop:
			{
				style.draw_rectangle(x - 6, y - 6, x + 6, y + 6, 0, clr);
			} break;
			case PlayShape::Repeat:
			{
				style.draw_rectangle(x - 6, y - 5, x + 6, y - 3, 0, clr); // T
				style.draw_rectangle(x - 6, y + 3, x + 6, y + 5, 0, clr); // B
				style.draw_rectangle(x - 6, y - 3, x - 4, y - 1, 0, clr); // L
				style.draw_rectangle(x + 4, y + 1, x + 6, y + 3, 0, clr); // R
				// T
				style.draw_quad(false,
					x + 3, y - 7,
					x + 6, y - 4.99,
					x + 6, y - 3,
					x + 3, y - 0.99,
					clr, clr, clr, clr);
				// B
				style.draw_quad(false,
					x - 6, y + 3.01,
					x - 3, y + 1,
					x - 3, y + 7.01,
					x - 6, y + 5,
					clr, clr, clr, clr);
			} break;
			case PlayShape::Play:
			default:
			{
				style.draw_quad(false,
					x - 5, y - 6, x - 5, y - 6,
					x + 6, y,
					x - 5, y + 6,
					clr, clr, clr, clr);
			} break;
		}
	}
	
}

namespace PlayShape
{
	
	const int Play = 0;
	const int Pause = 1;
	const int Stop = 2;
	const int Repeat = 3;
	
}
