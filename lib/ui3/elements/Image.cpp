#include '../../math/math.cpp';

class Image : Element
{
	
	protected string sprite_set;
	protected string sprite_name;
	protected sprites@ sprite;
	protected float sprite_offset_x;
	protected float sprite_offset_y;
	protected float sprite_width;
	protected float sprite_height;
	
	float origin_x = 0;
	float origin_y = 0;
	
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour = 0xffffffff;
	
	Image(UI@ ui, const string &in sprite_set, const string &in sprite_name)
	{
		super(ui, 'img');
		
		this.sprite_set = sprite_set;
		this.sprite_name = sprite_name;
		@sprite = ui.style.get_sprite_for_set(sprite_set);
		
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
	
	void do_layout(const float &in parent_x, const float &in parent_y) override
	{
		width = sprite_width * scale_x;
		height = sprite_height * scale_y;
		
		Element::do_layout(parent_x, parent_y);
	}
	
	void draw(const Graphics@ &in graphics, const float &in sub_frame) override
	{
		if(rotation == 0)
		{
			graphics.draw_sprite(sprite,
				sprite_name, 0, 0,
				x1, y1, rotation,
				scale_x, scale_y,
				colour);
			
			return;
		}
		
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y;
		
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		
		graphics.draw_sprite(sprite,
			sprite_name, 0, 0,
			x1 + dx, y1 + dy, rotation,
			scale_x, scale_y,
			colour);
	}
	
}