#include 'PropSelection.cpp';

class BrushDef
{
	
	private float DISTANCE_UNITS = 500;
	private float TIME_UNITS = 1;
	
	[text] bool active = true;
	/** Placed props will have a random rotation between these values (relative to the mouse direction if rotate_to_dir is checked) */
	[text] float angle_min = -180;
	[text] float angle_max =  180;
	/** If larger than zero, the angle will be increased by this much after each prop is placed, otherwise a random angle is chosen. */
	[text] float angle_step = 0;
	/** Rotates along the direction of the mouse */
	[text] bool rotate_to_dir;
	/** The radius of the circle props will randomly be placed in */
	[text] float spread = 48;
	/** The number of props per DISTANCE_UNITS units, or props per TIME_UNITS seconds if the spray option is checked */
	[text] float density = 4;
	/** If true props will be uniformly spaced */
	[text] bool uniform;
	/** If checked props will be placed continuously will the mouse is held down */
	[text] bool spray = false;
	[text] bool flip_x;
	[text] bool flip_y;
	[text] uint layer = 17;
	[text] uint sub_layer = 19;
	
	[text] array<PropSelection> props;
	[hidden] uint props_size;
	
	[text] float cluster_chance = 0.5;
	[text] uint cluster_min = 1;
	[text] uint cluster_max = 1;
	
	array<PropSelection@> valid_props;
	int prop_count;
	
	private float t;
	private float dist;
	private float next_t;
	private float next_t_gap;
	private float next_t_boundary;
	private float place_angle;
	PropSelection@ selected_prop = null;
	
	void init()
	{
		props_size = props.size();
		
		for(uint i = 0; i < props_size; i++)
		{
			props[i].init();
		}
		
		update_prop_count();
	}
	
	bool check_select_prop(bool visible)
	{
		const uint new_props_size = props.size();
		
		if(new_props_size != props_size)
		{
			props_size = new_props_size;
			update_prop_count();
		}
		
		for(uint i = 0; i < new_props_size; i++)
		{
			PropSelection@ prop_selection = @props[i];
			
			if(prop_selection.select_prop == prop_selection.select_prop_prev)
				continue;
			
			if(!visible)
			{
				@selected_prop = prop_selection;
				prop_selection.select_prop_prev = prop_selection.select_prop;
			}
			else
			{
				@selected_prop = null;
			}
			
			return true;
		}
		
		return false;
	}
	
	void update_prop(const PropIndex@ prop_data, uint palette = 0)
	{
		if(@selected_prop == null)
			return;
		
		selected_prop.update_prop(prop_data, palette);
		update_prop_count();
	}
	
	void update_prop_count()
	{
		prop_count = 0;
		valid_props.resize(0);
		
		for(uint i = 0; i < props.size(); i++)
		{
			PropSelection@ p = @props[i];
			
			if(p.has_sprite)
			{
				valid_props.insertLast(p);
				prop_count++;
			}
		}
	}
	
	void start_draw()
	{
		next_t = next_t_boundary = 0;
		calculate_next_t(spray ? TIME_UNITS : DISTANCE_UNITS);
		t = dist = next_t;
		place_angle = 0;
	}
	
	private void calculate_next_t(float unit)
	{
		next_t_gap = unit / density;
		next_t_boundary += next_t_gap;
		next_t = uniform ? next_t_boundary : next_t_boundary - rand_range(0.0, next_t_gap);
	}
	
