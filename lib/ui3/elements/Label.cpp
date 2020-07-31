#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include 'Graphic.cpp';

class Label : Graphic
{
	
	protected bool _auto_size = false;
	protected string _text;
	protected string _font;
	protected uint _size;
	protected uint _colour;
	protected bool _has_colour = false;
	TextAlign text_align_h = TextAlign::Left;
	
	Label(UI@ ui, const string text, const bool auto_size=false, const string font='', const uint size=0)
	{
		super(ui);
		
		_auto_size = auto_size;
		_font = font;
		_size = size;
		scale_x = ui.style.default_text_scale;
		scale_y = ui.style.default_text_scale;
		
		sizing = ImageSize:: None;
		
		origin_x = 0.5;
		origin_y = 0.5;
		
		this.text = text;
	}
	
	string element_type { get const override { return 'Label'; } }
	
	string text
	{
		get { return _text; }
		set { _text = value; update_size(); }
	}
	
	string font
	{
		get const { return _font; }
		set { _font = value; update_size(); }
	}
	 
	uint size
	{
		get const { return _size; }
		set { _size = value; update_size(); }
	}
	 
	float scale
	{
		set
		{
			if(scale_x == value && scale_y == value)
				return;
			
			scale_x = scale_y = value;
			update_size();
		}
	}
	
	bool auto_size
	{
		get const { return _auto_size; }
		set
		{
			if(_auto_size == value)
				return;
			
			_auto_size = value;
			
			if(value)
			{
				fit_to_contents();
			}
		}
	}
	
	bool has_colour { get const { return _has_colour; } }
	
	uint colour
	{
		get const { return _has_colour ? _colour : ui.style.text_clr; }
		set
		{
			_colour = value;
			_has_colour = true;
		}
	}
	
	Label@ fit_to_contents()
	{
		float w, h;
		ui.style.measure_text(_text, _font, _size, 1, 1, _graphic_width, _graphic_height);
		
		width  = _graphic_width  * scale_x + padding * 2;
		height = _graphic_height * scale_y + padding * 2;
		
		return @this;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		float align_origin;
		
		switch(text_align_h)
		{
			case TextAlign::Left:   align_origin = 0; break;
			case TextAlign::Centre: align_origin = 0.5; break;
			case TextAlign::Right:  align_origin = 1; break;
		}
		
		const float final_scale_x = is_transposed ? draw_scale_y : draw_scale_x;
		const float final_scale_y = is_transposed ? draw_scale_x : draw_scale_y;
		float dx = (is_transposed ? _graphic_height : _graphic_width)  * align_origin * final_scale_x;
		float dy = 0;
		
		if(rotation != 0)
		{
			rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		}
		
		style.draw_text(_text,
			draw_x + dx, draw_y + dy,
			_has_colour ? _colour : ui.style.text_clr,
			final_scale_x, final_scale_y,
			rotation, text_align_h, TextAlign::Top, _font, _size);
	}
	
	private void update_size()
	{
		ui.style.measure_text(_text, _font, _size, 1, 1, _graphic_width, _graphic_height);
		
		if(!auto_size)
			return;
		
		fit_to_contents();
	}
	
}