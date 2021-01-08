#include '../../misc/SelectableData.cpp';

class PropData : SelectableData
{
	
	PropTool@ tool;
	prop@ prop;
	
	const array<array<float>>@ outline;
	
	private float drag_start_x, drag_start_y;
	private float rotate_start_angle;
	private float rotate_offset_angle;
	private float drag_start_scale_x, drag_start_scale_y;
	
	float anchor_x, anchor_y;
	
	float local_x1, local_y1;
	float local_x2, local_y2;
	
	private sprites@ spr;
	
	private string sprite_name;
	float x, y;
	private float angle;
	private float layer_scale;
	private float backdrop_scale;
	private float draw_scale;
	private float draw_scale_x, draw_scale_y;
	
	private float align_x, align_y;
	private float prop_left, prop_top;
	private float prop_width, prop_height;
	private float prop_offset_x, prop_offset_y;
	private float prop_scale_x, prop_scale_y;
	
	private int lines_size = 32;
	private int lines_count;
	private array<float> lines(lines_size);
	
	void init(AdvToolScript@ script, PropTool@ tool, prop@ prop, const array<array<float>>@ outline)
	{
		SelectableData::init(script, prop.id() + '', scene_index);
		
		@this.tool = tool;
		@this.prop = prop;
		@this.outline = outline;
		string sprite_set;
		sprite_from_prop(@prop, sprite_set, sprite_name);
		
		if(@spr == null)
		{
			@spr = create_sprites();
		}
		
		spr.add_sprite_set(sprite_set);
		
		align_x = tool.origin_align_x;
		align_y = tool.origin_align_y;
		
		update();
		init_prop();
	}
	
	void step()
	{
		script.transform(x, y, prop.layer(), 22, aabb_x, aabb_y);
	}
	
	int draw(const int rendered_lines_count)
	{
		if(pending_selection == -2)
			return 0;
		
		float line_width;
		uint line_colour, fill_colour;
		get_colours(line_width, line_colour, fill_colour);
		
		spr.draw_world(22, 22, sprite_name, 0, prop.palette(),
			aabb_x, aabb_y, prop.rotation(),
			draw_scale_x, draw_scale_y,
			fill_colour);
		
		// Stop drawing outlines after some amount to prevent a black screen
		if(rendered_lines_count < 16000 || primary_selected || hovered)
		{
			for(int i = lines_count - 4; i >= 0; i -= 4)
			{
				script.g.draw_rectangle_world(22, 22,
					aabb_x + lines[i] - line_width, aabb_y + lines[i + 1] - lines[i + 2] * 0.5,
					aabb_x + lines[i] + line_width, aabb_y + lines[i + 1] + lines[i + 2] * 0.5,
					lines[i + 3],
					line_colour);
			}
			
			return lines_count;
		}
		
//		if(selected)
//		{
//			outline_rect(script.g,22,22,
//				aabb_x + aabb_x1, aabb_y + aabb_y1,
//				aabb_x + aabb_x2, aabb_y + aabb_y2,
//				1 / script.zoom, 0xaaff0000);
//		}
		
		return 0;
	}
	
	void update()
	{
		prop_scale_x = prop.scale_x();
		prop_scale_y = prop.scale_y();
		x = prop.x();
		y = prop.y();
		
		script.transform(x, y, prop.layer(), 22, aabb_x, aabb_y);
		
		angle = prop.rotation() * DEG2RAD * sign(prop_scale_x) * sign(prop_scale_y);
		layer_scale = prop.layer() <= 5 ? script.g.layer_scale(prop.layer()) : 1.0;
		backdrop_scale = prop.layer() <= 5 ? 2.0 : 1.0;
		draw_scale = get_layer_scale(script.g, prop.layer(), 22);
		
		const float cos_angle = cos(angle);
		const float sin_angle = sin(angle);
		
		draw_scale_x = prop_scale_x / layer_scale * backdrop_scale;
		draw_scale_y = prop_scale_y / layer_scale * backdrop_scale;
		
		lines_count = 0;
		
		local_x1 = local_y1 = 0;
		local_x2 = local_y2 = 0;
		
		for(int i = 0, path_count = int(outline.length()); i < path_count; i++)
		{
			const array<float>@ path = @outline[i];
			const int count = int(path.length());
			
			if(lines_count + count * 2 >= lines_size)
			{
				lines.resize(lines_size = lines_count + count * 2 + 32);
			}
			
			float p_x = path[count - 2];
			float p_y = path[count - 1];
			float prev_x = (cos_angle * p_x - sin_angle * p_y) * draw_scale_x;
			float prev_y = (sin_angle * p_x + cos_angle * p_y) * draw_scale_y;
			
			if(i == 0)
			{
				local_x1 = local_x2 = prev_x;
				local_y1 = local_y2 = prev_y;
			}
			
			prev_x *= draw_scale;
			prev_y *= draw_scale;
			
			for(int j = 0, k = count - 2; j < count; k = j, j += 2)
			{
				p_x = path[j];
				p_y = path[j + 1];
				
				float x = (cos_angle * p_x - sin_angle * p_y) * draw_scale_x;
				float y = (sin_angle * p_x + cos_angle * p_y) * draw_scale_y;
				
				if(x < local_x1) local_x1 = x;
				if(x > local_x2) local_x2 = x;
				if(y < local_y1) local_y1 = y;
				if(y > local_y2) local_y2 = y;
				
				x *= draw_scale;
				y *= draw_scale;
				const float dx = x - prev_x;
				const float dy = y - prev_y;
				
				lines[lines_count++] = (prev_x + x) * 0.5; // mx
				lines[lines_count++] = (prev_y + y) * 0.5; // my
				lines[lines_count++] = sqrt(dx * dx + dy * dy); // length
				lines[lines_count++] = atan2(-dx, dy) * RAD2DEG; // angle
				
				prev_x = x;
				prev_y = y;
			}
		}
		
		script.transform_size(local_x1, local_y1, prop.layer(), 22, aabb_x1, aabb_y1);
		script.transform_size(local_x2, local_y2, prop.layer(), 22, aabb_x2, aabb_y2);
		
		draw_scale_x *= draw_scale;
		draw_scale_y *= draw_scale;
	}
	
