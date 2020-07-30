#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include 'Graphic.cpp';

class Image : Graphic
{
	
	uint colour = 0xffffffff;
	
	string _sprite_set;
	string _sprite_name;
	
	protected sprites@ sprite;
	
	/**
	 * @brief Normally the sprite's width, height, and offset will be calcualted automatically, but for embedded/script sprites (and possibly some other ones too)
	 * these do not seem to be accurate
	 */
	Image(UI@ ui, const string _sprite_set, const string _sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		super(ui, 'img');
		
		set_sprite(_sprite_set, _sprite_name, width, height, offset_x, offset_y);
	}
	
	void set_sprite(const string _sprite_set, const string _sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		if(this._sprite_set == _sprite_set && this._sprite_name == _sprite_name)
			return;
		
		this._sprite_set = _sprite_set;
		this._sprite_name = _sprite_name;
		@sprite = @sprite == null ? ui.style.get_sprite_for_set(_sprite_set) : @sprite;
		sprite.add_sprite_set(_sprite_set);
		
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
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Element::_draw(@style, @ctx);
		
		style.draw_sprite(sprite,
			_sprite_name, 0, 0,
			draw_x - 0.5,
			draw_y - 0.5, rotation,
			draw_scale_x, draw_scale_y,
			colour);
	}
	
}