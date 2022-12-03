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
	
	Sprite()
	{
		@sprite = create_sprites();
	}
	
	Sprite(string sprite_set, string sprite_name, float origin_x=0.5, float origin_y=0.5)
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
	
	
	/**
	 * @param bg_scale Props in the bg layers (0-5) are rendered at twice the scale.
	 * For the origin to work correctly in these layers, pass `2.0 / layer_scale(layer)` for layers 0-5.
	 */
	void draw(
		int layer, int sub_layer,
		uint32 frame, uint32 palette,
		float x, float y, float rotation=0,
		float scale_x=1, float scale_y=1, uint32 colour=0xFFFFFFFF, float bg_scale=1)
	{
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x * bg_scale;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y * bg_scale;
		
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		
		sprite.draw_world(
			layer, sub_layer, sprite_name,
			frame, palette,
			x + dx, y + dy, rotation,
			scale_x, scale_y, colour);
	}
	
	void draw(
		canvas@ c,
		uint32 frame, uint32 palette,
		float x, float y, float rotation=0,
		float scale_x=1, float scale_y=1, uint32 colour=0xFFFFFFFF)
	{
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y;
		
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		
		c.draw_sprite(
			sprite, sprite_name,
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
	
	/**
	 * @param bg_scale Props in the bg layers (0-5) are rendered at twice the scale.
	 * For the origin to work correctly in these layers, pass `2.0 / layer_scale(layer)` for layers 0-5.
	 */
	void real_position(
		const float x, const float y, const float rotation, float &out out_x, float &out out_y,
		const float scale_x=1, const float scale_y=1, float bg_scale=1)
	{
		float dx = (sprite_offset_x - sprite_width * origin_x) * scale_x * bg_scale;
		float dy = (sprite_offset_y - sprite_height * origin_y) * scale_y * bg_scale;
		rotate(dx, dy, rotation * DEG2RAD, dx, dy);
		out_x = x + dx;
		out_y = y + dy;
	}
	
	void get_corners(
		const float x, const float y, const float rotation,
		const float scale_x, const float scale_y,
		float &out x1, float &out y1, float &out x2, float &out y2,
		float &out x3, float &out y3, float &out x4, float &out y4) const
	{
		x1 = -sprite_width * origin_x * scale_x;
		y1 = -sprite_height * origin_y * scale_y;
		x2 = x1 + sprite_width * scale_x;
		y2 = y1;
		x3 = x2;
		y3 = y2 + sprite_height * scale_y;
		x4 = x1;
		y4 = y3;
		rotate(x1, y1, rotation * DEG2RAD, x1, y1);
		rotate(x2, y2, rotation * DEG2RAD, x2, y2);
		rotate(x3, y3, rotation * DEG2RAD, x3, y3);
		rotate(x4, y4, rotation * DEG2RAD, x4, y4);
		x1 += x;
		y1 += y;
		x2 += x;
		y2 += y;
		x3 += x;
		y3 += y;
		x4 += x;
		y4 += y;
	}
	
	void get_bounds(
		const float x, const float y, const float rotation,
		const float scale_x, const float scale_y,
		float &out x1, float &out y1, float &out x2, float &out y2) const
	{
		float bx1, by1, bx2, by2, bx3, by3, bx4, by4;
		get_corners(
			x, y, rotation,
			scale_x, scale_y,
			bx1, by1, bx2, by2, bx3, by3, bx4, by4);
		x1 = min(min(bx1, bx2), min(bx3, bx4));
		y1 = min(min(by1, by2), min(by3, by4));
		x2 = max(max(bx1, bx2), max(bx3, bx4));
		y2 = max(max(by1, by2), max(by3, by4));
	}
	
}