	void set_prop_rotation(const float rotation)
	{
		float ox, oy;
		rotate(prop_offset_x, prop_offset_y, rotation * DEG2RAD, ox, oy);
		
		x = anchor_x - ox;
		y = anchor_y - oy;
		
		prop.rotation(rotation % 360);
		prop.x(x);
		prop.y(y);
	}
	
	void anchor_world(float world_x, float world_y)
	{
		const float scale_x = prop_scale_x / layer_scale * backdrop_scale;
		const float scale_y = prop_scale_y / layer_scale * backdrop_scale;
		
		rotate(world_x - x, world_y - y, -prop.rotation() * DEG2RAD, world_x, world_y);
		align_x = (world_x - prop_left * scale_x) / (prop_width * scale_x);
		align_y = (world_y - prop_top * scale_y)  / (prop_height * scale_y);
		
		init_anchors();
	}
	
	void shift_layer(const int dir, const bool sublayer=false)
	{
		if(sublayer)
		{
			prop.sub_layer(clamp(prop.sub_layer() + dir, 0, 24));
		}
		else
		{
			const int new_layer = clamp(prop.layer() + dir, 0, 20);
			
			if(new_layer <= 5 && prop.layer() > 5 || new_layer > 5 && prop.layer() <= 5)
			{
				script.g.remove_prop(prop);
				prop.layer(new_layer);
				script.g.add_prop(prop);
			}
			else
			{
				prop.layer(new_layer);
			}
		}
		
		update();
	}
	
	private void init_prop()
	{
		rectangle@ r = spr.get_sprite_rect(sprite_name, 0);
		prop_left = r.left();
		prop_top = r.top();
		prop_width = r.get_width();
		prop_height = r.get_height();
		
		init_anchors();
	}
	
	void init_anchors()
	{
		prop_offset_x = (prop_left + prop_width * align_x) * prop_scale_x  / layer_scale * backdrop_scale;
		prop_offset_y = (prop_top  + prop_height * align_y) * prop_scale_y / layer_scale * backdrop_scale;
		
		rotate(prop_offset_x, prop_offset_y, prop.rotation() * DEG2RAD, anchor_x, anchor_y);
		
		anchor_x += x;
		anchor_y += y;
	}
	
	//
	
	void start_drag()
	{
		drag_start_x = x;
		drag_start_y = y;
	}
	
	void do_drag(const float drag_delta_x, const float drag_delta_y)
	{
		x = drag_start_x + drag_delta_x;
		y = drag_start_y + drag_delta_y;
		
		init_anchors();
		
		prop.x(x);
		prop.y(y);
	}
	
	void cancel_drag()
	{
		x = drag_start_x;
		y = drag_start_y;
		
		prop.x(x);
		prop.y(y);
	}
	
	void move(const float dx, const float dy)
	{
		x += dx;
		y += dy;
		
		init_anchors();
		
		prop.x(x);
		prop.y(y);
	}
	
	//
	
	void start_rotate(const float anchor_x, const float anchor_y, const float base_rotation)
	{
		rotate_start_angle = prop.rotation();
		rotate_offset_angle = prop.rotation() - base_rotation;
		anchor_world(anchor_x, anchor_y);
		
		start_drag();
	}
	
	void do_rotation(const float angle)
	{
		set_prop_rotation(rotate_offset_angle + angle);
	}
	
	void stop_rotate(const bool cancel)
	{
		if(cancel)
		{
			prop.rotation(rotate_start_angle);
			cancel_drag();
		}
		
		align_x = tool.origin_align_x;
		align_y = tool.origin_align_y;
		init_anchors();
		
		update();
	}
	
	//
	
	void start_scale(const float anchor_x, const float anchor_y)
	{
		drag_start_scale_x = prop.scale_x();
		drag_start_scale_y = prop.scale_y();
		anchor_world(anchor_x, anchor_y);
		
		start_drag();
	}
	
	void do_scale(const float scale_x, const float scale_y)
	{
		prop_scale_x = drag_start_scale_x * scale_x;
		prop_scale_y = drag_start_scale_y * scale_y;
		
		if(prop_scale_x == 0)
			prop_scale_x = 0.001;
		if(prop_scale_y == 0)
			prop_scale_y = 0.001;
		
		prop.scale_x(prop_scale_x);
		prop.scale_y(prop_scale_y);
		
		float ox, oy;
		rotate(prop_offset_x * scale_x, prop_offset_y * scale_y, prop.rotation() * DEG2RAD, ox, oy);
		
		x = anchor_x - ox;
		y = anchor_y - oy;
		
		prop.x(x);
		prop.y(y);
	}
	
	void stop_scale(const bool cancel)
	{
		if(cancel)
		{
			prop.scale_x(drag_start_scale_x);
			prop.scale_y(drag_start_scale_y);
			cancel_drag();
		}
		
		align_x = tool.origin_align_x;
		align_y = tool.origin_align_y;
		
		init_anchors();
		update();
	}
	
}