	void erase(scene@ g, float x, float y, float spread_mul)
	{
		const float r = spread * spread_mul;
		int count = g.get_prop_collision(y - r, y + r, x - r, x + r);
		
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(uint(i));
			
			if(p.layer() != layer)
				continue;
			
			const uint prop_set = p.prop_set();
			const uint prop_group = p.prop_group();
			const uint prop_index = p.prop_index();
			
			for(uint j = 0; j < props.size(); j++)
			{
				PropSelection@ prop_selection = @props[j];
				
				if(prop_selection.prop_set == prop_set && prop_selection.prop_group == prop_group && prop_selection.prop_index == prop_index)
				{
					g.remove_prop(p);
				}
			}
		}
	}
	
	void draw(scene@ g, float mouse_x, float mouse_y, float dist, float dx, float dy, float draw_angle, bool force_angle, float spread_mul, float angle_mul)
	{
		if(!active || prop_count == 0)
			return;
		
		const float start_t = spray ? t : this.dist;
		const float offset_t = (next_t_boundary - start_t);
		t += DT;
		this.dist += dist;
		const float t_delta = spray ? DT : dist;
		const float mode_t = spray ? t : this.dist;
		
		while(mode_t >= next_t)
		{
			const uint cluster_count = cluster_chance > 0 && cluster_max > 1
				? (frand() <= cluster_chance ? (rand_range(cluster_min, cluster_max)) : 1)
				: 1;
			
			for(uint i = 0 ; i < cluster_count; i++)
			{
				// Uniform random point in circle
				float angle = rand_range(-PI, PI);
				float circ_dist = sqrt(frand()) * spread * spread_mul;
				const float dt = uniform
					? (t_delta != 0 ? (next_t - start_t - offset_t) / t_delta : 0)
					: frand();
				float x = mouse_x - (dx * dt) + cos(angle) * circ_dist;
				float y = mouse_y - (dy * dt) + sin(angle) * circ_dist;
				
				// TODO: Left mouse and scroll to adjust spread
				// TODO: Add scale options
				// TODO: Offset broken when angle is not zero and flipped
				if(abs(angle_step) < EPSILON)
				{
					float angle_min = 0;
					float angle_max = 0;
					calculate_angle(angle_mul, angle_min, angle_max);
					angle = rand_range(angle_min, angle_max);
				}
				else
				{
					const float angle_delta = this.angle_max - this.angle_min;
					angle = (angle_delta == 0) ? angle_min : (this.angle_min + (place_angle % angle_delta)) * DEG2RAD;
					place_angle += angle_step;
				}
				
				if(rotate_to_dir || force_angle)
				{
					angle += draw_angle;
				}
				
				PropSelection@ prop_selection = valid_props[rand_range(0, prop_count - 1)];
				
				float ox, oy;
				calculate_prop_offset(prop_selection, angle, ox, oy);
				
				prop@ p = create_prop();
				
				if(flip_x && rand() % 2 == 0)
				{
					ox = -ox;
					p.scale_x(-1);
				}
				
				if(flip_y && rand() % 2 == 0)
				{
					oy = -oy;
					p.scale_y(-1);
				}
				
				p.rotation(angle * RAD2DEG);
				p.x(x + ox);
				p.y(y + oy);
				p.prop_set(prop_selection.prop_set);
				p.prop_group(prop_selection.prop_group);
				p.prop_index(prop_selection.prop_index);
				p.palette(prop_selection.prop_palette);
				p.layer(layer);
				p.sub_layer(sub_layer);
				g.add_prop(p);
			}
			
			calculate_next_t(spray ? TIME_UNITS : DISTANCE_UNITS);
		}
	}
	
	private void calculate_prop_offset(PropSelection@ prop_selection, float angle, float &out ox, float &out oy)
	{
		const PropBounds@ bounds = @prop_selection.prop_bounds;
		const Pivot pivot_alignment = prop_selection.pivot;
		Vec2 pivot;
		
		if(pivot_alignment == Centre)
			pivot.set(0.5, 0.5);
		else if(pivot_alignment == Origin)
			pivot.set(bounds.origin_x, bounds.origin_y);
		else if(pivot_alignment == TopLeft)
			pivot.set(0.0, 0.0);
		else if(pivot_alignment == Top)
			pivot.set(0.5, 0.0);
		else if(pivot_alignment == TopRight)
			pivot.set(1.0, 0.0);
		else if(pivot_alignment == Right)
			pivot.set(1.0, 0.5);
		else if(pivot_alignment == BottomRight)
			pivot.set(1.0, 1.0);
		else if(pivot_alignment == Bottom)
			pivot.set(0.5, 1.0);
		else if(pivot_alignment == BottomLeft)
			pivot.set(0.0, 1.0);
		else if(pivot_alignment == Left)
			pivot.set(0.0, 0.5);
		else if(pivot_alignment == Custom)
			pivot.set(prop_selection.pivot_x, prop_selection.pivot_y);
		
		const float offset_x = bounds.x + bounds.width * pivot.x;
		const float offset_y = bounds.y + bounds.height * pivot.y;
		
		rotate(offset_x, offset_y, angle, ox, oy);
		
		ox = -ox;
		oy = -oy;
	}
	
	void preview(sprites@ sprite, float x, float y, float base_angle, float angle_mul, uint colour)
	{
		if(prop_count == 0)
			return;
		
		PropSelection@ prop_selection = valid_props[0];
		
		float angle_min = 0;
		float angle_max = 0;
		calculate_angle(angle_mul, angle_min, angle_max);
		float angle = lerp(angle_min, angle_max, 0.5);
		
		if(rotate_to_dir)
		{
			angle += base_angle;
		}
		
		float ox, oy;
		calculate_prop_offset(prop_selection, angle, ox, oy);
		
		sprite.add_sprite_set(prop_selection.sprite_set);
		sprite.draw_world(
			layer, sub_layer, prop_selection.sprite_name, 0, prop_selection.prop_palette,
			x + ox, y + oy, angle * RAD2DEG,
			1, 1, colour);
	}
	
	void calculate_angle(float angle_mul, float &out min, float &out max)
	{
		float angle_min = this.angle_min * DEG2RAD;
		float angle_max = this.angle_max * DEG2RAD;
		
		if(angle_mul == 1)
		{
			min = angle_min;
			max = angle_max;
			return;
		}
		
		float range = shortest_angle(angle_min, angle_max);
		float mid = angle_min + range * 0.5;
		
		min = mid - range * 0.5 * angle_mul;
		max = mid + range * 0.5 * angle_mul;
	}
	
}