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
//	TextAlign align_h = TextAlign::Left;
//	TextAlign align_v = TextAlign::Top;
	
	Label(UI@ ui, const string text, const bool auto_size=false, const string font='', const uint size=0)
	{
		super(ui, 'lbl');
		
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
		style.draw_text(_text,
			draw_x, draw_y,
			_has_colour ? _colour : ui.style.text_clr,
			is_transposed ? draw_scale_y : draw_scale_x,
			is_transposed ? draw_scale_x : draw_scale_y,
			rotation, TextAlign::Left, TextAlign::Top, _font, _size);
	}
	
	private void update_size()
	{
		ui.style.measure_text(_text, _font, _size, 1, 1, _graphic_width, _graphic_height);
		
		if(!auto_size)
			return;
		
		fit_to_contents();
	}
	
}