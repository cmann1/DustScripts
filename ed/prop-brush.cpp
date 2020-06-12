#include '../lib/std.cpp';
#include '../lib/math/math.cpp';
#include '../lib/tiles/closest_tile.cpp';
#include '../lib/drawing/circle.cpp';
#include '../lib/props.cpp';
#include '../lib/props_bounds.cpp';
#include '../lib/drawing/Sprite.cpp';
#include '../lib/ui/prop-selector/PropSelector.cpp';

class script
{
	
	[text] bool draw = false;
	[text] bool preview = true;
	/** A multiplier for spread affecting all props in the brush */
	[text] float spread_mul = 1;
	/** A multiplier for angle affecting all props in the brush */
	[text] float angle_mul = 1;
	/** How much to smooth placement angles based on mouse movement */
	[text] float smoothing = 50;
	/** If true props snap to the nearest tile's surface */
	[text] bool place_on_tiles;
	/** How far to check for tiles when **place_on_tiles** is on */
	[text] float place_on_tiles_distance = 30;
	/** Which layer to snap to when **place_on_tiles** is on. -1 will use the brush's layer */
	[text] int place_on_tiles_layer = -1;
	[text] array<BrushDef> brushes;
	
	private scene@ g;
	private UI@ ui = UI();
	private PropSelector prop_selector(ui);
	private int selected_brush_def_index = -1;
	
	private bool started = false;
	private sprites@ sprite;
	private raycast@ ray = null;
	
	private float tail_x;
	private float tail_y;
	private float prev_x;
	private float prev_y;
	private float prev_angle;
	private float prev_angle2;
	private float draw_angle;
	
	script()
	{
		@g = get_scene();
		@sprite = create_sprites();
	}
	
	void start()
	{
		prop_selector.hide();
		
		for(uint i = 0; i < brushes.size(); i++)
		{
			brushes[i].init();
		}
	}
	
	void editor_step()
	{
		if(!started)
		{
			start();
			started = true;
		}
		
		ui.step();
		
		for(uint i = 0; i < brushes.size(); i++)
		{
			BrushDef@ brush_def = @brushes[i];
			
			if(!brush_def.check_select_prop(prop_selector.visible))
				continue;
			
			selected_brush_def_index = int(i);
			
			if(!prop_selector.visible)
			{
				prop_selector.select_group(null);
				prop_selector.select_prop(null);
				
				if(@brush_def.selected_prop != null)
				{
					prop_selector.select_prop(
						brush_def.selected_prop.prop_set,
						brush_def.selected_prop.prop_group,
						brush_def.selected_prop.prop_index,
						brush_def.selected_prop.prop_palette);
				}
				else
				{
					prop_selector.select_prop(null);
				}
				
				prop_selector.show();
			}
			else
			{
				selected_brush_def_index = -1;
				prop_selector.hide();
			}
			
			break;
		}
		
		if(draw && !prop_selector.visible)
		{
			run();
		}
	}
	
