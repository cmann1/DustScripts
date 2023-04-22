#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/GraphicAlign.cpp';
#include '../utils/ImageSize.cpp';

abstract class Graphic : Element
{
	
	protected float _rotation;
	protected float _scale_x = 1;
	protected float _scale_y = 1;
	protected float _origin_x = 0.5;
	protected float _origin_y = 0.5;
	
	protected float _align_h = GraphicAlign::Centre;
	protected float _align_v = GraphicAlign::Middle;
	protected ImageSize _sizing = ImageSize:: ConstrainInside;
	protected float _padding_left;
	protected float _padding_right;
	protected float _padding_top;
	protected float _padding_bottom;
	
	protected uint _colour;
	protected bool _has_colour = false;
	
	protected float _graphic_offset_x = 0;
	protected float _graphic_offset_y = 0;
	protected float _graphic_width;
	protected float _graphic_height;
	
	protected float draw_x;
	protected float draw_y;
	protected float draw_scale_x;
	protected float draw_scale_y;
	
	protected bool is_transposed;
	
	float graphic_offset_x { get const { return _graphic_offset_x; } }
	float graphic_offset_y { get const { return _graphic_offset_y; } }
	float graphic_width { get const { return _graphic_width; } }
	float graphic_height { get const { return _graphic_height; } }
	
	float debug_draw_x { get const { return draw_x; } }
	float debug_draw_y { get const { return draw_y; } }
	float debug_draw_scale_x { get const { return draw_scale_x; } }
	float debug_draw_scale_y { get const { return draw_scale_y; } }
	bool debug_is_transposed { get const { return is_transposed; } }
	
	Graphic(UI@ ui)
	{
		super(ui);
	}
	
	string element_type { get const override { return 'Graphic'; } }
	
	float rotation
	{
		get const { return _rotation; }
		set { if(_rotation == value) return; _rotation = value; validate_layout = true; }
	}
	
	float scale_x
	{
		get const { return _scale_x; }
		set { if(_scale_x == value) return; _scale_x = value; validate_layout = true; }
	}
	
	float scale_y
	{
		get const { return _scale_y; }
		set { if(_scale_y == value) return; _scale_y = value; validate_layout = true; }
	}
	
	float origin_x
	{
		get const { return _origin_x; }
		set { if(_origin_x == value) return; _origin_x = value; validate_layout = true; }
	}
	
	float origin_y
	{
		get const { return _origin_y; }
		set { if(_origin_y == value) return; _origin_y = value; validate_layout = true; }
	}
	
	float align_h
	{
		get const { return _align_h; }
		set { if(_align_h == value) return; _align_h = value; validate_layout = true; }
	}
	
	float align_v
	{
		get const { return _align_v; }
		set { if(_align_v == value) return; _align_v = value; validate_layout = true; }
	}
	
	ImageSize sizing
	{
		get const { return _sizing; }
		set { if(_sizing == value) return; _sizing = value; validate_layout = true; }
	}
	
	float padding_left
	{
		get const { return _padding_left; }
		set { if(_padding_left == value) return; _padding_left = value; validate_layout = true; }
	}
	
	float padding_right
	{
		get const { return _padding_right; }
		set { if(_padding_right == value) return; _padding_right = value; validate_layout = true; }
	}
	
	float padding_top
	{
		get const { return _padding_top; }
		set { if(_padding_top == value) return; _padding_top = value; validate_layout = true; }
	}
	
	float padding_bottom
	{
		get const { return _padding_bottom; }
		set { if(_padding_bottom == value) return; _padding_bottom = value; validate_layout = true; }
	}
	
	void set_padding(const float padding)
	{
		_padding_left = _padding_right = _padding_top = _padding_bottom = padding;
		validate_layout= true;
	}
	
	void set_padding(const float padding_left_right, const float padding_top_bottom)
	{
		_padding_left	= padding_left_right;
		_padding_right	= padding_left_right;
		_padding_top	= padding_top_bottom;
		_padding_bottom	= padding_top_bottom;
		validate_layout= true;
	}
	
	void set_padding(const float padding_left, const float padding_right, const float padding_top, const float padding_bottom)
	{
		this._padding_left		= padding_left;
		this._padding_right		= padding_right;
		this._padding_top		= padding_top;
		this._padding_bottom	= padding_bottom;
		validate_layout= true;
	}
	
	bool has_colour
	{
		get const { return _has_colour; }
		set { _has_colour = value; }
	}
	
	uint colour
	{
		get const { return _has_colour ? _colour : ui.style.text_clr; }
		set
		{
			_colour = value;
			_has_colour = true;
		}
	}
	
