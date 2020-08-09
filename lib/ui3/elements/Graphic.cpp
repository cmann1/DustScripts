#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/GraphicAlign.cpp';
#include '../utils/ImageSize.cpp';

abstract class Graphic : Element
{
	
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	float origin_x = 0.5;
	float origin_y = 0.5;
	
	float align_h = GraphicAlign::Centre;
	float align_v = GraphicAlign::Middle;
	ImageSize sizing = ImageSize:: ConstrainInside;
	float padding;
	
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
	
	Graphic(UI@ ui)
	{
		super(ui);
	}
	
	string element_type { get const override { return 'Graphic'; } }
	
	void _do_layout(LayoutContext@ ctx)
	{
		float x1 = ctx.scroll_x + this.x + (@parent != null ? parent.x1 : 0);
		float y1 = ctx.scroll_y + this.y + (@parent != null ? parent.y1 : 0);
		float x2 = x1 + this._width;
		float y2 = y1 + this._height;
		
		is_transposed = abs(abs(rotation) - 90) <= EPSILON;
		const float gr_width  = ceil(is_transposed ? _graphic_height : _graphic_width);
		const float gr_height = ceil(is_transposed ? _graphic_width  : _graphic_height);
		const float scale_x = is_transposed ? this.scale_y : this.scale_x;
		const float scale_y = is_transposed ? this.scale_x : this.scale_y;
		
		if(border_size > 0 && border_colour != 0)
		{
			x1 += border_size;
			y1 += border_size;
			x2 -= border_size;
			y2 -= border_size;
		}
		
		const float padding = is_nan(this.padding) ? ui.style.spacing : this.padding;
		
		{
			x1 += padding;
			y1 += padding;
			x2 -= padding;
			y2 -= padding;
		}
		
		const float width  = x2 - x1;
		const float height = y2 - y1;
		
		switch(sizing)
		{
			case ImageSize::None:
				draw_scale_x = scale_x;
				draw_scale_y = scale_y;
				break;
			case ImageSize::Fit:
				draw_scale_x = gr_width > 0  ? (width  / gr_width) : 0;
				draw_scale_y = gr_height > 0 ? (height / gr_height) : 0;
				ui.debug.print(_id + ' ' + width +'/'+ gr_width + ' - ' + height+'/'+ gr_height+' = '+draw_scale_x+','+draw_scale_y, 0xffffffff, _id+'aafs', 1);
				break;
			case ImageSize::FitInside:
			case ImageSize::ConstrainInside:
			{
				if(sizing == ImageSize::FitInside || gr_width * scale_x > width || gr_height * scale_y > height)
				{
					const float width_scale  = gr_width  * scale_x > 0 ? width  / (gr_width  * scale_x) : 0;
					const float height_scale = gr_height * scale_y > 0 ? height / (gr_height * scale_y) : 0;
					const float scale = min(width_scale, height_scale);
					draw_scale_x = scale_x * scale;
					draw_scale_y = scale_y * scale;
				}
				else
				{
					draw_scale_x = scale_x;
					draw_scale_y = scale_y;
				}
			}
				break;
		}
		
		float dx = _graphic_offset_x - _graphic_width  * origin_x;
		float dy = _graphic_offset_y - _graphic_height * origin_y;
		
		if(rotation != 0)
		{
			rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		}
		
		const float x = x1 + (width  - gr_width  * draw_scale_x) * align_h + gr_width  * draw_scale_x * (is_transposed ? origin_y : origin_x);
		const float y = y1 + (height - gr_height * draw_scale_y) * align_v + gr_height * draw_scale_y * (is_transposed ? origin_x : origin_y);
		
		draw_x = x + ui.pixel_ceil(dx * draw_scale_x);
		draw_y = y + ui.pixel_ceil(dy * draw_scale_y);
	}
	
}