#include '../lib/std.cpp';
#include '../lib/math/math.cpp';
#include '../lib/drawing/circle.cpp';
#include '../lib/props.cpp';
#include '../lib/props_bounds.cpp';
#include '../lib/drawing/Sprite.cpp';
#include '../lib/ui/prop-selector/PropSelector.cpp';

class script
{
	
	[text] bool draw = false;
	/** A multiplier for spread affecting all props in the brush */
	[text] float spread_mul = 1;
	/** A multiplier for angle affecting all props in the brush */
	[text] float angle_mul = 1;
	[text] array<BrushDef> brushes;
	
	private scene@ g;
	private UI@ ui = UI();
	private PropSelector prop_selector(ui);
	private int selected_brush_def_index = -1;
	
	private bool started = false;
	private Sprite sprite;
	
	private float prev_x;
	private float prev_y;
	private float prev_angle;
	private float prev_angle2;
	private float draw_angle;
	
	script()
	{
		@g = get_scene();
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
		draw_angle = lerp_angle(lerp_angle(mouse_angle, prev_angle, 0.5), prev_angle2, 0.5);
		
		if(ui.right_mouse_down)
		{
			for(uint i = 0; i < brushes.size(); i++)
			{
				BrushDef@ b = @brushes[i];
				const float mx = b.layer < 12 ? g.mouse_x_world(0, b.layer) : mouse_x;
				const float my = b.layer < 12 ? g.mouse_y_world(0, b.layer) : mouse_y;
				brushes[i].draw(g, mx, my, mouse_distance, dx, dy, draw_angle);
			}
		}
		
		prev_x = mouse_x;
		prev_y = mouse_y;
		prev_angle2 = prev_angle;
		prev_angle = mouse_angle;
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
			
			// TODO: Use brush angle
			const float angle_min = normalize_angle(@brush != null ? brush.angle_min * DEG2RAD : 0);
			const float angle_max = normalize_angle(@brush != null ? brush.angle_max * DEG2RAD : 0);
			const float mouse_x = g.mouse_x_world(0, mouse_layer);
			const float mouse_y = g.mouse_y_world(0, mouse_layer);
			const uint alpha = ui.right_mouse_down ? 0x44000000 : 0xaa000000;
			const float radius = max(brush.spread * spread_mul, 24);
			const float thickness = 2;
			const uint colour = alpha | 0xffffff;
			
			draw_circle(g, mouse_x, mouse_y, radius, 32, mouse_layer, 24, thickness, colour);
			
			g.draw_line(
				mouse_layer, 24,
				mouse_x, mouse_y,
				mouse_x + cos(draw_angle) * radius,
				mouse_y + sin(draw_angle) * radius,
				thickness, colour);
			
			if(abs(angle_min) > 0.001)
			{
				g.draw_line(
					mouse_layer, 24,
					mouse_x, mouse_y,
					mouse_x + cos(draw_angle + angle_min) * radius,
					mouse_y + sin(draw_angle + angle_min) * radius,
					thickness, colour);
			}
			
			if(abs(angle_max) > 0.001)
			{
				g.draw_line(
					mouse_layer, 24,
					mouse_x, mouse_y,
					mouse_x + cos(draw_angle + angle_max) * radius,
					mouse_y + sin(draw_angle + angle_max) * radius,
					thickness, colour);
			}
		}
	}
	
}

class BrushDef
{
	
	[text] bool active = true;
	/** Placed props will have a random rotation between these values (relative to the mouse direction if rotate_to_dir is checked) */
	[text] float angle_min = -180;
	[text] float angle_max =  180;
	/** Rotates along the direction of the mouse */
	[text] bool rotate_to_dir;
	/** The radius of the circle props will randomly be placed in */
	[text] float spread = 48;
	/** The number of props per 100 units, or props per second if the spray option is checked */
	[text] float density = 4;
	/** If checked props will be placed continuously will the mouse is held down */
	[text] bool spray = false;
	[text] uint layer = 17;
	[text] uint sub_layer = 19;
	
	[text] array<PropSelection> props;
	array<PropSelection@> valid_props;
	int prop_count;
	
	private float t;
	private float dist;
	private float last_placed;
	private float next_place;
	private float next_place_reset;
	private bool has_placed;
	PropSelection@ selected_prop = null;
	
	void init()
	{
		for(uint i = 0; i < props.size(); i++)
		{
			props[i].init();
		}
		
		update_prop_count();
	}
	
	bool check_select_prop(bool visible)
	{
		for(uint i = 0; i < props.size(); i++)
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
		t = 0;
		dist = 0;
		last_placed = 0;
		next_place = 9999999;
		next_place_reset = 0;
		has_placed = false;
		
		if(spray)
		{
			update_next_place_t();
		}
		else
		{
			// TODO: Update distance for density per units
		}
	}
	
	void draw(scene@ g, float mouse_x, float mouse_y, float dist, float dx, float dy, float draw_angle)
	{
		if(!active || prop_count == 0)
			return;
		
		bool place = false;
		
		if(spray)
		{
			if(!has_placed && t >= next_place)
			{
				place = true;
				has_placed = true;
			}
			
			if(t >= next_place_reset)
			{
				update_next_place_t();
			}
		}
		else
		{
			// TODO: Density per units
		}
		
		if(place)
		{
			// TODO: Use brush angle and spread multipliers
			// Uniform random point in circle
			float angle = rand_range(-PI, PI);
			float circ_dist = sqrt(frand()) * spread;
			float x = mouse_x + cos(angle) * circ_dist;
			float y = mouse_y + sin(angle) * circ_dist;
			
			angle = rand_range(angle_min * DEG2RAD, angle_max * DEG2RAD);
			
			if(rotate_to_dir)
			{
				angle += draw_angle;
			}
			
			PropSelection@ prop_selection = valid_props[rand_range(0, prop_count - 1)];
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
			float ox, oy;
			
			rotate(offset_x, offset_y, angle, ox, oy);
			prop@ p = create_prop();
			p.rotation(angle * RAD2DEG);
			p.x(x - ox);
			p.y(y - oy);
			p.prop_set(prop_selection.prop_set);
			p.prop_group(prop_selection.prop_group);
			p.prop_index(prop_selection.prop_index);
			p.palette(prop_selection.prop_palette);
			p.layer(layer);
			p.sub_layer(sub_layer);
			g.add_prop(p);
		}
		
		t += DT;
		this.dist += dist;
	}
	
	private void update_next_place_t()
	{
		next_place = t + rand_range(0.0, 1 / density);
		next_place_reset += 1 / density;
		has_placed = false;
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











