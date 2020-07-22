#include "../../lib/std.cpp";
#include "../../lib/math/math.cpp";
#include "../../lib/math/geom.cpp";
#include "../../lib/enums/ColType.cpp";
#include "../../lib/drawing/common.cpp";
#include "../../lib/drawing/circle.cpp";
#include "../../lib/layer.cpp";
#include "Colours.cpp";
#include "EmitterData.cpp";
#include "DragMode.cpp";
#include "ResizeMode.cpp";
#include "RenderParallaxHitbox.cpp";

const float handle_radius = 5;
const Colours colours;

class script
{
	
	[text] bool enabled = true;
	[option,0:Never,1:Hover,2:Always] RenderParallaxHitbox parallax_hitbox = RenderParallaxHitbox::Never;
	[text] bool parallax_lines = false;
	
	private scene@ g;
	private textfield@ layer_text;
	private textfield@ active_layer_text;
	private Line@ line = Line();
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
	[hidden]
	private bool scroll_layer = false;
	
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
//					resize_x3 = hovered_emitter.min_x;
//					resize_y3 = hovered_emitter.min_y;
//					resize_x4 = hovered_emitter.min_x;
//					resize_y4 = hovered_emitter.min_y;
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
				
				if(active_emitter.is_mouse_over || active_emitter.hovered_handle != ResizeMode::None)
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
			
