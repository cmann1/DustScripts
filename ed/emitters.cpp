#include "../lib/std.cpp";
#include "../lib/math/math.cpp";
#include "../lib/enums/ColType.cpp";
#include "../lib/print_vars.cpp";
#include "../lib/drawing/common.cpp";
#include "../lib/drawing/circle.cpp";
#include "../lib/layer.cpp";

const float handle_radius = 5;

const Colours colours;
class Colours
{
	
	uint normal_fill = 0x33ffffff;
	uint normal_outline = 0x44FFFFFF;
	uint selected_fill = 0x4400FF00;
	uint selected_outline = 0x6600ff00;
	uint handle = 0xeeee4444;
	uint handle_selected = 0xeeeeee44;
	uint rotation_indicator = 0x882222ee;
	uint rotation_indicator_secondary = 0x66ffffff;
	
	uint layer_fill = 0xaaffffff;
	uint layer_shadow = 0x88000000;
	uint active_layer_fill = 0xffffffff;
	uint active_layer_shadow = 0xee000000;
	
}

class script
{
	
	[text] bool enabled = true;
	
	private scene@ g;
	private textfield@ layer_text;
	private textfield@ active_layer_text;
	private camera@ cam;
	
	private DragMode dragMode = None;
	private float drag_angle_offset;
	private float handle_offset_x;
	private float handle_offset_y;
	private float resize_min_x;
	private float resize_min_y;
	private float resize_max_x;
	private float resize_max_y;
	private EmitterData@ validate_emitter = null;
	
	private array<EmitterData@> highlighted_emitters;
	private EmitterData@ hovered_emitter;
	private EmitterData@ active_emitter;
	
	private bool prev_left_mouse_down;
	private bool prev_right_mouse_down;
	private bool prev_middle_mouse_down;
	
	private bool left_mouse_down;
	private bool right_mouse_down;
	private bool middle_mouse_down;
	
	private bool left_mouse_press;
	private bool right_mouse_press;
	private bool middle_mouse_press;
	
	private float mouse_x;
	private float mouse_y;
	private int mouse_scroll;
	private int mouse_state;
	
	script()
	{
		@g = get_scene();
		@cam = get_active_camera();
		
		@layer_text = create_textfield();
        layer_text.align_horizontal(-1);
        layer_text.align_vertical(1);
		layer_text.colour(colours.layer_fill);
		
		@active_layer_text = create_textfield();
        active_layer_text.align_horizontal(0);
        active_layer_text.align_vertical(-1);
		active_layer_text.colour(colours.active_layer_fill);
	}
	
	void editor_step()
	{
		if(!enabled)
			return;
		
		if(@validate_emitter != null)
		{
			g.remove_entity(validate_emitter.emitter);
			g.add_entity(validate_emitter.emitter);
			@validate_emitter = null;
		}
		
		float view_x = cam.x();
		float view_y = cam.y();
		
		update_mouse();
		find_emitters(view_x, view_y);
		
		if(@active_emitter == null && @hovered_emitter != null)
		{
			if(right_mouse_press)
			{
				ResizeMode mode = hovered_emitter.check_handles(handle_offset_x, handle_offset_y);
				
				if(mode != None)
				{
					dragMode = Resize;
					hovered_emitter.selected_handle = mode;
					resize_min_x = hovered_emitter.min_x;
					resize_min_y = hovered_emitter.min_y;
					resize_max_x = hovered_emitter.max_x;
					resize_max_y = hovered_emitter.max_y;
				}
				else
				{
					handle_offset_x = hovered_emitter.mouse_x - hovered_emitter.x;
					handle_offset_y = hovered_emitter.mouse_y - hovered_emitter.y;
					dragMode = Move;
				}
			}
			else if(middle_mouse_press)
			{
				float dx = hovered_emitter.mouse_x - hovered_emitter.x;
				float dy = hovered_emitter.mouse_y - hovered_emitter.y;
				drag_angle_offset = shortest_angle(hovered_emitter.rotation * DEG2RAD, atan2(dy, dx));
				dragMode = Rotation;
			}
			else
			{
				hovered_emitter.hovered_handle = hovered_emitter.check_handles(handle_offset_x, handle_offset_y);
			}
			
			if(dragMode != None)
			{
				@active_emitter = @hovered_emitter;
			}
		}
		
		if(@active_emitter != null)
		{
			@hovered_emitter = null;
			active_emitter.set_view(view_x, view_y);
			
			bool requires_update = false;
			bool mouse_button;
			
			switch(dragMode)
			{
				case Rotation:
					update_rotation();
					mouse_button = middle_mouse_down;
					break;
				case Move:
					update_position();
					mouse_button = right_mouse_down;
					break;
				case Resize:
					update_size();
					mouse_button = right_mouse_down;
					break;
			}
			
			if(!mouse_button)
			{
				dragMode = None;
				active_emitter.selected_handle = None;
				
				if(active_emitter.is_mouse_over)
				{
					@hovered_emitter = @active_emitter;
				}
				
				@active_emitter = null;
			}
		}
		
		// Remove the emitter and add it again the next frame to force the modified vars to reflect in the editor
		if(@validate_emitter != null)
		{
			g.remove_entity(validate_emitter.emitter);
		}
	}
	
