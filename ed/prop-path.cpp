#include '../lib/std.cpp';
#include '../lib/props.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/math/Bezier.cpp';
#include '../lib/drawing/Sprite.cpp';

class script
{
}

class PropDef
{
	[hidden] string sprite_set = 'props1';
	[hidden] string sprite_name = 'books_8';
	[text] uint prop_set = 1;
	[text] uint prop_group = 0;
	[text] uint prop_index = 8;
	[text] float origin_x = 0.1;
	[text] float origin_y = 0.5;
	[text] float spacing = 55; 
	[text] int layer = 19;
	[text] int sub_layer = 19;
	[text] int end_layer = -1;
	[text] int end_sub_layer = -1;
	[text] int frame = 0;
	[text] int palette = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	[angle] float rotation;
	[text] float scale_sx = 1;
	[text] float scale_sy = 1;
	[text] float scale_ex = 1;
	[text] float scale_ey = 1;
	
	void calculate_layer_ranges(int &out layer_start, int &out layer_end, int &out layer_count)
	{
		layer_start = (layer * 25 + sub_layer);
		layer_end   = (end_layer != -1 ? end_layer : layer) * 25 + int(max(0, end_sub_layer));
		layer_count = layer_end - layer_start + 1;// + start_layer_offset + end_layer_offset;
	}
	
	void calculate_layers(int layer_start, int layer_count, int total_sub_layer, int &out current_layer, int &out current_sub_layer)
	{
		current_layer = (total_sub_layer / 25);
		current_sub_layer = (total_sub_layer % 25);
	}
	
	void calculate_layers(int layer_start, int layer_count, float t, int &out current_layer, int &out current_sub_layer)
	{
		calculate_layers(layer_start, layer_count, int(layer_start + layer_count * t), current_layer, current_sub_layer);
	}
}

class PropPath : trigger_base
{

	scene@ g;
	scripttrigger @self;
	
	[text] bool place = false;
	[hidden] bool place_prev = false;

	[text] bool hide_props = false;
	[text] bool hide_overlays = false;
	[text] bool smart_handles = true;
	[text] array<Bezier> curves;
	[text] PropDef prop_def;
	
	Sprite spr;
	
	int vertex_count = 0;
	array<float> previous_curve_values;
	
	int dragged_vertex_index = -1;
	float dragged_vertex_dx;
	float dragged_vertex_dy;
	float vertex_radius = pow(7, 2);
	int mouse_dragged_vertex_index = -1;
	int mouse_over_vertex_index = -1;
	
	int t = 0;

