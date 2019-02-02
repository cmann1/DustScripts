#include 'math/math.cpp';

const array<string> PROP_GROUP_NAMES = {
	'books', 'buildingblocks', 'chains', 'decoration', 'facade', 'foliage', 'furniture', 'gazebo',
	'lighting', '', 'statues', 'storage', 'study', 'fencing', '', '',
	'', '', 'backleaves', 'leaves', 'trunks', 'boulders', 'backdrops', 'temple',
	'npc', 'symbol', 'cars', 'sidewalk', 'machinery'
};

void sprite_from_prop(prop@ p, string &out sprite_set, string &out sprite_name)
{
	sprite_set = 'props' + p.prop_set();
	sprite_name = PROP_GROUP_NAMES[p.prop_group()] + '_' + p.prop_index();
}
void sprite_from_prop(uint prop_set, uint prop_group, uint prop_index, string &out sprite_set, string &out sprite_name)
{
	sprite_set = 'props' + prop_set;
	sprite_name = PROP_GROUP_NAMES[prop_group] + '_' + prop_index;
}

class Prop
{
	
	prop@ p;
	float align_x, align_y;
	float prop_left, prop_top;
	float prop_offset_x, prop_offset_y;
	float props_size_x, props_size_y;
	float anchor_x, anchor_y;
	
	Prop(prop@ p, float align_x=0.5, float align_y=0.5)
	{
		@this.p = p;
		this.align_x = align_x;
		this.align_y = align_y;
		
		string sprite_set, sprite_name;
		sprite_from_prop(p, sprite_set, sprite_name);
		sprites@ spr = create_sprites();
		spr.add_sprite_set(sprite_set);
		rectangle@ r = spr.get_sprite_rect(sprite_name, 0);
		prop_left = r.left();
		prop_top = r.top();
		
		prop_offset_x = prop_left + r.get_width() * align_x;
		prop_offset_y = prop_top + r.get_height() * align_y;
		
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