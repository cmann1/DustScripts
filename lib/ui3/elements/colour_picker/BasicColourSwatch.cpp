#include '../Element.cpp';

namespace BasicColourSwatch { const string TYPE_NAME = 'BasicColourSwatch'; }

class BasicColourSwatch : Element
{
	
	BasicColourSwatch(UI@ ui)
	{
		super(ui);
		
		width = 34;
		height = 34;
		
		mouse_enabled = false;
	}
	
	string element_type { get const override { return ColourSlider::TYPE_NAME; } }
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		const float border_size = max(0.0, this.border_size);
		const float inset = border_colour != 0 ? border_size : 0;
		const float x1 = this.x1 + inset;
		const float y1 = this.y1 + inset;
		const float x2 = this.x2 - inset;
		const float y2 = this.y2 - inset;
		const float _width  = x2 - x1;
		const float _height = y2 - y1;
		
		if((background_colour >> 24) & 0xff != 255)
		{
			style.draw_rectangle(x1, y1, x1 + _width * 0.5, y2, 0, 0xffffffff);
			
			const int repeat_x = max(2, ceil_int(_width  / 15));
			const int repeat_y = max(2, ceil_int(_height / 15));
			int sx = 0;
			
			for(int y = 0; y < repeat_y; y++)
			{
				const float sqr_y1 = y1 + _height * (float(y) / repeat_y);
				const float sqr_y2 = y1 + _height * (float(y + 1) / repeat_y);
				
				for(int x = sx; x < repeat_x; x += 2)
				{
					style.draw_rectangle(
						x1 + _width * 0.5 * (float(x) / repeat_x), sqr_y1,
						x1 + _width * 0.5 * (float(x + 1) / repeat_x), sqr_y2,
						0, 0xffaaaaaa);
				}
				
				sx = (sx + 1) % 2;
			}
			
			style.draw_rectangle(x1, y1, x1 + _width * 0.5, y2, 0, background_colour);
			style.draw_rectangle(x1 + _width * 0.5, y1, x2, y2, 0, background_colour | 0xff000000);
		}
		else
		{
			style.draw_rectangle(x1, y1, x2, y2, 0, background_colour);
		}
		
		if(border_colour != 0)
		{
			style.outline(
				this.x1, this.y1,
				this.x2, this.y2,
				border_size, border_colour);
		}
	}
	
}