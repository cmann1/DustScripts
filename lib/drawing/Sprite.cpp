#include '../math/math.cpp';

class Sprite
{
	string sprite_set;
	string sprite_name;

	sprites@ sprite;
	float sprite_offset_x;
	float sprite_offset_y;
	float sprite_width;
	float sprite_height;
	
	float origin_x;
	float origin_y;
	
	Sprite(string sprite_set='', string sprite_name='', float origin_x=0.5, float origin_y=0.5)
	{
		@sprite = create_sprites();
		set(sprite_set, sprite_name, origin_x, origin_y);
	}
	
	void set(string sprite_set, string sprite_name, float origin_x=0.5, float origin_y=0.5)
	{
		this.sprite_set  = sprite_set;
		this.sprite_name = sprite_name;
		this.origin_x = origin_x;
		this.origin_y = origin_y;
		
		if(sprite_set == '')
		{
			sprite_offset_x = sprite_offset_y = sprite_width = sprite_height = 0;
		}
		else
		{
			sprite.add_sprite_set(sprite_set);
			rectangle@ rect = sprite.get_sprite_rect(sprite_name, 0);
			sprite_offset_x = -rect.left();
			sprite_offset_y = -rect.top();
			sprite_width = rect.get_width();
			sprite_height = rect.get_height();
			
			if(sprite_offset_x == 0 && sprite_offset_y == 0 && sprite_width == 1 && sprite_height == 1)
				sprite_offset_x = sprite_offset_y = sprite_width = sprite_height = 0;
		}
	}
	
	void draw(
		int layer, int sub_layer,
		uint32 frame, uint32 palette,
		float x, float y, float rotation=0,
		float scale_x=1, float scale_y=1, uint32 colour=0xFFFFFFFF)
	{
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y;
		
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		
		sprite.draw_world(
			layer, sub_layer, sprite_name,
			frame, palette,
			x + dx, y + dy, rotation,
			scale_x, scale_y, colour);
	}
	
	void draw_hud(int layer, int sub_layer,
		uint32 frame, uint32 palette, float x, float y, float rotation=0,
		float scale_x=1, float scale_y=1, uint32 colour=0xFFFFFFFF)
	{
		float dx = sprite_offset_x - sprite_width * origin_x;
		float dy = sprite_offset_y - sprite_height * origin_y;
		
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		
		sprite.draw_hud(
			layer, sub_layer, sprite_name,
			frame, palette,
			x + dx * scale_x, y + dy * scale_y, rotation,
			scale_x, scale_y, colour);
	}
	
	void real_position(const float x, const float y, const float rotation, float &out out_x, float &out out_y, const float scale_x=1, const float scale_y=1)
	{
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y;
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		out_x = x + dx;
		out_y = y + dy;
	}
	
}