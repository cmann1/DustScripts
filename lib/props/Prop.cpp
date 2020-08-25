#include '../math/math.cpp';
#include 'common.cpp';

class Prop
{
	
	prop@ p;
	float align_x, align_y;
	float prop_left, prop_top;
	float prop_offset_x, prop_offset_y;
	float prop_scale_x, prop_scale_y;
	float anchor_x, anchor_y;
	
	Prop(prop@ p, float align_x=0.5, float align_y=0.5, scene@ g=null)
	{
		@this.p = p;
		this.align_x = align_x;
		this.align_y = align_y;
		
		prop_scale_x = p.scale_x();
		prop_scale_y = p.scale_y();
		
		string sprite_set, sprite_name;
		sprite_from_prop(p, sprite_set, sprite_name);
		sprites@ spr = create_sprites();
		spr.add_sprite_set(sprite_set);
		rectangle@ r = spr.get_sprite_rect(sprite_name, 0);
		prop_left = r.left();
		prop_top = r.top();
		
		const float layer_scale = @g != null && p.layer() <= 5 ? g.layer_scale(p.layer()) : 1.0;
		const float backdrop_scale = p.layer() <= 5 ? 2.0 : 1.0;
		
		prop_offset_x = prop_scale_x * (prop_left + r.get_width() * align_x)  / layer_scale * backdrop_scale;
		prop_offset_y = prop_scale_y * (prop_top  + r.get_height() * align_y) / layer_scale * backdrop_scale;
		
		rotate(prop_offset_x, prop_offset_y, p.rotation() * DEG2RAD, anchor_x, anchor_y);
		anchor_x += p.x();
		anchor_y += p.y();
	}
	
	void rotation(float r)
	{
		float ox = 0, oy = 0;
		rotate(prop_offset_x, prop_offset_y, r * DEG2RAD, ox, oy);
		p.rotation(r);
		
		p.x(anchor_x - ox);
		p.y(anchor_y - oy);
	}
	
}