	PropPath()
	{
		@g = get_scene();
	}

	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		
		const int curve_count = curves.length();
		previous_curve_values.resize(vertex_count = curve_count * 8);
		int j = 0;
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			curve.update();
			previous_curve_values[j++] = curve.x1;
			previous_curve_values[j++] = curve.y1;
			previous_curve_values[j++] = curve.y2;
			previous_curve_values[j++] = curve.y2;
			previous_curve_values[j++] = curve.y3;
			previous_curve_values[j++] = curve.y3;
			previous_curve_values[j++] = curve.y4;
			previous_curve_values[j++] = curve.y4;
		}
	}
	
	void place_props()
	{
		const int curve_count = int(curves.length());
		
		if(curve_count > 0)
		{
			float total_length = 0;
			for(int i = 0; i < curve_count; i++)
				total_length += curves[i].length;
			
			const float spacing = prop_def.spacing;
			float d = spacing;
			float total_d = d;
			int curve_index = 0;
			Bezier@ curve = @curves[curve_index++];
			float x1 = curve.x1;
			float y1 = curve.y1;
			float x, y;
			
			const float scale_sx = prop_def.scale_sx;
			const float scale_sy = prop_def.scale_sy;
			float d_scale_x = prop_def.scale_ex - scale_sx;
			float d_scale_y = prop_def.scale_ey - scale_sy;
			
			int layer_start, layer_end, layer_count;
			prop_def.calculate_layer_ranges(layer_start, layer_end, layer_count);
			float layer_current = layer_start;
			
			while(true)
			{
				const float t = total_d / total_length;
				
				const float x2 = curve.mx(d);
				const float y2 = curve.my(d);
				const float angle = atan2(y2 - y1, x2 - x1) * RAD2DEG + prop_def.rotation;
				const float scale_x = prop_def.scale_x * (scale_sx + d_scale_x * t);
				const float scale_y = prop_def.scale_y * (scale_sy + d_scale_y * t);
				
				float base_t = (total_d - spacing) / (total_length - spacing);
				int current_layer, current_sub_layer;
				prop_def.calculate_layers(layer_start, layer_count, base_t, current_layer, current_sub_layer);
				
				spr.real_position(x1, y1, angle, x, y, scale_x, scale_y);
				prop@ p = create_prop();
				p.x(x);
				p.y(y);
				p.rotation(angle);
				p.scale_x(scale_x);
				p.scale_y(scale_y);
				p.prop_set(prop_def.prop_set);
				p.prop_group(prop_def.prop_group);
				p.prop_index(prop_def.prop_index);
				p.palette(prop_def.palette);
				p.layer(current_layer);
				p.sub_layer(current_sub_layer);
				g.add_prop(p);
				
				d += spacing;
				total_d += spacing;
				x1 = x2;
				y1 = y2;
				if(d > curve.length)
				{
					if(curve_index >= curve_count)
						break;
						
					d -= curve.length;
					@curve = @curves[curve_index++];
				}
			}
		}
		
		place_prev = place;
	}

	void editor_step()
	{
		const int curve_count = curves.length();
		
		if(vertex_count != curve_count * 8)
		{
			const int count_prev = vertex_count / 8;
			previous_curve_values.resize(vertex_count = curve_count * 8);
			
			// Initialise new curves to something reasonable
			if(curve_count > count_prev)
				for(int i = count_prev; i < curve_count; i++)
				{
					const int vi = i * 8;
					const bool p = i > 0;
					Bezier@ cp = p ? @curves[i - 1] : null;
					Bezier@ curve = @curves[i];
					previous_curve_values[vi    ] = curve.x1 = p ? cp.x4 : self.x() - 100;
					previous_curve_values[vi + 1] = curve.y1 = p ? cp.y4 : self.y();
					previous_curve_values[vi + 2] = curve.x2 = curve.x1 + (p ? (cp.x4 - cp.x3) :  50);
					previous_curve_values[vi + 3] = curve.y2 = curve.y1 + (p ? (cp.y4 - cp.y3) : -50);
					previous_curve_values[vi + 4] = curve.x3 = curve.x1 + 150;
					previous_curve_values[vi + 5] = curve.y3 = curve.y1 - 50;
					previous_curve_values[vi + 6] = curve.x4 = curve.x1 + 200;
					previous_curve_values[vi + 7] = curve.y4 = curve.y1;
				}
		}
		
		if(spr.sprite_set == '' || prop_def.sprite_set != spr.sprite_set || prop_def.sprite_name != spr.sprite_set)
		{
			sprite_from_prop(prop_def.prop_set, prop_def.prop_group, prop_def.prop_index, prop_def.sprite_set, prop_def.sprite_name);
			spr.set(prop_def.sprite_set, prop_def.sprite_name, prop_def.origin_x, prop_def.origin_y);
		}
		
		if(place != place_prev)
			place_props();
			
		const bool left_mouse_down = g.mouse_state(0) & 4 != 0;
		const bool middle_mouse_down = g.mouse_state(0) & 16 != 0;
		const float mouse_x = g.mouse_x_world(0, 22); //prop_def.layer);
		const float mouse_y = g.mouse_y_world(0, 22); //prop_def.layer);
		
		bool smart_handles = this.smart_handles;

		if(self.editor_selected())
		{
			if(middle_mouse_down)
				smart_handles = !smart_handles;
				
			mouse_over_vertex_index = -1;
			
			if(mouse_dragged_vertex_index == -1)
			{
				float dx, dy;
				
				for(int i = 0; i < curve_count; i++)
				{
					Bezier@ curve = @curves[i];
					
					dx = curve.x1 - mouse_x; dy = curve.y1 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8;
						break;
					}
					
					dx = curve.x2 - mouse_x; dy = curve.y2 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 2;
						break;
					}
					
					dx = curve.x3 - mouse_x; dy = curve.y3 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 4;
						break;
					}
					
					dx = curve.x4 - mouse_x; dy = curve.y4 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 6;
						break;
					}
				}
			}
			
			if(left_mouse_down || middle_mouse_down)
			{
				if(mouse_dragged_vertex_index == -1)
					mouse_dragged_vertex_index = mouse_over_vertex_index;
			}
			else
			{
				mouse_dragged_vertex_index = -1;
			}
			
			if(mouse_dragged_vertex_index != -1)
			{
				const int dragged_handle = mouse_dragged_vertex_index % 8;
				Bezier@ curve = @curves[mouse_dragged_vertex_index / 8];
				
				if(dragged_handle == 0)
				{ curve.x1 = mouse_x; curve.y1 = mouse_y; }
				else if(dragged_handle == 2)
				{ curve.x2 = mouse_x; curve.y2 = mouse_y; }
				else if(dragged_handle == 4)
				{ curve.x3 = mouse_x; curve.y3 = mouse_y; }
				else if(dragged_handle == 6)
				{ curve.x4 = mouse_x; curve.y4 = mouse_y; }
			}
		}
		else if(mouse_dragged_vertex_index != -1)
		{
			mouse_dragged_vertex_index = -1;
		}
		
		dragged_vertex_index = -1;
		dragged_vertex_dx = 0;
		dragged_vertex_dy = 0;
		
		int pc_index = 0; // previous_curve_values_index
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			
			if(previous_curve_values[pc_index] != curve.x1 || previous_curve_values[pc_index + 1] != curve.y1)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x1 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y1 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x1;
				previous_curve_values[pc_index + 1] = curve.y1;
				curve.requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x2 || previous_curve_values[pc_index + 1] != curve.y2)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x2 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y2 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x2;
				previous_curve_values[pc_index + 1] = curve.y2;
				curve.requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x3 || previous_curve_values[pc_index + 1] != curve.y3)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x3 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y3 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x3;
				previous_curve_values[pc_index + 1] = curve.y3;
				curve.requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x4 || previous_curve_values[pc_index + 1] != curve.y4)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x4 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y4 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x4;
				previous_curve_values[pc_index + 1] = curve.y4;
				curve.requires_update = true;
			}
			pc_index += 2;
		}
		
		if(dragged_vertex_index != -1)
		{
			const int curve_index = dragged_vertex_index / 8;
			Bezier@ curve = @curves[curve_index]; 
			bool move_x2 = false;
			bool move_x3 = false;
			bool move_next_x2 = false;
			bool move_prev_x3 = false;
			
			// Keep vertices on adjacent curves together, and (if smart handles is on) handles in a straight line to form a smooth join.
			
			const int dragged_handle = dragged_vertex_index % 8;
			
			// x1, y1
			if(dragged_handle == 0)
			{
				// Update x4 on prev curve
				if(curve_index > 0)
				{
					previous_curve_values[dragged_vertex_index - 2] = curves[curve_index - 1].x4 = curve.x1;
					previous_curve_values[dragged_vertex_index - 1] = curves[curve_index - 1].y4 = curve.y1;
					curves[curve_index - 1].requires_update = true;
					move_prev_x3 = true;
				}
				
				move_x2 = true;
			}
			
			// x2, y2
			if(dragged_handle == 2)
			{
				// Update x3 on prev curve
				if(curve_index > 0 && smart_handles)
				{
					float dx1 = curve.x2 - curve.x1;
					float dy1 = curve.y2 - curve.y1;
					const float dx2 = curves[curve_index - 1].x3 - curve.x1;
					const float dy2 = curves[curve_index - 1].y3 - curve.y1;
					const float length1 = sqrt(dx1 * dx1 + dy1 * dy1);
					const float length2 = sqrt(dx2 * dx2 + dy2 * dy2);
					dx1 /= length1;
					dy1 /= length1;
					previous_curve_values[dragged_vertex_index - 6] = curves[curve_index - 1].x3 = curve.x1 - dx1 * length2;
					previous_curve_values[dragged_vertex_index - 5] = curves[curve_index - 1].y3 = curve.y1 - dy1 * length2;
					curves[curve_index - 1].requires_update = true;
				}
			}
			// x3, y3
			if(dragged_handle == 4)
			{
				// Update x2 on next curve
				if(curve_count > curve_index + 1 && smart_handles)
				{
					float dx1 = curve.x3 - curve.x4;
					float dy1 = curve.y3 - curve.y4;
					const float dx2 = curves[curve_index + 1].x2 - curve.x4;
					const float dy2 = curves[curve_index + 1].y2 - curve.y4;
					const float length1 = sqrt(dx1 * dx1 + dy1 * dy1);
					const float length2 = sqrt(dx2 * dx2 + dy2 * dy2);
					dx1 /= length1;
					dy1 /= length1;
					previous_curve_values[dragged_vertex_index + 6] = curves[curve_index + 1].x2 = curve.x4 - dx1 * length2;
					previous_curve_values[dragged_vertex_index + 7] = curves[curve_index + 1].y2 = curve.y4 - dy1 * length2;
					curves[curve_index + 1].requires_update = true;
				}
			}
			// x4, y4
			if(dragged_handle == 6)
			{
				// Update x1 on next curve
				if(curve_count > curve_index + 1)
				{
					previous_curve_values[dragged_vertex_index + 2] = curves[curve_index + 1].x1 = curve.x4;
					previous_curve_values[dragged_vertex_index + 3] = curves[curve_index + 1].y1 = curve.y4;
					curves[curve_index + 1].requires_update = true;
					move_next_x2 = true;
				}
				
				move_x3 = true;
			}
			
			// Keep handles relative to vertices
			if(smart_handles)
			{
				if(move_x2)
				{
					previous_curve_values[dragged_vertex_index + 2] = (curve.x2 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index + 3] = (curve.y2 += dragged_vertex_dy);
				}
				if(move_x3)
				{
					previous_curve_values[dragged_vertex_index - 2] = (curve.x3 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index - 1] = (curve.y3 += dragged_vertex_dy);
				}
				if(move_prev_x3)
				{
					previous_curve_values[dragged_vertex_index - 4] = (curves[curve_index - 1].x3 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index - 3] = (curves[curve_index - 1].y3 += dragged_vertex_dy);
				}
				if(move_next_x2)
				{
					previous_curve_values[dragged_vertex_index + 4] = (curves[curve_index + 1].x2 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index + 5] = (curves[curve_index + 1].y2 += dragged_vertex_dy);
				}
			}
		}
		
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			if(curve.requires_update)
				curve.update();
		}
		
		t++;
	}
	void step()
	{
		editor_step();
	}
	
	void editor_draw(float sub_frame)
	{
		const float segment_length = 5;
		const bool is_selected = self.editor_selected();
		const bool draw = is_selected || ! hide_overlays;
		const int curve_count = int(curves.length());
		
		float total_length = 0;
		const int layer = 22; // prop_def.layer;
		
		if(mouse_over_vertex_index != -1 || mouse_dragged_vertex_index != -1)
		{
			int index = mouse_dragged_vertex_index != -1 ? mouse_dragged_vertex_index : mouse_over_vertex_index;
			const int dragged_handle = index % 8;
			Bezier@ curve = @curves[index / 8];
			
			if(dragged_handle == 0)
				draw_dot(g, layer, 23, curve.x1, curve.y1, 8, 0xFFFFFFFF, 0);
			else if(dragged_handle == 2)
				draw_dot(g, layer, 243, curve.x2, curve.y2, 7, 0xFFFFFFFF, 45);
			else if(dragged_handle == 4)
				draw_dot(g, layer, 23, curve.x3, curve.y3, 7, 0xFFFFFFFF, 45);
			else if(dragged_handle == 6)
				draw_dot(g, layer, 23, curve.x4, curve.y4, 8, 0xFFFFFFFF, 0);
		}
		
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			total_length += curve.length;
			
			if(!draw)
				continue;
				
			float x1 = curve.x1;
			float y1 = curve.y1;
			float x2 = 0, y2 = 0;
			float d = segment_length;
			
			while(d <= curve.length)
			{
				x2 = curve.mx(d);
				y2 = curve.my(d);
				
				g.draw_line_world(layer, 24, x1, y1, x2, y2, 3, 0xFF3333FF);
				
				x1 = x2;
				y1 = y2;
				d += segment_length;
			}
			
			g.draw_line_world(layer, 24, x1, y1, x2, y2, 3, 0xFF3333FF);
			
			g.draw_line_world(layer, 24, curve.x1, curve.y1, curve.x2, curve.y2, 2, 0xFFFF44FF);
			g.draw_line_world(layer, 24, curve.x3, curve.y3, curve.x4, curve.y4, 2, 0xFFFF44FF);
			
			draw_dot(g, layer, 24, curve.x1, curve.y1, 5, 0xFFEE3333, 0);
			draw_dot(g, layer, 24, curve.x2, curve.y2, 4, 0xFFEE4488, 45);
			draw_dot(g, layer, 24, curve.x3, curve.y3, 4, 0xFFFF4444, 45);
			draw_dot(g, layer, 24, curve.x4, curve.y4, 5, 0xFFFF0000, 0);
		}
		
		if(curve_count > 0 && !hide_props)
		{
			const float spacing = prop_def.spacing;
			float d = spacing;
			float total_d = d;
			int curve_index = 0;
			Bezier@ curve = @curves[curve_index++];
			float x1 = curve.x1;
			float y1 = curve.y1;
			
			const float scale_sx = prop_def.scale_sx;
			const float scale_sy = prop_def.scale_sy;
			float d_scale_x = prop_def.scale_ex - scale_sx;
			float d_scale_y = prop_def.scale_ey - scale_sy;
			
			int layer_start, layer_end, layer_count;
			prop_def.calculate_layer_ranges(layer_start, layer_end, layer_count);
			float layer_current = layer_start;
			
			while(true)
			{
				const float t = total_d / total_length;
				const float x2 = curve.mx(d);
				const float y2 = curve.my(d);
				const float angle = atan2(y2 - y1, x2 - x1) * RAD2DEG;
				
				float base_t = (total_d - spacing) / (total_length - spacing);
				int current_layer, current_sub_layer;
				prop_def.calculate_layers(layer_start, layer_count, base_t, current_layer, current_sub_layer);
				
				spr.draw(current_layer, current_sub_layer, prop_def.frame, prop_def.palette,
					x1, y1,
					angle + prop_def.rotation,
					prop_def.scale_x * (scale_sx + d_scale_x * t),
					prop_def.scale_y * (scale_sy + d_scale_y * t),
					0xFFFFFFFF
				);
				
				d += spacing;
				total_d += spacing;
				x1 = x2;
				y1 = y2;
				
				if(d > curve.length)
				{
					if(curve_index >= curve_count)
						break;
						
					d -= curve.length;
					@curve = @curves[curve_index++];
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		editor_draw(sub_frame);
	}

}