	void update_mouse()
	{
		mouse_x = g.mouse_x_world(0, 19);
		mouse_y = g.mouse_y_world(0, 19);
		mouse_state = g.mouse_state(0);
		mouse_scroll = (mouse_state & 1 != 0) ? -1 : ((mouse_state & 2 != 0) ? 1 : 0);
		
		left_mouse_down = (mouse_state & 4) != 0;
		right_mouse_down = (mouse_state & 8) != 0;
		middle_mouse_down = (mouse_state & 16) != 0;
		
		left_mouse_press = left_mouse_down && !prev_left_mouse_down;
		right_mouse_press = right_mouse_down && !prev_right_mouse_down;
		middle_mouse_press = middle_mouse_down && !prev_middle_mouse_down;
		
		prev_left_mouse_down = left_mouse_down;
		prev_right_mouse_down = right_mouse_down;
		prev_middle_mouse_down = middle_mouse_down;
	}
	
	void find_emitters(float view_x, float view_y)
	{
		@hovered_emitter = null;
		highlighted_emitters.resize(0);
		
		const float closest_radius = 400;
		
		for(int layer = 0; layer <= 22; layer++)
		{
			float layer_mouse_x = g.mouse_x_world(0, layer);
			float layer_mouse_y = g.mouse_y_world(0, layer);
			
			float layer_closest_radius = closest_radius * get_layer_scale(22, layer);
			
			int count = g.get_entity_collision(
				layer_mouse_y - layer_closest_radius, layer_mouse_y + layer_closest_radius,
				layer_mouse_x - layer_closest_radius, layer_mouse_x + layer_closest_radius,
				ColType::Emitter);
			
			for(int i = 0; i < count; i++)
			{
				entity@ emitter = g.get_entity_collision_index(i);
				
				if(emitter is null)
					continue;
				
				if(emitter.layer() != layer)
					continue;
				
				if(@active_emitter != null && emitter.is_same(@active_emitter.emitter))
				{
					active_emitter.set_view(view_x, view_y);
					active_emitter.update_mouse(layer_mouse_x, layer_mouse_y, mouse_x, mouse_y);
					continue;
				}
				
				EmitterData@ data = EmitterData();
				data.update_emitter(emitter);
				data.set_view(view_x, view_y);
				data.update_mouse(layer_mouse_x, layer_mouse_y, mouse_x, mouse_y);
				highlighted_emitters.insertLast(@data);
				
				if(data.is_mouse_over)
				{
					@hovered_emitter = data;
				}
			}
		}
	}
	
	void update_rotation()
	{
		if(@active_emitter == null)
			return;
		
		float dx = active_emitter.mouse_x - active_emitter.x;
		float dy = active_emitter.mouse_y - active_emitter.y;
		
		int new_rotation = round_int((atan2(dy, dx) - drag_angle_offset) * RAD2DEG) % 360;
		
		if(new_rotation < 0)
			new_rotation = 360 + new_rotation;
		
		if((!active_emitter.has_rotation && new_rotation != 0) || (active_emitter.has_rotation && new_rotation == 0) || new_rotation != active_emitter.rotation)
		{
			active_emitter.update_rotation(new_rotation);
			@validate_emitter = @active_emitter;
		}
	}
	
	void update_position()
	{
		if(@active_emitter == null)
			return;
		
		active_emitter.update_position(active_emitter.mouse_x - handle_offset_x, active_emitter.mouse_y - handle_offset_y);
		
		// TODO: Some way to adjust layer
		if(mouse_scroll != 0)
		{
			active_emitter.update_sub_layer(active_emitter.sub_layer - mouse_scroll);
			@validate_emitter = @active_emitter;
		}
	}
	
