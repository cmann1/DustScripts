#include '../../lib/std.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/tiles/closest_tile.cpp';
#include '../../lib/drawing/circle.cpp';
#include '../../lib/props.cpp';
#include '../../lib/props_bounds.cpp';
#include '../../lib/drawing/Sprite.cpp';
#include '../../lib/ui/prop-selector/PropSelector.cpp';
#include 'BrushDef.cpp';

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
		
		/* Adjust spread with mouse wheel
		 * */
		
		float spread_adjustment;
		bool adjust_spread = ui.mouse_scroll(spread_adjustment);
		spread_adjustment = -spread_adjustment;
		
		if(!ui.left_mouse_down)
		{
			spread_adjustment *= 5;
		}
		
		if(place_on_tiles)
		{
			if(adjust_spread)
			{
				place_on_tiles_distance = max(0, place_on_tiles_distance + spread_adjustment);
			}
		}
		
		/* Update brushes and show prop selector
		 * */
		
		for(uint i = 0; i < brushes.size(); i++)
		{
			BrushDef@ brush_def = @brushes[i];
			
			if(!place_on_tiles && brush_def.active)
			{
				if(adjust_spread)
				{
					brush_def.adjust_spread(spread_adjustment);
				}
			}
			
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
				
				b.draw(g, mx, my, mouse_distance, dx, dy, draw_angle, place_on_tiles, spread_mul, angle_mul);
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
		else
		{
			
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
			const float length = sqrt(dx * dx + dy * dy);
					
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
			const float real_radius = !place_on_tiles
				? (@brush != null ? max(brush.spread * spread_mul, 0) : 0)
				: place_on_tiles_distance;
			const float radius = max(real_radius, 15);
			const float thickness = 2;
			const uint colour = alpha | 0xffffff;
			const uint range_colour = alpha | 0x4444ff;
			float overlay_angle = @brush == null || brush.rotate_to_dir || place_on_tiles  ? draw_angle : 0;
			
			if(place_on_tiles)
			{
				float tile_x, tile_y;
				
				if(snap_to_tiles(
					mouse_x, mouse_y, overlay_angle, place_on_tiles_distance,
					place_on_tiles_layer > 0 && place_on_tiles_layer <= 20 || @brush == null ? place_on_tiles_layer : brush.layer,
					tile_x, tile_y, overlay_angle)
				)
				{
					g.draw_line(
						22, 22, 
						mouse_x, mouse_y,
						tile_x, tile_y,
						2, alpha | 0xff00ff);
				}
			}
			
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
			
			draw_circle(g, mouse_x, mouse_y, real_radius, 32, mouse_layer, 24, thickness, colour);
			
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











