#include 'Pivot.cpp';

class PropSelection
{
	
	[text] uint prop_set;
	[text] uint prop_group;
	[text] uint prop_index;
	[text] uint prop_palette;
	
	[text] bool select_prop;
	[hidden] bool select_prop_prev;
	
	[option,0:Centre,1:Origin,2:TopLeft,3:Top,4:TopRight,5:Right,6:BottomRight,7:Bottom,8:BottomLeft,9:Left,10:Custom]
	Pivot pivot = Pivot::Centre;
	[text] float pivot_x = 0.5;
	[text] float pivot_y = 0.5;
	
	[text] bool clone;
	[hidden] bool clone_prev;
	
	[hidden] bool has_sprite;
	string sprite_set;
	string sprite_name;
	const PropBounds@ prop_bounds;
	
	void init()
	{
		update_prop();
	}
	
	void update_prop(const PropIndex@ prop_data, uint palette = 0)
	{
		if(prop_data is null)
		{
			prop_set = 0;
			prop_group = 0;
			prop_index = 0;
			prop_palette = 0;
			has_sprite = false;
			
			update_prop();
			return;
		}
		
		prop_set = prop_data.set;
		prop_group = prop_data.group;
		prop_index = prop_data.index;
		prop_palette = palette;
		has_sprite = true;
		
		update_prop();
	}
	
	private void update_prop()
	{
		if(!has_sprite)
		{
			sprite_set = '';
			sprite_name = '';
			@prop_bounds = null;
			return;
		}
		
		sprite_from_prop(prop_set, prop_group, prop_index, sprite_set, sprite_name);
		int index = prop_index_to_array_index(prop_set, prop_group, prop_index);
		
		@prop_bounds = index != -1 ? PROP_BOUNDS[prop_group][index] : null;
	}

	PropSelection copy()
	{
		PropSelection p;

		p.prop_set = prop_set;
		p.prop_group = prop_group;
		p.prop_index = prop_index;
		p.prop_palette = prop_palette;

		p.select_prop = select_prop;
		p.select_prop_prev = select_prop_prev;

		p.pivot = pivot;
		p.pivot_x = pivot_x;
		p.pivot_y = pivot_y;

		p.has_sprite = has_sprite;
		p.sprite_set = sprite_set;
		p.sprite_name = sprite_name;
		@p.prop_bounds = @prop_bounds;

		return p;
	}
	
}