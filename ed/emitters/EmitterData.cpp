#include '../../lib/math/Line.cpp';
#include '../../lib/drawing/common.cpp';

class EmitterData
{
	
	scene@ g;
	entity@ emitter;
	varstruct@ vars;
	varvalue@ width_var;
	varvalue@ height_var;
	varvalue@ has_rotation_var;
	varvalue@ rotation_var;
	varvalue@ sub_layer_var;
	
	int layer;
	int sub_layer;
	float x;
	float y;
	float width;
	float height;
	bool has_rotation;
	float rotation;
	float min_x;
	float min_y;
	float max_x;
	float max_y;
	bool is_mouse_over;
	bool is_active;
	
	float hud_x;
	float hud_y;
	float hud_min_x;
	float hud_min_y;
	float hud_max_x;
	float hud_max_y;
	float hud_mid_x;
	float hud_mid_y;
	float x1, y1, x2, y2, x3, y3, x4, y4;
	
	float mouse_x;
	float mouse_y;
	float hud_mouse_x;
	float hud_mouse_y;
	float local_mouse_x;
	float local_mouse_y;
	ResizeMode selected_handle = None;
	ResizeMode hovered_handle = None;
	
	EmitterData(scene@ g)
	{
		@this.g = g;
	}
	
	EmitterData(entity@ emitter)
	{
		update_emitter(@emitter);
	}
	
	void update_emitter(entity@ emitter)
	{
		@this.emitter = @emitter;
		@vars = emitter.vars();
		@width_var = vars.get_var('width');
		@height_var = vars.get_var('height');
		@has_rotation_var = vars.get_var('r_rotation');
		@rotation_var = vars.get_var('e_rotation');
		@sub_layer_var = vars.get_var('draw_depth_sub');
		
		layer = emitter.layer();
		sub_layer = sub_layer_var.get_int32();
		x = emitter.x();
		y = emitter.y();
		width = width_var.get_int32();
		height = height_var.get_int32();
		rotation = rotation_var.get_int32();
		has_rotation = has_rotation_var.get_bool();
		
		update_bounds();
		
		is_mouse_over = false;
		is_active = false;
	}
	
	private void update_bounds()
	{
		min_x = x - width * 0.5;
		min_y = y - height* 0.5;
		max_x = x + width * 0.5;
		max_y = y + height* 0.5;
	}
	
	void update_mouse(const float x, const float y, const float hud_x, const float hud_y, const float zoom)
	{
		mouse_x = x;
		mouse_y = y;
		hud_mouse_x = hud_x;
		hud_mouse_y = hud_y;
		
		rotate(hud_mouse_x - hud_mid_x, hud_mouse_y - hud_mid_y, -rotation * DEG2RAD, local_mouse_x, local_mouse_y);
		local_mouse_x += hud_mid_x;
		local_mouse_y += hud_mid_y;
		
		if(
			local_mouse_x >= hud_min_x - handle_radius * zoom &&
			local_mouse_x <= hud_max_x + handle_radius * zoom &&
			local_mouse_y >= hud_min_y - handle_radius * zoom &&
			local_mouse_y <= hud_max_y + handle_radius * zoom)
		{
			float ox, oy;
			
			is_mouse_over = local_mouse_x >= hud_min_x && local_mouse_x <= hud_max_x && local_mouse_y >= hud_min_y && local_mouse_y <= hud_max_y
				|| check_handles(ox, oy, zoom) != ResizeMode::None;
		}
		else
		{
			is_mouse_over = false;
		}
	}
	
	void update_rotation(float new_rotation)
	{
		has_rotation = new_rotation != 0;
		rotation = new_rotation;
		has_rotation_var.set_bool(has_rotation);
		rotation_var.set_int32(int(new_rotation));
	}
	
	void update_position(float x, float y)
	{
		this.x = x;
		this.y = y;
		emitter.set_xy(x, y);
		update_bounds();
	}
	
	void update_layer(int layer)
	{
		this.layer = clamp(layer, 0, 20);
		emitter.layer(this.layer);
	}
	
	void update_sub_layer(int sub_layer)
	{
		this.sub_layer = clamp(sub_layer, 0, 24);
		sub_layer_var.set_int32(this.sub_layer);
	}
	