	void update_size()
	{
		if(@active_emitter == null)
			return;
		
		float min_x = resize_min_x;
		float min_y = resize_min_y;
		float max_x = resize_max_x;
		float max_y = resize_max_y;
		const float mouse_x = active_emitter.mouse_x;
		const float mouse_y = active_emitter.mouse_y;
		
		const float scale = get_layer_scale(22, active_emitter.layer);
		
		const float offset_x = handle_offset_x * scale;
		const float offset_y = handle_offset_y * scale;
		const float handle_layer_radius = handle_radius * scale;
		
		switch(active_emitter.selected_handle)
		{
			case TopLeft:
				min_x = mouse_x - offset_x - handle_layer_radius;
				min_y = mouse_y - offset_y - handle_layer_radius;
				break;
			case Top:
				min_y = mouse_y - offset_y - handle_layer_radius;
				break;
			case TopRight:
				max_x = mouse_x - offset_x + handle_layer_radius;
				min_y = mouse_y - offset_y - handle_layer_radius;
				break;
			case BottomLeft:
				min_x = mouse_x - offset_x - handle_layer_radius;
				max_y = mouse_y - offset_y + handle_layer_radius;
				break;
			case Bottom:
				max_y = mouse_y - offset_y + handle_layer_radius;
				break;
			case BottomRight:
				max_x = mouse_x - offset_x + handle_layer_radius;
				max_y = mouse_y - offset_y + handle_layer_radius;
				break;
			case Left:
				min_x = mouse_x - offset_x - handle_layer_radius;
				break;
			case Right:
				max_x = mouse_x - offset_x + handle_layer_radius;
				break;
		}
		
		if(min_x > max_x)
		{
			float temp = min_x;
			min_x = max_x;
			max_x = temp;
		}
		
		if(min_y > max_y)
		{
			float temp = min_y;
			min_y = max_y;
			max_y = temp;
		}
		
		if(active_emitter.update_size(min_x, min_y, max_x, max_y))
		{
			@validate_emitter = @active_emitter;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(!enabled)
			return;
		
		for(int i = int(highlighted_emitters.length()) - 1; i >= 0; i--)
		{
			EmitterData@ data = @highlighted_emitters[i];
			
			
			if(@data == @hovered_emitter || @data == @active_emitter)
				continue;
			
			data.render_highlight(g, colours.normal_fill, colours.normal_outline);
		}
		
		EmitterData@ active_data = @active_emitter != null ? @active_emitter : @hovered_emitter;
		
		if(@active_data != null)
		{
			active_data.render_highlight(g, colours.selected_fill, colours.selected_outline);
			active_data.render_rotation(g);
			active_data.render_handles(g);
			
			if(dragMode == Move)
			{
				active_data.render_active_layer_text(active_layer_text, mouse_x, mouse_y);
			}
			else
			{
				active_data.render_layer_text(layer_text);
			}
		}
	}
	
}

class EmitterData
{
	
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
	
	private float hud_x;
	private float hud_y;
	private float hud_min_x;
	private float hud_min_y;
	private float hud_max_x;
	private float hud_max_y;
	
	float mouse_x;
	float mouse_y;
	float hud_mouse_x;
	float hud_mouse_y;
	ResizeMode selected_handle = None;
	ResizeMode hovered_handle = None;
	