			float layer_closest_radius = closest_radius * get_layer_scale(g, 22, layer);
			
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
					active_emitter.update_view(view_x, view_y);
					active_emitter.update_mouse(layer_mouse_x, layer_mouse_y, mouse_x, mouse_y);
					continue;
				}
				
				EmitterData@ data = EmitterData(g);
				data.update_emitter(emitter);
				data.update_view(view_x, view_y);
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
		
		if(left_mouse_press)
		{
			g.remove_entity(active_emitter.emitter);
			@active_emitter = null;
			@hovered_emitter = null;
			dragMode = None;
			return;
		}
		
		if(middle_mouse_press)
		{
			scroll_layer = !scroll_layer;
		}
		
		if(mouse_scroll != 0)
		{
			if(scroll_layer)
			{
				active_emitter.update_layer(active_emitter.layer - mouse_scroll);
			}
			else
			{
				active_emitter.update_sub_layer(active_emitter.sub_layer - mouse_scroll);
			}
			
			@validate_emitter = @active_emitter;
		}
		
		active_emitter.update_position(active_emitter.mouse_x - handle_offset_x, active_emitter.mouse_y - handle_offset_y);
	}
	
	void update_size()
	{
		if(@active_emitter == null)
			return;
		
		const float view_x = cam.x();
		const float view_y = cam.y();
		
		float x1 = resize_min_x;
		float y1 = resize_min_y;
		float x2 = resize_max_x;
		float y2 = resize_min_y;
		float x3 = resize_max_x;
		float y3 = resize_max_y;
		float x4 = resize_min_x;
		float y4 = resize_max_y;
		const float mid_x = (x1 + x3) * 0.5;
		const float mid_y = (y1 + y3) * 0.5;
		
		const float angle = active_emitter.rotation * DEG2RAD;
		rotate(x1, y1, mid_x, mid_y, angle, x1, y1);
		rotate(x2, y2, mid_x, mid_y, angle, x2, y2);
		rotate(x3, y3, mid_x, mid_y, angle, x3, y3);
		rotate(x4, y4, mid_x, mid_y, angle, x4, y4);
		
		const float hor_dx = x2 - x1;
		const float hor_dy = y2 - y1;
		const float ver_dx = x4 - x1;
		const float ver_dy = y4 - y1;
		const float hor_length = magnitude(hor_dx, hor_dy);
		const float ver_length = magnitude(ver_dx, ver_dy);
		const float hor_nx = hor_dx / hor_length;
		const float hor_ny = hor_dy / hor_length;
		const float ver_nx = ver_dx / ver_length;
		const float ver_ny = ver_dy / ver_length;
		float x, y;
		float dx, dy;
		
		const float scale = get_layer_scale(g, 22, active_emitter.layer);
		const float offset_x = handle_offset_x * scale;
		const float offset_y = handle_offset_y * scale;
		
		/*
		 * Horizontal
		 */
		
		if(
			active_emitter.selected_handle == TopRight ||
			active_emitter.selected_handle == BottomRight ||
			active_emitter.selected_handle == Right
		)
		{
			dx = active_emitter.mouse_x - x1;
			dy = active_emitter.mouse_y - y1;
			project(dx, dy, hor_dx, hor_dy, x, y);
			const float size = magnitude(x, y) * sign(dot(x, y, hor_dx, hor_dy)) - offset_x;
			x2 = x1 + hor_nx * size;
			y2 = y1 + hor_ny * size;
			x3 = x4 + hor_nx * size;
			y3 = y4 + hor_ny * size;
		}
		else if(
			active_emitter.selected_handle == TopLeft ||
			active_emitter.selected_handle == BottomLeft ||
			active_emitter.selected_handle == Left
		)
		{
			dx = active_emitter.mouse_x - x2;
			dy = active_emitter.mouse_y - y2;
			project(dx, dy, hor_dx, hor_dy, x, y);
			const float size = magnitude(x, y) * sign(dot(x, y, -hor_dx, -hor_dy)) + offset_x;
			x1 = x2 - hor_nx * size;
			y1 = y2 - hor_ny * size;
			x4 = x3 - hor_nx * size;
			y4 = y3 - hor_ny * size;
		}
		
		/*
		 * Vertical
		 */
		
		if(
			active_emitter.selected_handle == BottomLeft ||
			active_emitter.selected_handle == BottomRight||
			active_emitter.selected_handle == Bottom
		)
		{
			dx = active_emitter.mouse_x - x1;
			dy = active_emitter.mouse_y - y1;
			project(dx, dy, ver_dx, ver_dy, x, y);
			const float size = magnitude(x, y) * sign(dot(x, y, ver_dx, ver_dy)) - offset_y;
			x4 = x1 + ver_nx * size;
			y4 = y1 + ver_ny * size;
			x3 = x2 + ver_nx * size;
			y3 = y2 + ver_ny * size;
		}
		else if(
			active_emitter.selected_handle == TopLeft ||
			active_emitter.selected_handle == TopRight ||
			active_emitter.selected_handle == Top
		)
		{
			dx = active_emitter.mouse_x - x4;
			dy = active_emitter.mouse_y - y4;
			project(dx, dy, ver_dx, ver_dy, x, y);
			const float size = magnitude(x, y) * sign(dot(x, y, -ver_dx, -ver_dy)) + offset_y;
			x1 = x4 - ver_nx * size;
			y1 = y4 - ver_ny * size;
			x2 = x3 - ver_nx * size;
			y2 = y3 - ver_ny * size;
		}
		
		const float mx = (x1 + x3) * 0.5;
		const float my = (y1 + y3) * 0.5;
		rotate(x1, y1, mx, my, -angle, x1, y1);
		rotate(x3, y3, mx, my, -angle, x3, y3);
		
		if(x1 > x3)
		{
			float temp = x1;
			x1 = x3;
			x3 = temp;
		}
		
		if(y1 > y3)
		{
			float temp = y1;
			y1 = y3;
			y3 = temp;
		}
		
		if(active_emitter.update_size(x1, y1, x3, y3))
		{
			@validate_emitter = @active_emitter;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(!enabled)
			return;
		
		float view_x = cam.x();
		float view_y = cam.y();
		
		for(int i = int(highlighted_emitters.length()) - 1; i >= 0; i--)
		{
			EmitterData@ data = @highlighted_emitters[i];
			
			if(@data == @hovered_emitter || @data == @active_emitter)
				continue;
			
			data.update_view(view_x, view_y);
			data.render_highlight(g, colours.normal_fill, colours.normal_outline, parallax_hitbox, parallax_lines);
		}
		
		EmitterData@ active_data = @active_emitter != null ? @active_emitter : @hovered_emitter;
		
		if(@active_data != null)
		{
			active_data.update_view(view_x, view_y);
			active_data.render_highlight(g, colours.selected_fill, colours.selected_outline, parallax_hitbox, parallax_lines);
			active_data.render_rotation(g);
			active_data.render_handles(g);
			
			if(dragMode == Move)
			{
				active_data.render_active_layer_text(active_layer_text, mouse_x, mouse_y, scroll_layer);
			}
			else
			{
				active_data.render_layer_text(layer_text, line);
			}
		}
	}
	
}