	private void run()
	{
		const float mouse_x = g.mouse_x_world(0, 19);
		const float mouse_y = g.mouse_y_world(0, 19);
		
		move_tail(mouse_x, mouse_y);
		
		if(ui.right_mouse_down)
		{
			if(ui.right_mouse_press)
			{
				for(uint i = 0; i < brushes.size(); i++)
				{
					brushes[i].start_draw();
				}
			}
		}
		
		const float mouse_distance = distance(prev_x, prev_y, mouse_x, mouse_y);
		const float dx = mouse_x - prev_x;
		const float dy = mouse_y - prev_y;
		const float mouse_angle = mouse_distance > 0.001 ? atan2(dy, dx) : prev_angle;
		
		if(smoothing > 0)
		{
			const float tail_delta_x = mouse_x - tail_x;
			const float tail_delta_y = mouse_y - tail_y;
			draw_angle = atan2(tail_delta_y, tail_delta_x);
		}
		else
		{
			draw_angle = lerp_angle(lerp_angle(mouse_angle, prev_angle, 0.5), prev_angle2, 0.5);
		}
		
		if(ui.right_mouse_down)
		{
			for(uint i = 0; i < brushes.size(); i++)
			{
				BrushDef@ b = @brushes[i];
				float mx = b.layer < 12 ? g.mouse_x_world(0, b.layer) : mouse_x;
				float my = b.layer < 12 ? g.mouse_y_world(0, b.layer) : mouse_y;
				
				if(place_on_tiles)
				{
					if(!snap_to_tiles(
						mx, my, draw_angle, place_on_tiles_distance,
						place_on_tiles_layer <= 0 || place_on_tiles_layer > 20 ? b.layer : place_on_tiles_layer,
						mx, my, draw_angle)
					)
						continue;
				}
				
				// TODO: Options to flip horizontally or vertically
				b.draw(g, mx, my, mouse_distance, dx, dy, draw_angle, spread_mul, angle_mul);
			}
		}
		else if(ui.middle_mouse_down)
		{
			for(uint i = 0; i < brushes.size(); i++)
			{
				BrushDef@ b = @brushes[i];
				const float mx = b.layer < 12 ? g.mouse_x_world(0, b.layer) : mouse_x;
				const float my = b.layer < 12 ? g.mouse_y_world(0, b.layer) : mouse_y;
				b.erase(g, mx, my, spread_mul);
			}
		}
		
		prev_x = mouse_x;
		prev_y = mouse_y;
		prev_angle2 = prev_angle;
		prev_angle = mouse_angle;
	}
	
	bool snap_to_tiles(float x, float y, float angle, float radius, uint layer, float &out out_x, float &out out_y, float &out out_angle)
	{
		int tile_x, tile_y;
		float result_x, result_y;
		float normal_x, normal_y;
		
		if(closest_tile(g,
			x, y, radius,
			layer, tile_x, tile_y, result_x, result_y, normal_x, normal_y))
		{
			const float dx = x - result_x;
			const float dy = y - result_y;
			const float length = sqrt(dx * dy + dy * dy);
			
//			g.draw_line(
//					22, 22, 
//					x, y,
//					result_x, result_y,
//					1, 0xFFFF00FF);
					
			if(length <= radius)
			{
				out_x = result_x;
				out_y = result_y;
				const float l = sqrt(dx * dx + dy * dy);
				out_angle = dot(dx, dy, normal_x, normal_y) < 0 ? atan2(-dx / l, dy / l) : atan2(dx / l, -dy / l);
				return true;
			}
		}
		
		out_x = x;
		out_y = y;
		out_angle = angle;
		return false;
	}
	
