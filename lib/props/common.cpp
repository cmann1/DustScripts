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

prop@ copy_prop(prop@ p)
{
	prop@ new_p = create_prop();
	new_p.x(p.x());
	new_p.y(p.y());
	new_p.rotation(p.rotation());
	new_p.scale_x(p.scale_x());
	new_p.scale_y(p.scale_y());
	new_p.prop_set(p.prop_set());
	new_p.prop_group(p.prop_group());
	new_p.prop_index(p.prop_index());
	new_p.palette(p.palette());
	new_p.layer(p.layer());
	new_p.sub_layer(p.sub_layer());
	
	return new_p;
}

prop@ create_prop(uint set, uint group, uint index, float x=0, float y=0, int layer=19, int sub_layer=19, float rotation=0)
{
	prop@ p = create_prop();
	p.prop_set(set);
	p.prop_group(group);
	p.prop_index(index);
	p.layer(layer);
	p.sub_layer(sub_layer);
	p.x(x);
	p.y(y);
	p.rotation(rotation);
	
	return p;
}


