#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include 'Graphic.cpp';

namespace Image { const string TYPE_NAME = 'Image'; }

class Image : Graphic
{
	
	string _sprite_set;
	string _sprite_name;
	uint palette;
	uint frame;
	
	protected sprites@ sprite;
	
	/// @brief Normally the sprite's width, height, and offset will be calcualted automatically, but for embedded/script
	/// sprites (and possibly some other ones too) these do not seem to be accurate
	Image(
		UI@ ui, const string _sprite_set, const string _sprite_name, const float width=-1, const float height=-1,
		const float offset_x=0, const float offset_y=0)
	{
		super(ui);
		
		set_sprite(_sprite_set, _sprite_name, width, height, offset_x, offset_y);
	}
	
	string element_type { get const override { return Image::TYPE_NAME; } }
	
	void set_sprite(
		const string _sprite_set, const string _sprite_name, const float width=-1, const float height=-1,
		const float offset_x=0, const float offset_y=0)
	{
		if(this._sprite_set == _sprite_set && this._sprite_name == _sprite_name)
			return;
		
		this._sprite_set = _sprite_set;
		this._sprite_name = _sprite_name;
		@sprite = ui.style.get_sprite_for_set(_sprite_set);
		
		if(width <= 0 || height <= 0)
		{
			rectangle@ rect = sprite.get_sprite_rect(_sprite_name, 0);
			_graphic_offset_x = -rect.left();
			_graphic_offset_y = -rect.top();
			_graphic_width  = rect.get_width();
			_graphic_height = rect.get_height();
			
			if(_graphic_offset_x == 0 && _graphic_offset_y == 0 && _graphic_width == 1 && _graphic_height == 1)
			{
				_graphic_offset_x = _graphic_offset_y = _graphic_width = _graphic_height = 0;
			}
		}
		else
		{
			_graphic_width  = width;
			_graphic_height = height;
			_graphic_offset_x = offset_x;
			_graphic_offset_y = offset_y;
		}
		
		_set_width  = this._width  = _graphic_width  * scale_x;
		_set_height = this._height = _graphic_height * scale_y;
		
		validate_layout = true;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Element::_draw(@style, @ctx);
		
		style.draw_sprite(sprite,
			_sprite_name, frame, palette,
			// Add 0.5 based on the screen size to get pixel perfect images (seems like screen size was forced to be even by some dustmod update).
			// All other drawing operations are `ceil`ed by `Style` and the values here rounded down to make sure everything aligns correctly.
			// X and Y outputs seem to differe though when rendered so one needs to be incremented and the other decremented.
			ui._pixel_round(x1 + draw_x - 0.5) - (ui._even_screen_width  ? 0.0 : 0.5),
			ui._pixel_round(y1 + draw_y + 0.5) - (ui._even_screen_height ? 0.0 : 0.5),
			_rotation,
			is_transposed ? draw_scale_y : draw_scale_x,
			is_transposed ? draw_scale_x : draw_scale_y,
			get_draw_colour());
	}
	
}