	bool update_size(float min_x, float min_y, float max_x, float max_y)
	{
		if(min_x == this.min_x && min_y == this.min_y && max_x == this.max_x && max_y == this.max_y)
			return false;
		
		x = (min_x + max_x) * 0.5;
		y = (min_y + max_y) * 0.5;
		emitter.set_xy(x, y);
		this.min_x = min_x;
		this.min_y = min_y;
		this.max_x = max_x;
		this.max_y = max_y;
		width = ceil(max_x - min_x);
		height = ceil(max_y - min_y);
		width_var.set_int32(int(width));
		height_var.set_int32(int(height));
		
		return true;
	}
	
	ResizeMode check_handles(float &out offset_x, float &out offset_y, const float zoom)
	{
		ResizeMode mode = None;
		
		if(check_handle(TopLeft, hud_min_x, hud_min_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Top, hud_mid_x, hud_min_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(TopRight, hud_max_x, hud_min_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Right, hud_max_x, hud_mid_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(BottomRight, hud_max_x, hud_max_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Bottom, hud_mid_x, hud_max_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(BottomLeft, hud_min_x, hud_max_y, zoom, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Left, hud_min_x, hud_mid_y, zoom, offset_x, offset_y, mode))
			return mode;
		
		return mode;
	}
	
	private bool check_handle(ResizeMode mode, const float x, const float y, const float zoom, float &out offset_x, float &out offset_y, ResizeMode &out result)
	{
		if(
			local_mouse_x >= x - handle_radius * zoom &&
			local_mouse_x <= x + handle_radius * zoom &&
			local_mouse_y >= y - handle_radius * zoom &&
			local_mouse_y <= y + handle_radius * zoom)
		{
			offset_x = (local_mouse_x - x);
			offset_y = (local_mouse_y - y);
			result = mode;
			return true;
		}
		
		result = None;
		return false;
	}
	
	void update_view(float view_x, float view_y)
	{
		transform_layer_position(g, view_x, view_y, x, y, layer, 22, hud_x, hud_y);
		transform_layer_position(g, view_x, view_y, min_x, min_y, layer, 22, hud_min_x, hud_min_y);
		transform_layer_position(g, view_x, view_y, max_x, max_y, layer, 22, hud_max_x, hud_max_y);
		
		float size_x = (hud_max_x - hud_min_x) * 0.5;
		float size_y = (hud_max_y - hud_min_y) * 0.5;
		hud_mid_x = (hud_min_x + hud_max_x) * 0.5;
		hud_mid_y = (hud_min_y + hud_max_y) * 0.5;
		calculate_rotated_rectangle(hud_mid_x, hud_mid_y, size_x, size_y, rotation, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	
	void render_highlight(scene@ g, uint fill_colour, uint outline_colour, RenderParallaxHitbox parallax_hitbox, bool parallax_lines, const float zoom)
	{
		/*
		 * Fill
		 */
		
		g.draw_rectangle_world(
			22, 20,
			hud_min_x, hud_min_y, hud_max_x, hud_max_y,
			rotation,
			fill_colour);
		
		/*
		 * Outline
		 */
		 
		float size_x = (hud_max_x - hud_min_x) * 0.5;
		float size_y = (hud_max_y - hud_min_y) * 0.5;
		float x1, y1, x2, y2, x3, y3, x4, y4;
		calculate_rotated_rectangle(hud_mid_x, hud_mid_y, size_x, size_y, rotation, x1, y1, x2, y2, x3, y3, x4, y4);
		
		draw_line(g, 22, 23, x1, y1, x2, y2, 1 * zoom, outline_colour);
		draw_line(g, 22, 23, x2, y2, x3, y3, 1 * zoom, outline_colour);
		draw_line(g, 22, 23, x3, y3, x4, y4, 1 * zoom, outline_colour);
		draw_line(g, 22, 23, x4, y4, x1, y1, 1 * zoom, outline_colour);
		
		/*
		 * Layer 19 outline
		 */
		
		if(!is_active && layer < 12 && (parallax_hitbox == Always || parallax_hitbox == Hover && is_mouse_over))
		{
			outline_rect(g,
				22, 23, min_x, min_y, max_x, max_y,
				1 * zoom, colours.parallax_outline);
			
			if(parallax_lines)
			{
				draw_line(g, 22, 23, min_x, min_y, hud_min_x, hud_min_y, 1 * zoom, colours.parallax_outline);
				draw_line(g, 22, 23, min_x, max_y, hud_min_x, hud_max_y, 1 * zoom, colours.parallax_outline);
				draw_line(g, 22, 23, max_x, min_y, hud_max_x, hud_min_y, 1 * zoom, colours.parallax_outline);
				draw_line(g, 22, 23, max_x, max_y, hud_max_x, hud_max_y, 1 * zoom, colours.parallax_outline);
			}
		}
	}
	
	void render_layer_text(textfield@ layer_text, Line@ line, const float zoom)
	{
		layer_text.text(layer + '.' + sub_layer);
		
		float x, y;
		float text_rot = rotation;
		
		if(rotation < 45 || rotation > 315)
		{
			x = x1;
			y = y1;
		}
		else if(rotation < 135)
		{
			x = x4;
			y = y4;
			text_rot -= 90;
		}
		else if(rotation < 225)
		{
			x = x3;
			y = y3;
			text_rot -= 180;
		}
		else
		{
			x = x2;
			y = y2;
			text_rot -= 270;
		}
		
		shadowed_text_world(layer_text,
			22, 24, x + 6 * zoom, y - 14 * zoom,
			zoom, zoom, text_rot,
			colours.layer_shadow, 2 * zoom, 2 * zoom);
	}
	
	void render_active_layer_text(textfield@ layer_text, float x, float y, bool layer_active, const float zoom)
	{
		const string layer_str = layer + '.';
		const string text = layer + '.' + sub_layer;
		
		layer_text.text(text);
		const float width = layer_text.text_width() * zoom;
		layer_text.text(layer + '');
		const float layer_x = -width * 0.5 + layer_text.text_width() * zoom * 0.5;
		layer_text.text(sub_layer + '');
		const float sub_layer_x =  width * 0.5 - layer_text.text_width() * zoom * 0.5;
		
		layer_text.text(text);
		const float offset = 30;
		const float height = layer_text.text_height();
		shadowed_text_world(layer_text,
			22, 24, x, y + offset * zoom,
			zoom, zoom, 0,
			colours.active_layer_shadow, 2 * zoom, 2 * zoom);
		
		layer_text.text('_');
		shadowed_text_world(layer_text,
			22, 24,
			layer_active ? x + layer_x : x + sub_layer_x,
			y + (offset + 5) * zoom,
			zoom, zoom, 0,
			colours.active_layer_shadow, 2 * zoom, 2 * zoom);
		shadowed_text_world(layer_text,
			22, 24,
			layer_active ? x + layer_x : x + sub_layer_x,
			y + (offset - height - 7) * zoom,
			zoom, zoom, 0,
			colours.active_layer_shadow, 2 * zoom, 2 * zoom);
	}
	
	void render_rotation(scene@ g, const float zoom)
	{
		if(!has_rotation || rotation == 0)
			return;
		
		const float thickness = 1;
		float radius = min(24.0, width * 0.5) * zoom;
		float outer_radius = radius * 1.5;
		float angle = rotation * DEG2RAD;
		
		draw_arc(g,
			hud_x, hud_y, radius, radius,
			0, rotation, 64,
			22, 23,
			thickness * zoom, colours.rotation_indicator_secondary);
		draw_line(g,
			22, 23, 
			hud_x, hud_y,
			hud_x + outer_radius, hud_y,
			thickness * zoom, colours.rotation_indicator_secondary);
		draw_line(g,
			22, 23, 
			hud_x, hud_y,
			hud_x + cos(angle) * outer_radius, hud_y + sin(angle) * outer_radius,
			thickness * zoom, colours.rotation_indicator);
	}
	
	void render_handles(scene@ g, const float zoom)
	{
		const float tx = (x1 + x2) * 0.5;
		const float ty = (y1 + y2) * 0.5;
		const float rx = (x2 + x3) * 0.5;
		const float ry = (y2 + y3) * 0.5;
		const float bx = (x3 + x4) * 0.5;
		const float by = (y3 + y4) * 0.5;
		const float lx = (x4 + x1) * 0.5;
		const float ly = (y4 + y1) * 0.5;
		
		render_handle(g, TopLeft, x1, y1, zoom);
		render_handle(g, Top, tx, ty, zoom);
		render_handle(g, TopRight, x2, y2, zoom);
		render_handle(g, Right, rx, ry, zoom);
		render_handle(g, BottomRight, x3, y3, zoom);
		render_handle(g, Bottom, bx, by, zoom);
		render_handle(g, BottomLeft, x4, y4, zoom);
		render_handle(g, Left, lx, ly, zoom);
	}
	
	private void render_handle(scene@ g, ResizeMode handle, const float x, const float y, const float zoom)
	{
		g.draw_rectangle_world(
			22, 24,
			x - handle_radius * zoom, y - handle_radius * zoom,
			x + handle_radius * zoom, y + handle_radius * zoom,
			rotation,
			handle == selected_handle || handle == hovered_handle ? colours.handle_selected : colours.handle);
	}
	
}
