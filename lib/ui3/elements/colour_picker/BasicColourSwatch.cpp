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
		if((background_colour >> 24) & 0xff != 255)
		{
			style.draw_rectangle(x1, y1, x1 + _width * 0.5, y2, 0, 0xffffffff);
			
			const int repeat_x = 3;
			const int repeat_y = 3;
			int sx = 0;
			
			for(int y = 0; y < repeat_y; y++)
			{
				const float y1 = this.y1 + _height * (float(y) / repeat_y);
				const float y2 = this.y1 + _height * (float(y + 1) / repeat_y);
				
				for(int x = sx; x < repeat_x; x += 2)
				{
					style.draw_rectangle(
						x1 + _width * 0.5 * (float(x) / repeat_x), y1,
						x1 + _width * 0.5 * (float(x + 1) / repeat_x), y2,
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
	}
	
}