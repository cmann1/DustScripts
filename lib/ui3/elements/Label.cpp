#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';

class Label : Element
{
	
	bool auto_size = true;
	
	private string _text;
	private string _font;
	private uint _size;
	private float _scale;
	uint colour;
	TextAlign align_h = TextAlign::Left;
	TextAlign align_v = TextAlign::Top;
	
	Label(UI@ ui, const string text, const string font='', const uint size=0)
	{
		super(ui, 'lbl');
		
		_font = font == '' ? ui.style.default_font : font;
		_size = size == 0 ? ui.style.default_text_size : size;
		_scale = ui.style.default_text_scale;
		colour = ui.style.text_clr;
		
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
		get const { return _scale; }
		set { _scale = value; update_size(); }
	}
	
	void fit_to_contents(const float padding=0)
	{
		ui.style.measure_text(_text, _font, _size, _scale, width, height);
		
		width  += padding * 2;
		height += padding * 2;
	}
	
	void _draw(Style@ style) override
	{
		float x, y;
		
		switch(align_h)
		{
			case TextAlign::Left:	x = x1; break;
			case TextAlign::Centre:	x = (x1 + x2) * 0.5; break;
			case TextAlign::Right:	x = x2; break;
		}
		
		switch(align_v)
		{
			case TextAlign::Top:	y = y1; break;
			case TextAlign::Middle:	y = (y1 + y2) * 0.5; break;
			case TextAlign::Bottom:	y = y2; break;
		}
		
		style.draw_text(_text, x, y, colour, _scale, _scale, 0, align_h, align_v, _font, _size);
	}
	
	private void update_size()
	{
		if(!auto_size)
			return;
		
		fit_to_contents(0);
	}
	
}