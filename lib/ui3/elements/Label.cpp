#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../utils/TextClipping.cpp';
#include 'Graphic.cpp';

namespace Label { const string TYPE_NAME = 'Label'; }

class Label : Graphic
{
	
	TextClipping clipping = TextClipping::None;
	
	TextAlign _text_align_h = TextAlign::Left;
	
	protected bool _auto_size = false;
	protected string _text;
	protected string _font;
	protected uint _size;
	
	protected bool is_clipped;
	protected string clipped_text = '';
	protected float clipped_text_width;
	
	Label(UI@ ui, const string text='', const bool auto_size=false, const string font='', const uint size=0)
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
		align_h = GraphicAlign::Left;
		align_v = GraphicAlign::Top;
		
		fit_to_contents();
	}
	
	string element_type { get const override { return Label::TYPE_NAME; } }
	
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
			
			scale_x = value;
			scale_y = value;
			update_size();
		}
	}
	
	TextAlign text_align_h
	{
		get const { return _text_align_h; }
		set
		{
			if(value == _text_align_h)
				return;
			
			_text_align_h = value;
			validate_layout = true;
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
			
			validate_layout = true;
		}
	}
	
	Label@ fit_to_contents()
	{
		float w, h;
		ui.style.measure_text(_text, _font, _size, 1, 1, _graphic_width, _graphic_height);
		
		width  = _graphic_width  * scale_x + real_padding_left + real_padding_right;
		height = _graphic_height * scale_y + real_padding_top + real_padding_bottom;
		
		return @this;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		// Force a graphic align of left and calculate the offsets ourselves.
		_align_h = GraphicAlign::Left;
		
		Graphic::_do_layout(ctx);
		
		if(clipping != TextClipping::None)
		{
			do_clipping();
		}
		else
		{
			is_clipped = false;
		}
	}
	
	void set_font(const string font, const uint size)
	{
		_font = font;
		_size = size;
		update_size();
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		float align_origin;
		
		switch(_text_align_h)
		{
			case TextAlign::Left:   align_origin = 0; break;
			case TextAlign::Centre: align_origin = 0.5; break;
			case TextAlign::Right:  align_origin = 1; break;
		}
		
		const float final_scale_x = is_transposed ? draw_scale_y : draw_scale_x;
		const float final_scale_y = is_transposed ? draw_scale_x : draw_scale_y;
		const float size = is_transposed ? _graphic_height : _graphic_width;
		float dx = size * align_origin;
		float dy = 0;
		
		if(!is_transposed)
		{
			const float padding_left	= is_nan(this._padding_left)	? ui.style.spacing : this._padding_left;
			const float padding_right	= is_nan(this._padding_right)	? ui.style.spacing : this._padding_right;
			dx += (_width - padding_left - padding_right - size) * align_origin;
		}
		else
		{
			const float padding_top		= is_nan(this._padding_top)		? ui.style.spacing : this._padding_top;
			const float padding_bottom	= is_nan(this._padding_bottom)	? ui.style.spacing : this._padding_bottom;
			dx += (_height - padding_top - padding_bottom - size) * align_origin;
		}
		
		if(_rotation != 0)
		{
			rotate(dx, dy, _rotation * DEG2RAD, dx, dy);
		}
		
		style.draw_text(is_clipped ? clipped_text : _text,
			ui._pixel_round(x1) + draw_x + dx - (_font != 'envy_bold' ? 1 : 0),
			ui._pixel_round(y1) + draw_y + dy,
			get_draw_colour(),
			final_scale_x, final_scale_y,
			_rotation, _text_align_h, TextAlign::Top, _font, _size);
	}
	
	protected void update_size()
	{
		ui.style.measure_text(_text, _font, _size, 1, 1, _graphic_width, _graphic_height);
		validate_layout = true;
		
		if(!auto_size)
			return;
		
		fit_to_contents();
	}
	
	protected void do_clipping()
	{
		if(_graphic_width * _scale_x <= _width)
		{
			is_clipped = false;
			return;
		}
		
		float max_width = _width;
		
		float ellipses_width = 0;
		if(clipping == TextClipping::Ellipses)
		{
			float _;
			ui.style.measure_text('...', _font, _size, _scale_x, _scale_y, ellipses_width, _);
			max_width -= ellipses_width;
		}
		
		int left = 0;
		int right = int(_text.length());
		is_clipped = true;
		int mid = 0;
		float _;
		
		while(right >= left)
		{
			mid = left + (right - left) / 2;
			
			const string txt = _text.substr(0, mid);
			float w;
			ui.style.measure_text(txt, _font, _size, _scale_x, _scale_y, w, _);
			
			if(w < max_width)
			{
				left = mid + 1;
			}
			else if(w > max_width)
			{
				right = mid - 1;
			}
			else
			{
				break;
			}
		}
		
		clipped_text = _text.substr(0, mid - 1);
		
		if(clipping == TextClipping::Ellipses)
		{
			clipped_text += '...';
		}
		
		ui.style.measure_text(clipped_text, _font, _size, _scale_x, _scale_y, clipped_text_width, _);
	}
	
	protected uint get_draw_colour() override
	{
		return _has_colour ? _colour : ui.style.text_clr;
	}
	
}
