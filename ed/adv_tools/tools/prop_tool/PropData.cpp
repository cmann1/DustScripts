class PropData
{
	
	AdvToolScript@ script;
	prop@ prop;
	string key;
	bool hovered;
	bool selected;
	int pending;
	
	const array<array<float>>@ outline;
	
	float draw_x, draw_y;
	float x, y;
	float x1, y1;
	float x2, y2;
	
	private float drag_start_x, drag_start_y;
	private float rotate_start_angle;
	private float rotate_offset_angle;
	private float drag_start_scale_x, drag_start_scale_y;
	
	float anchor_x, anchor_y;
	
	private sprites@ spr;
	
	private string sprite_name;
	private float draw_scale_x, draw_scale_y;
	private float angle;
	private float layer_scale;
	private float backdrop_scale;
	private float draw_scale;
	
	private float align_x, align_y;
	private float prop_left, prop_top;
	private float prop_width, prop_height;
	private float prop_offset_x, prop_offset_y;
	private float prop_scale_x, prop_scale_y;
	
	private int lines_size = 32;
	private int lines_count;
	private array<float> lines(lines_size);
	
	void init(AdvToolScript@ script)
	{
		@this.script = script;
		string sprite_set;
		sprite_from_prop(@prop, sprite_set, sprite_name);
		
		if(@spr == null)
		{
			@spr = create_sprites();
		}
		
		spr.add_sprite_set(sprite_set);
		
		align_x = 0.5;
		align_y = 0.5;
		
		update();
		init_prop();
	}
	
	void step()
	{
		transform_layer_position(script.g, script.view_x, script.view_y, this.x, this.y, prop.layer(), 22, draw_x, draw_y);
	}
	
	void draw()
	{
		if(pending == -2)
			return;
		
		spr.draw_world(22, 22, sprite_name, 0, prop.palette(),
			draw_x, draw_y, prop.rotation(),
			draw_scale_x, draw_scale_y,
			pending == 1 ? PropToolSettings::PendingAddOverlayColour : pending == -1 ? PropToolSettings::PendingRemoveOverlayColour
				: hovered
					? PropToolSettings::HighlightOverlayColour
					: PropToolSettings::SelectOverlayColour);
		
		const float width = (pending == 1 ? PropToolSettings::PendingAddOutlineWidth : pending == -1 ? PropToolSettings::PendingRemoveOutlineWidth
			: hovered
				? PropToolSettings::HighlightOutlineWidth
				: PropToolSettings::SelectedOutlineWidth) / script.zoom;
		const uint colour = pending == 1 ? PropToolSettings::PendingAddOutlineColour : pending == -1 ? PropToolSettings::PendingRemoveOutlineColour
			: hovered
				? PropToolSettings::HighlightOutlineColour
				: PropToolSettings::SelectedOutlineColour;
		
		for(int i = lines_count - 4; i >= 0; i -= 4)
		{
			script.g.draw_rectangle_world(22, 22,
				draw_x + lines[i] - width, draw_y + lines[i + 1] - lines[i + 2] * 0.5,
				draw_x + lines[i] + width, draw_y + lines[i + 1] + lines[i + 2] * 0.5,
				lines[i + 3],
				colour);
		}
	}
	
	void update()
	{
		prop_scale_x = prop.scale_x();
		prop_scale_y = prop.scale_y();
		x = prop.x();
		y = prop.y();
		
		transform_layer_position(script.g, script.view_x, script.view_y, this.x, this.y, prop.layer(), 22, draw_x, draw_y);
		
		angle = prop.rotation() * DEG2RAD * sign(prop_scale_x) * sign(prop_scale_y);
		layer_scale = prop.layer() <= 5 ? script.g.layer_scale(prop.layer()) : 1.0;
		backdrop_scale = prop.layer() <= 5 ? 2.0 : 1.0;
		draw_scale = get_layer_scale(script.g, prop.layer(), 22);
		
		const float cos_angle = cos(angle);
		const float sin_angle = sin(angle);
		
		draw_scale_x = prop_scale_x / layer_scale * backdrop_scale;
		draw_scale_y = prop_scale_y / layer_scale * backdrop_scale;
		
		lines_count = 0;
		
		x1 = y1 = 0;
		x2 = y2 = 0;
		
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
				x1 = x2 = prev_x;
				y1 = y2 = prev_y;
			}
			
			prev_x *= draw_scale;
			prev_y *= draw_scale;
			
			for(int j = 0, k = count - 2; j < count; k = j, j += 2)
			{
				p_x = path[j];
				p_y = path[j + 1];
				
				float x = (cos_angle * p_x - sin_angle * p_y) * draw_scale_x;
				float y = (sin_angle * p_x + cos_angle * p_y) * draw_scale_y;
				
				if(x < x1) x1 = x;
				if(x > x2) x2 = x;
				if(y < y1) y1 = y;
				if(y > y2) y2 = y;
				
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
			prop.layer(clamp(prop.layer() + dir, 0, 20));
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
	
	private void init_anchors()
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
		
		align_x = 0.5;
		align_y = 0.5;
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
	
	void do_scale(const float scale)
	{
		prop_scale_x = max(drag_start_scale_x * scale, 0.001);
		prop_scale_y = max(drag_start_scale_y * scale, 0.001);
		prop.scale_x(prop_scale_x);
		prop.scale_y(prop_scale_y);
		
		float ox, oy;
		rotate(prop_offset_x * scale, prop_offset_y * scale, prop.rotation() * DEG2RAD, ox, oy);
		
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
		
		align_x = 0.5;
		align_y = 0.5;
		init_anchors();
		
		update();
	}
	
}