	/// Resets the colour to the default text colour defined in the ui style
	void clear_colour()
	{
		has_colour = false;
	}
	
	float real_padding_left		{ get const { return is_nan(this._padding_left)		? ui.style.spacing : this._padding_left; } }
	float real_padding_right	{ get const { return is_nan(this._padding_right)	? ui.style.spacing : this._padding_right; } }
	float real_padding_top		{ get const { return is_nan(this._padding_top)		? ui.style.spacing : this._padding_top; } }
	float real_padding_bottom	{ get const { return is_nan(this._padding_bottom)	? ui.style.spacing : this._padding_bottom; } }
	
	void _do_layout(LayoutContext@ ctx) override
	{
		float x1 = 0;
		float y1 = 0;
		float x2 = this._width;
		float y2 = this._height;
		
		_rotation = mod(_rotation, 360.0);
		is_transposed = abs(abs(mod(_rotation, 180.0)) - 90) <= EPSILON;
		
		const float gr_width  = is_transposed ? _graphic_height : _graphic_width;
		const float gr_height = is_transposed ? _graphic_width  : _graphic_height;
		const float _scale_x = is_transposed ? this._scale_y : this._scale_x;
		const float _scale_y = is_transposed ? this._scale_x : this._scale_y;
		
		if(border_size > 0 && border_colour != 0)
		{
			x1 += border_size;
			y1 += border_size;
			x2 -= border_size;
			y2 -= border_size;
		}
		
		const float padding_left	= is_nan(this._padding_left)	? ui.style.spacing : this._padding_left;
		const float padding_right	= is_nan(this._padding_right)	? ui.style.spacing : this._padding_right;
		const float padding_top		= is_nan(this._padding_top)		? ui.style.spacing : this._padding_top;
		const float padding_bottom	= is_nan(this._padding_bottom)	? ui.style.spacing : this._padding_bottom;
		
		{
			x1 += padding_left;
			y1 += padding_top;
			x2 -= padding_right;
			y2 -= padding_bottom;
		}
		
		const float width  = x2 - x1;
		const float height = y2 - y1;
		
		switch(_sizing)
		{
			case ImageSize::None:
				draw_scale_x = _scale_x;
				draw_scale_y = _scale_y;
				break;
			case ImageSize::Fit:
				draw_scale_x = gr_width > 0  ? (width  / gr_width) : 0;
				draw_scale_y = gr_height > 0 ? (height / gr_height) : 0;
				break;
			case ImageSize::FitInside:
			case ImageSize::ConstrainInside:
			{
				if(_sizing == ImageSize::FitInside || gr_width * _scale_x > width || gr_height * _scale_y > height)
				{
					const float width_scale  = gr_width  * _scale_x > 0 ? width  / (gr_width  * _scale_x) : 0;
					const float height_scale = gr_height * _scale_y > 0 ? height / (gr_height * _scale_y) : 0;
					const float scale = min(width_scale, height_scale);
					draw_scale_x = _scale_x * scale;
					draw_scale_y = _scale_y * scale;
				}
				else
				{
					draw_scale_x = _scale_x;
					draw_scale_y = _scale_y;
				}
			}
				break;
		}
		
		float dx = _graphic_offset_x - _graphic_width  * _origin_x;
		float dy = _graphic_offset_y - _graphic_height * _origin_y;
		
		if(_rotation != 0)
		{
			rotate(dx, dy, _rotation * DEG2RAD, dx, dy);
		}
		
		if(is_transposed)
		{
			if(abs(_rotation - 90) <= EPSILON)
			{
				dy--;
			}
			else if(abs(_rotation - 270) <= EPSILON)
			{
				dy++;
			}
		}
		else if(abs(_rotation - 180) <= EPSILON)
		{
			dx++;
		}
		
		const float x = x1 + (width  - gr_width  * draw_scale_x) * _align_h + gr_width  * draw_scale_x * (is_transposed ? _origin_y : _origin_x);
		const float y = y1 + (height - gr_height * draw_scale_y) * _align_v + gr_height * draw_scale_y * (is_transposed ? _origin_x : _origin_y);
		
		draw_x = x + ui._pixel_ceil(dx * draw_scale_x);
		draw_y = y + ui._pixel_ceil(dy * draw_scale_y);
		//puts(id+str(x + ui._pixel_ceil(dx * draw_scale_x), y + ui._pixel_ceil(dy * draw_scale_y)));
	}
	
	protected uint get_draw_colour()
	{
		return _has_colour ? _colour : 0xffffffff;
	}
	
}