	EmitterData()
	{
		
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
	
	void update_mouse(float x, float y, float hud_x, float hud_y)
	{
		// TODO: Take rotation into account for hit test
		mouse_x = x;
		mouse_y = y;
		hud_mouse_x = hud_x;
		hud_mouse_y = hud_y;
		is_mouse_over = hud_x >= hud_min_x && hud_x <= hud_max_x && hud_y >= hud_min_y && hud_y <= hud_max_y;
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
	
	ResizeMode check_handles(float &out offset_x, float &out offset_y)
	{
		ResizeMode mode = None;
		
		float mid_x = (hud_min_x + hud_max_x) * 0.5;
		float mid_y = (hud_min_y + hud_max_y) * 0.5;
		
		if(check_handle(TopLeft, hud_min_x, hud_min_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Top, mid_x, hud_min_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(TopRight, hud_max_x, hud_min_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Right, hud_max_x, mid_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(BottomRight, hud_max_x, hud_max_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Bottom, mid_x, hud_max_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(BottomLeft, hud_min_x, hud_max_y, offset_x, offset_y, mode))
			return mode;
		if(check_handle(Left, hud_min_x, mid_y, offset_x, offset_y, mode))
			return mode;
		
		return mode;
	}
	
	private bool check_handle(ResizeMode mode, float x, float y, float &out offset_x, float &out offset_y, ResizeMode &out result)
	{
		calculate_handle_centre(mode, x, y, x, y);
		
		if(hud_mouse_x >= x - handle_radius && hud_mouse_x <= x + handle_radius && hud_mouse_y >= y - handle_radius && hud_mouse_y <= y + handle_radius)
		{
			offset_x = (hud_mouse_x - x);
			offset_y = (hud_mouse_y - y);
			result = mode;
			return true;
		}
		
		result = None;
		return false;
	}
	
	private void calculate_handle_centre(ResizeMode mode, float x, float y, float &out out_x, float &out out_y)
	{
		switch(mode)
		{
			case TopLeft:
			case Left:
			case BottomLeft:
				x += handle_radius;
				break;
			case TopRight:
			case Right:
			case BottomRight:
				x -= handle_radius;
				break;
		}
		
		switch(mode)
		{
			case TopLeft:
			case Top:
			case TopRight:
				y += handle_radius;
				break;
			case BottomLeft:
			case Bottom:
			case BottomRight:
				y -= handle_radius;
				break;
		}
		
		out_x = x;
		out_y = y;
	}
	
	void set_view(float view_x, float view_y)
	{
		transform_layer_position(view_x, view_y, x, y, layer, 22, hud_x, hud_y);
		transform_layer_position(view_x, view_y, min_x, min_y, layer, 22, hud_min_x, hud_min_y);
		transform_layer_position(view_x, view_y, max_x, max_y, layer, 22, hud_max_x, hud_max_y);
	}
	
	void render_highlight(scene@ g, uint fill_colour, uint outline_colour)
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
		 * Unrotated outline
		 */
		 
		if(rotation != 0 && layer < 12)
		{
			outline_rect(g,
				hud_min_x, hud_min_y, hud_max_x, hud_max_y,
				22, 23, 1, outline_colour);
		}
		
		// Layer 19 outline
		// TODO: Option to render always or only when hovered/active
		outline_rect(g,
			min_x, min_y, max_x, max_y,
			22, 23, 1, outline_colour);
		
		// TODO: Option to draw lines connecting non-parallax outline
	}
	
	void render_layer_text(textfield@ layer_text)
	{
		layer_text.text(layer + '.' + sub_layer);
		shadowed_text_world(layer_text,
			22, 24, hud_min_x, hud_min_y - 8,
			1, 1, 0,
			colours.layer_shadow, 2, 2);
	}
	
	void render_active_layer_text(textfield@ layer_text, float x, float y)
	{
		layer_text.text(layer + '.' + sub_layer);
		shadowed_text_world(layer_text,
			22, 24, x, y + 30,
			1, 1, 0,
			colours.active_layer_shadow, 2, 2);
	}
	
	void render_rotation(scene@ g)
	{
		if(!has_rotation || rotation == 0)
			return;
		
		const float thickness = 2;
		float radius = min(24, width * 0.5);
		float outer_radius = radius * 1.5;
		float angle = rotation * DEG2RAD;
		
		draw_arc(g,
			hud_x, hud_y, radius, radius,
			0, rotation, 64,
			22, 23,
			thickness, colours.rotation_indicator_secondary);
		g.draw_line(
			22, 23, 
			hud_x, hud_y,
			hud_x + outer_radius, hud_y,
			thickness, colours.rotation_indicator_secondary);
		g.draw_line(
			22, 23, 
			hud_x, hud_y,
			hud_x + cos(angle) * outer_radius, hud_y + sin(angle) * outer_radius,
			thickness, colours.rotation_indicator);
	}
	
	void render_handles(scene@ g)
	{
		// TODO: Rotated handles
		float mid_x = (hud_min_x + hud_max_x) * 0.5;
		float mid_y = (hud_min_y + hud_max_y) * 0.5;
		
		render_handle(g, TopLeft, hud_min_x, hud_min_y);
		render_handle(g, Top, mid_x, hud_min_y);
		render_handle(g, TopRight, hud_max_x, hud_min_y);
		render_handle(g, Right, hud_max_x, mid_y);
		render_handle(g, BottomRight, hud_max_x, hud_max_y);
		render_handle(g, Bottom, mid_x, hud_max_y);
		render_handle(g, BottomLeft, hud_min_x, hud_max_y);
		render_handle(g, Left, hud_min_x, mid_y);
	}
	
	private void render_handle(scene@ g, ResizeMode handle, float x, float y)
	{
		calculate_handle_centre(handle, x, y, x, y);
		
		g.draw_rectangle_world(
			22, 24,
			x - handle_radius, y - handle_radius,
			x + handle_radius, y + handle_radius,
			0,
			handle == selected_handle || handle == hovered_handle ? colours.handle_selected : colours.handle);
	}
	
}

enum DragMode
{
	
	None,
	Rotation,
	Move,
	Resize,
	
}

enum ResizeMode
{
	
	None,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left,
	
}














