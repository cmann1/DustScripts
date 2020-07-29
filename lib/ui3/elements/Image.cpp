#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Position.cpp';

class Image : Element
{
	
	float origin_x = 0.5;
	float origin_y = 0.5;
	
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour = 0xffffffff;
	
	Position position = Position::Middle;
	
	protected string sprite_set;
	protected string sprite_name;
	protected sprites@ sprite;
	protected float sprite_offset_x;
	protected float sprite_offset_y;
	protected float sprite_width;
	protected float sprite_height;
	
	/**
	 * @brief Normally the sprite's width, height, and offset will be calcualted automatically, but for embedded/script sprites (and possibly some other ones too)
	 * these do not seem to be accurate
	 */
	Image(UI@ ui, const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		super(ui, 'img');
		
		set_sprite(sprite_set, sprite_name, width, height, offset_x, offset_y);
	}
	
	void set_sprite(const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		if(this.sprite_set == sprite_set && this.sprite_name == sprite_name)
			return;
		
		this.sprite_set = sprite_set;
		this.sprite_name = sprite_name;
		@sprite = @sprite == null ? ui.style.get_sprite_for_set(sprite_set) : @sprite;
		sprite.add_sprite_set(sprite_set);
		
		if(width <= 0 || height <= 0)
		{
			rectangle@ rect = sprite.get_sprite_rect(sprite_name, 0);
			sprite_offset_x = -rect.left();
			sprite_offset_y = -rect.top();
			sprite_width = rect.get_width();
			sprite_height = rect.get_height();
			
			if(sprite_offset_x == 0 && sprite_offset_y == 0 && sprite_width == 1 && sprite_height == 1)
			{
				sprite_offset_x = sprite_offset_y = sprite_width = sprite_height = 0;
			}
		}
		else
		{
			sprite_width = width;
			sprite_height = height;
			sprite_offset_x = offset_x;
			sprite_offset_y = offset_y;
		}
		
		_set_width  = this._width = sprite_width * scale_x;
		_set_height = this._height = sprite_height * scale_y;
	}
	
	/*void _do_layout(LayoutContext@ ctx) override
	{
		width = sprite_width * scale_x;
		height = sprite_height * scale_y;
	}*/
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Element::_draw(@style, @ctx);
		
		float x;
		float y;
		
		switch(position)
		{
			case Position::LeftTop:
				x = x1;
				y = y1;
				break;
			case Position::Top:
				x = x1 + (x2 - x1) * 0.5;
				y = y1;
				break;
			case Position::RightTop:
				x = x2;
				y = y1;
				break;
			case Position::Right:
				x = x2;
				y = y1 + (y2 - y1) * 0.5;
				break;
			case Position::RightBottom:
				x = x2;
				y = y2;
				break;
			case Position::Bottom:
				x = x1 + (x2 - x1) * 0.5;
				y = y2;
				break;
			case Position::LeftBottom:
				x = x1;
				y = y2;
				break;
			case Position::Left:
				x = x1;
				y = y1 + (y2 - y1) * 0.5;
				break;
			case Position::Middle:
				x = (x2 - x1) * 0.5;
				y = (y2 - y1) * 0.5;
				break;
		}
		
		float dx;
		float dy;
		
		if(rotation == 0)
		{
			dx = sprite_offset_x - sprite_width * origin_x;
			dy = sprite_offset_y - sprite_height * origin_y;
		}
		else
		{
			dx = sprite_offset_x - sprite_width * origin_x;
			dy = sprite_offset_y - sprite_height * origin_y;
			rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		}
		
		style.draw_sprite(sprite,
			sprite_name, 0, 0,
			x + dx * scale_x - 0.5,
			y + dy * scale_y - 0.5, rotation,
			scale_x, scale_y,
			colour);
	}
	
}