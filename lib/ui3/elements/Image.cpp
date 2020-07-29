#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Position.cpp';
#include '../utils/ImageSize.cpp';

class Image : Element
{
	
	float origin_x = 0.5;
	float origin_y = 0.5;
	
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour = 0xffffffff;
	
	Position position = Position::Middle;
	ImageSize sizing = ImageSize::ConstrainInside;
	float padding;
	
	string _sprite_set;
	string _sprite_name;
	float _sprite_width;
	float _sprite_height;
	float _sprite_offset_x;
	float _sprite_offset_y;
	
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
			_sprite_offset_x = -rect.left();
			_sprite_offset_y = -rect.top();
			_sprite_width = rect.get_width();
			_sprite_height = rect.get_height();
			
			if(_sprite_offset_x == 0 && _sprite_offset_y == 0 && _sprite_width == 1 && _sprite_height == 1)
			{
				_sprite_offset_x = _sprite_offset_y = _sprite_width = _sprite_height = 0;
			}
		}
		else
		{
			_sprite_width = width;
			_sprite_height = height;
			_sprite_offset_x = offset_x;
			_sprite_offset_y = offset_y;
		}
		
		_set_width  = this._width = _sprite_width * scale_x;
		_set_height = this._height = _sprite_height * scale_y;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Element::_draw(@style, @ctx);
		
		float x1 = this.x1;
		float y1 = this.y1;
		float x2 = this.x2;
		float y2 = this.y2;
		
		if(border_size > 0 && border_colour != 0)
		{
			x1 += border_size;
			y1 += border_size;
			x2 -= border_size;
			y2 -= border_size;
		}
		
		if(padding > 0)
		{
			x1 += padding;
			y1 += padding;
			x2 -= padding;
			y2 -= padding;
		}
		
		const float width  = x2 - x1;
		const float height = y2 - y1;
		
		float x;
		float y;
		
		switch(position)
		{
			case Position::LeftTop:
				x = x1;
				y = y1;
				break;
			case Position::Top:
				x = x1 + width * 0.5;
				y = y1;
				break;
			case Position::RightTop:
				x = x2;
				y = y1;
				break;
			case Position::Right:
				x = x2;
				y = y1 + height * 0.5;
				break;
			case Position::RightBottom:
				x = x2;
				y = y2;
				break;
			case Position::Bottom:
				x = x1 + width * 0.5;
				y = y2;
				break;
			case Position::LeftBottom:
				x = x1;
				y = y2;
				break;
			case Position::Left:
				x = x1;
				y = y1 + height * 0.5;
				break;
			case Position::Middle:
				x = x1 + width * 0.5;
				y = y1 + height * 0.5;
				break;
		}
		
		float scale_x;
		float scale_y;
		
		switch(sizing)
		{
			case ImageSize::None:
				scale_x = this.scale_x;
				scale_y = this.scale_y;
				break;
			case ImageSize::Fit:
				scale_x = _sprite_width > 0  ? width  / _sprite_width : 0;
				scale_y = _sprite_height > 0 ? height / _sprite_height : 0;
				break;
			case ImageSize::FitInside:
			case ImageSize::ConstrainInside:
			{
				if(sizing == ImageSize::FitInside || _sprite_width * this.scale_x > width || _sprite_height * this.scale_y > height)
				{
					const float width_scale  = _sprite_width  * this.scale_x > 0 ? width  / (_sprite_width  * this.scale_x) : 0;
					const float height_scale = _sprite_height * this.scale_y > 0 ? height / (_sprite_height * this.scale_y) : 0;
					const float scale = min(width_scale, height_scale);
					scale_x = this.scale_x * scale;
					scale_y = this.scale_y * scale;
				}
				else
				{
					scale_x = this.scale_x;
					scale_y = this.scale_y;
				}
			}
				break;
		}
		
		float dx = _sprite_offset_x - _sprite_width * origin_x;
		float dy = _sprite_offset_y - _sprite_height * origin_y;
		
		if(rotation != 0)
		{
			dx = _sprite_offset_x - _sprite_width * origin_x;
			dy = _sprite_offset_y - _sprite_height * origin_y;
			rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		}
		
		style.draw_sprite(sprite,
			_sprite_name, 0, 0,
			x + dx * scale_x - 0.5,
			y + dy * scale_y - 0.5, rotation,
			scale_x, scale_y,
			colour);
	}
	
}