	void move_tail(float mouse_x, float mouse_y)
	{
		// Move the tail towards the position
		float tail_delta_x = tail_x - mouse_x;
		float tail_delta_y = tail_y - mouse_y;
		float tail_angle = atan2(tail_delta_y, tail_delta_x);
		const float tail_t = 1 - clamp01(smoothing * DT);
		tail_x -= tail_delta_x * tail_t;
		tail_y -= tail_delta_y * tail_t;
		
		// Clamp the tail distance
		tail_delta_x = tail_x - mouse_x;
		tail_delta_y = tail_y - mouse_y;
		const float tail_distance = tail_delta_x * tail_delta_x + tail_delta_y * tail_delta_y;
		const float tail_dist_min = smoothing * 0.15;
		const float tail_dist_max = smoothing * 0.5;
		
		if(tail_dist_min > 0 && tail_distance < tail_dist_min * tail_dist_min)
		{
			tail_x = mouse_x + cos(tail_angle) * tail_dist_min;
			tail_y = mouse_y + sin(tail_angle) * tail_dist_min;
		}
		
		if(tail_dist_max >= 0 && tail_distance > tail_dist_max * tail_dist_max)
		{
			tail_x = mouse_x + cos(tail_angle) * tail_dist_max;
			tail_y = mouse_y + sin(tail_angle) * tail_dist_max;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(prop_selector.visible)
		{
			if(selected_brush_def_index >= 0 && selected_brush_def_index < int(brushes.size()))
			{
				prop_selector.draw();
				
				if(prop_selector.result == Selected)
				{
					brushes[selected_brush_def_index].update_prop(prop_selector.result_prop, prop_selector.result_palette);
					
					selected_brush_def_index = -1;
					prop_selector.hide();
				}
				else if(prop_selector.result == None)
				{
					brushes[selected_brush_def_index].update_prop(null);
					
					prop_selector.hide();
				}
			}
			else
			{
				selected_brush_def_index = -1;
				prop_selector.hide();
			}
		}
		else if(draw)
		{
			BrushDef@ brush;
			
			for(uint i = 0; i < brushes.size(); i++)
			{
				BrushDef@ b = @brushes[i];
				
				if(b.active && b.prop_count > 0)
				{
					@brush = b;
					break;
				}
			}
			
			const uint mouse_layer = @brush != null ? brush.layer : 19;
			
			float angle_min = 0;
			float angle_max = 0;
			
			if(@brush != null)
			{
				brush.calculate_angle(angle_mul, angle_min, angle_max);
			}
			
			const float mouse_x = g.mouse_x_world(0, mouse_layer);
			const float mouse_y = g.mouse_y_world(0, mouse_layer);
			const uint alpha = ui.right_mouse_down ? 0x44000000 : 0xaa000000;
			const float radius = !place_on_tiles ? max(@brush != null ? brush.spread * spread_mul : 0, 10) : place_on_tiles_distance;
			const float thickness = 2;
			const uint colour = alpha | 0xffffff;
			const uint range_colour = alpha | 0x4444ff;
			const float overlay_angle = @brush == null || brush.rotate_to_dir  ? draw_angle : 0;
			
			if(preview && !ui.right_mouse_down && !ui.middle_mouse_down && @brush != null)
			{
				brush.preview(@sprite, mouse_x, mouse_y, overlay_angle, angle_mul, alpha | 0xffffff);
			}
			
			// Draw tail
//			g.draw_line(
//				mouse_layer, 24,
//				mouse_x, mouse_y,
//				tail_x, tail_y,
//				1, alpha | 0x0000ff);
			
			draw_circle(g, mouse_x, mouse_y, radius, 32, mouse_layer, 24, thickness, colour);
			
			g.draw_line(
				mouse_layer, 24,
				mouse_x, mouse_y,
				mouse_x + cos(overlay_angle) * radius,
				mouse_y + sin(overlay_angle) * radius,
				thickness, colour);
			
			if(abs(angle_min) > 0.001)
			{
				g.draw_line(
					mouse_layer, 24,
					mouse_x, mouse_y,
					mouse_x + cos(overlay_angle + angle_min) * radius,
					mouse_y + sin(overlay_angle + angle_min) * radius,
					thickness, range_colour);
			}
			
			if(abs(angle_max) > 0.001)
			{
				g.draw_line(
					mouse_layer, 24,
					mouse_x, mouse_y,
					mouse_x + cos(overlay_angle + angle_max) * radius,
					mouse_y + sin(overlay_angle + angle_max) * radius,
					thickness, range_colour);
			}
		}
	}
	
}

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
	
	void draw(scene@ g, float mouse_x, float mouse_y, float dist, float dx, float dy, float draw_angle, float spread_mul, float angle_mul)
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
				
				// TODO: Error if angle_step is not zero, and angle_min == angle_max
				// TODO: Always use tile angle when place_on_tiles is true
				// TODO: Left mouse and scroll to adjust spread
				// TODO: Add scale options
				if(abs(angle_step) < EPSILON)
				{
					float angle_min = 0;
					float angle_max = 0;
					calculate_angle(angle_mul, angle_min, angle_max);
					angle = rand_range(angle_min, angle_max);
				}
				else
				{
					angle = (this.angle_min + (place_angle % (this.angle_max - this.angle_min))) * DEG2RAD;
					place_angle += angle_step;
				}
				
				if(rotate_to_dir)
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
	
}

enum Pivot
{
	
	Centre,
	Origin,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left,
	Custom,
	
}











