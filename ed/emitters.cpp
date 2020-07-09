#include "../lib/std.cpp";
#include "../lib/math/math.cpp";
#include "../lib/enums/ColType.cpp";
#include "../lib/print_vars.cpp";
#include "../lib/drawing/common.cpp";

class script
{
	
	[text] bool enabled = true;
	
	private scene@ g;
	private bool prev_left_mouse_down;
	private bool prev_right_mouse_down;
	private bool prev_middle_mouse_down;
	
	private float handle_radius = 5;
	private uint normal_fill = 0x33ffffff;
	private uint normal_outline = 0x44FFFFFF;
	private uint selected_fill = 0x4400FF00;
	private uint selected_outline = 0x6600ff00;
	private uint handle_colour = 0xeeee4444;
	private uint handle_selected_colour = 0xeeeeee44;
	
	private DragMode dragMode = None;
	private entity@ active_emitter;
	private float drag_angle_offset;
	private float handle_offset_x;
	private float handle_offset_y;
	private float min_x;
	private float min_y;
	private float max_x;
	private float max_y;
	
	textfield@ layer_text;
	
	bool left_mouse_down;
	bool right_mouse_down;
	bool middle_mouse_down;
	
	bool left_mouse_press;
	bool right_mouse_press;
	bool middle_mouse_press;
	
	float mouse_x;
	float mouse_y;
	int mouse_state;
	
	script()
	{
		@g = get_scene();
		
		@layer_text = create_textfield();
        layer_text.align_horizontal(0);
        layer_text.align_vertical(-1);
	}
	
	void editor_step()
	{
		if(!enabled)
			return;
		
		if(@revalidate_emitter != null)
		{
			g.remove_entity(revalidate_emitter);
			g.add_entity(revalidate_emitter);
			@active_emitter = revalidate_emitter;
			@revalidate_emitter = null;
		}
		
		mouse_x = g.mouse_x_world(0, 19);
		mouse_y = g.mouse_y_world(0, 19);
		mouse_state = g.mouse_state(0);
		int mouse_scroll = (mouse_state & 1 != 0) ? -1 : ((mouse_state & 2 != 0) ? 1 : 0);
		
		left_mouse_down = (mouse_state & 4) != 0;
		right_mouse_down = (mouse_state & 8) != 0;
		middle_mouse_down = (mouse_state & 16) != 0;
		
		left_mouse_press = left_mouse_down && !prev_left_mouse_down;
		right_mouse_press = right_mouse_down && !prev_right_mouse_down;
		middle_mouse_press = middle_mouse_down && !prev_middle_mouse_down;
		
		prev_left_mouse_down = left_mouse_down;
		prev_right_mouse_down = right_mouse_down;
		prev_middle_mouse_down = middle_mouse_down;
		
		uint emitter_under_mouse = 0;
		const float radius = 0.5;
		
		if(@active_emitter == null)
		{
			for(int layer = 0; layer <= 22; layer++)
			{
				float layer_mouse_x = g.mouse_x_world(0, layer);
				float layer_mouse_y = g.mouse_y_world(0, layer);
				
				int count = g.get_entity_collision(
					layer_mouse_y - radius, layer_mouse_y + radius,
					layer_mouse_x - radius, layer_mouse_x + radius,
					ColType::Emitter);
				
				for(int i = 0; i < count; i++)
				{
					entity@ emitter = g.get_entity_collision_index(i);
					
					if(emitter is null)
						continue;
					
					if(emitter.layer() != layer)
						continue;
					
					bool requires_update = false;
					
					varstruct@ vars = emitter.vars();
					varvalue@ width_var = vars.get_var('width');
					varvalue@ height_var = vars.get_var('height');
					varvalue@ has_rotation_var = vars.get_var('r_rotation');
					varvalue@ rotation_var = vars.get_var('e_rotation');
					varvalue@ sub_layer_var = vars.get_var('draw_depth_sub');
					float x = emitter.x();
					float y = emitter.y();
					float width = width_var.get_int32();
					float height = height_var.get_int32();
					float rotation = has_rotation_var.get_bool() ? rotation_var.get_int32() : 0;
					int sub_layer = sub_layer_var.get_int32();
					
					min_x = x - width * 0.5;
					min_y = y - height* 0.5;
					max_x = x + width * 0.5;
					max_y = y + height* 0.5;
					
					// Render rect
					// -----------------------
					
					render_emitter(emitter, selected_fill, selected_outline);
					render_emitter_handles(emitter, right_mouse_press);
					
					if(dragMode == None)
					{
						if(right_mouse_press)
						{
							handle_offset_x = layer_mouse_x - x;
							handle_offset_y = layer_mouse_y - y;
							dragMode = Move;
						}
						else if(middle_mouse_press)
						{
							float dx = layer_mouse_x - x;
							float dy = layer_mouse_y - y;
							drag_angle_offset = shortest_angle(rotation * DEG2RAD, atan2(dy, dx));
							dragMode = Rotation;
						}
					}
					
					if(dragMode != None)
					{
						@active_emitter = emitter;
					}
					
					emitter_under_mouse = emitter.id();
					
					// Add and remove again or changes are only reflected in game after entering and exiting playmode.
					// -----------------------
					
					if(requires_update)
					{
						@revalidate_emitter = @emitter;
					}
					
					layer = 23;
					break;
				}
			}
		}
		
		// Outline emitters close to mouse
		// -----------------------------------------------------------
		
		const float closest_radius = 400;
		
		for(int layer = 0; layer <= 22; layer++)
		{
			float layer_mouse_x = g.mouse_x_world(0, layer);
			float layer_mouse_y = g.mouse_y_world(0, layer);
			
			int count = g.get_entity_collision(
				layer_mouse_y - closest_radius, layer_mouse_y + closest_radius,
				layer_mouse_x - closest_radius, layer_mouse_x + closest_radius,
				ColType::Emitter);
			
			for(int i = 0; i < count; i++)
			{
				entity@ emitter = g.get_entity_collision_index(i);
				
				if(emitter is null)
					continue;
				
				if(emitter.layer() != layer || emitter.id() == emitter_under_mouse || emitter.is_same(active_emitter))
					continue;
				
				render_emitter(emitter, normal_fill, normal_outline);
			}
		}
		
		// Interact with active emitter
		// -----------------------------------------------------------
		
		if(@active_emitter != null)
		{
			if(emitter_under_mouse != active_emitter.id())
			{
				render_emitter(active_emitter, selected_fill, selected_outline);
				render_emitter_handles(active_emitter, false);
			}
			
			bool requires_update = false;
			bool update_size = false;
			
			varstruct@ vars = active_emitter.vars();
			varvalue@ sub_layer_var = vars.get_var('draw_depth_sub');
			
			int layer = active_emitter.layer();
			int sub_layer = sub_layer_var.get_int32();
			
			float x = active_emitter.x();
			float y = active_emitter.y();
			float layer_mouse_x = g.mouse_x_world(0, layer);
			float layer_mouse_y = g.mouse_y_world(0, layer);
			
			float min_x = this.min_x;
			float min_y = this.min_y;
			float max_x = this.max_x;
			float max_y = this.max_y;
			
			if(dragMode == Move)
			{
				layer_text.text(layer + '.' + sub_layer);
				shadowed_text_world(layer_text,
					22, 24, mouse_x, mouse_y + 30,
					1, 1, 0,
					0xbb000000, 2, 2);
			}
			
			switch(dragMode)
			{
				case Rotation:
				{
					float dx = layer_mouse_x - x;
					float dy = layer_mouse_y - y;
					
					varvalue@ has_rotation_var = vars.get_var('r_rotation');
					varvalue@ rotation_var = vars.get_var('e_rotation');
					
					int new_rotation = round_int((atan2(dy, dx) - drag_angle_offset) * RAD2DEG) % 360;
					
					if(new_rotation < 0)
						new_rotation = 360 + new_rotation;
					
					bool has_rotation = has_rotation_var.get_bool();
					float rotation = rotation_var.get_int32();
					
					if((!has_rotation && new_rotation != 0) || (has_rotation && new_rotation == 0) || new_rotation != rotation)
					{
						has_rotation_var.set_bool(new_rotation != 0);
						rotation_var.set_int32(new_rotation);
						requires_update = true;
					}
				}
					break;
				case Move:
				{
					active_emitter.set_xy(layer_mouse_x - handle_offset_x, layer_mouse_y - handle_offset_y);
					
					// Adjust sub layer
					// -----------------------
					
					if(mouse_scroll != 0)
					{
						sub_layer = clamp(sub_layer - mouse_scroll, 0, 24);
						sub_layer_var.set_int32(sub_layer);
						requires_update = true;
					}
				}
					break;
				case TopLeft:
					min_x = layer_mouse_x - handle_offset_x - handle_radius;
					min_y = layer_mouse_y - handle_offset_y - handle_radius;
					update_size = true;
					break;
				case Top:
					min_y = layer_mouse_y - handle_offset_y - handle_radius;
					update_size = true;
					break;
				case TopRight:
					max_x = layer_mouse_x - handle_offset_x + handle_radius;
					min_y = layer_mouse_y - handle_offset_y - handle_radius;
					update_size = true;
					break;
				case BottomLeft:
					min_x = layer_mouse_x - handle_offset_x - handle_radius;
					max_y = layer_mouse_y - handle_offset_y + handle_radius;
					update_size = true;
					break;
				case Bottom:
					max_y = layer_mouse_y - handle_offset_y + handle_radius;
					update_size = true;
					break;
				case BottomRight:
					max_x = layer_mouse_x - handle_offset_x + handle_radius;
					max_y = layer_mouse_y - handle_offset_y + handle_radius;
					update_size = true;
					break;
				case Left:
					min_x = layer_mouse_x - handle_offset_x - handle_radius;
					update_size = true;
					break;
				case Right:
					max_x = layer_mouse_x - handle_offset_x + handle_radius;
					update_size = true;
					break;
			}
			
			if(update_size)
			{
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
				
				active_emitter.set_xy(ceil_int((min_x + max_x) * 0.5), round_int((min_y + max_y) * 0.5));
				varvalue@ width_var = vars.get_var('width');
				varvalue@ height_var = vars.get_var('height');
				width_var.set_int32(ceil_int(max_x - min_x));
				height_var.set_int32(ceil_int(max_y - min_y));
				
				requires_update = true;
			}
			
			if(requires_update)
			{
				@revalidate_emitter = @active_emitter;
			}
			
			switch(dragMode)
			{
				case Rotation:
					if(!middle_mouse_down)
					{
						dragMode = None;
					}
					break;
				case TopLeft:
				case Top:
				case TopRight:
				case BottomLeft:
				case Bottom:
				case BottomRight:
				case Left:
				case Right:
				case Move:
					if(!right_mouse_down)
					{
						dragMode = None;
					}
					break;
			}
			
			if(dragMode == None)
			{
				@active_emitter = null;
			}
		}
		
		// Remove the emitter and add it again the next frame to force the modified vars to reflect in the editor
		if(@revalidate_emitter != null)
		{
			g.remove_entity(revalidate_emitter);
		}
	}
	
	private entity@ revalidate_emitter;
	
	private void render_emitter(entity@ emitter, uint fill_colour, uint outline_colour)
	{
		varstruct@ vars = emitter.vars();
		varvalue@ has_rotation_var = vars.get_var('r_rotation');
		float x = emitter.x();
		float y = emitter.y();
		float width = vars.get_var('width').get_int32();
		float height = vars.get_var('height').get_int32();
		float rotation = has_rotation_var.get_bool() ? vars.get_var('e_rotation').get_int32() : 0;
		
		g.draw_rectangle_world(
				emitter.layer(), 23,
				x - width * 0.5, y - height* 0.5,
				x + width * 0.5, y + height* 0.5,
				rotation,
				fill_colour);
		
		if(rotation != 0 && emitter.layer() < 12)
		{
			outline_rect(g,
				x - width * 0.5, y - height* 0.5,
				x + width * 0.5, y + height* 0.5,
				emitter.layer(), 23, 1, outline_colour);
		}
		
		outline_rect(g,
			x - width * 0.5, y - height* 0.5,
			x + width * 0.5, y + height* 0.5,
			22, 23, 1, outline_colour);
	}
	
	private void render_emitter_handles(entity@ emitter, bool mouse_press)
	{
		varstruct@ vars = emitter.vars();
		
		float layer_mouse_x = g.mouse_x_world(0, emitter.layer());
		float layer_mouse_y = g.mouse_y_world(0, emitter.layer());
		
		float x = emitter.x();
		float y = emitter.y();
		float width = vars.get_var('width').get_int32();
		float height = vars.get_var('height').get_int32();
		
		float min_x = x - width * 0.5;
		float min_y = y - height* 0.5;
		float max_x = x + width * 0.5;
		float max_y = y + height* 0.5;
		
		// Top left
		render_size_handle(emitter, min_x + handle_radius, min_y + handle_radius, layer_mouse_x, layer_mouse_y, TopLeft, mouse_press);
		// Top
		render_size_handle(emitter, (min_x + max_x) * 0.5, min_y + handle_radius, layer_mouse_x, layer_mouse_y, Top, mouse_press);
		// Top right
		render_size_handle(emitter, max_x - handle_radius, min_y + handle_radius, layer_mouse_x, layer_mouse_y, TopRight, mouse_press);
		
		// Bottom left
		render_size_handle(emitter, min_x + handle_radius, max_y - handle_radius, layer_mouse_x, layer_mouse_y, BottomLeft, mouse_press);
		// Bottom
		render_size_handle(emitter, (min_x + max_x) * 0.5, max_y - handle_radius, layer_mouse_x, layer_mouse_y, Bottom, mouse_press);
		// Bottom right
		render_size_handle(emitter, max_x - handle_radius, max_y - handle_radius, layer_mouse_x, layer_mouse_y, BottomRight, mouse_press);
		
		// Left
		render_size_handle(emitter, min_x + handle_radius, (min_y + max_y) * 0.5, layer_mouse_x, layer_mouse_y, Left, mouse_press);
		// Right
		render_size_handle(emitter, max_x - handle_radius, (min_y + max_y) * 0.5, layer_mouse_x, layer_mouse_y, Right, mouse_press);
	}

	private void render_size_handle(entity@ emitter, float x, float y, float mouse_x, float mouse_y, DragMode mode, bool is_mouse_down)
	{
		bool mouse_over = false;
		
		if(mouse_x >= x - handle_radius && mouse_x <= x + handle_radius && mouse_y >= y - handle_radius && mouse_y <= y + handle_radius)
		{
			if(is_mouse_down)
			{
				dragMode = mode;
				handle_offset_x = mouse_x - x;
				handle_offset_y = mouse_y - y;
			}
			
			mouse_over = true;
		}
		
		g.draw_rectangle_world(
				emitter.layer(), 24,
				x - handle_radius, y - handle_radius,
				x + handle_radius, y + handle_radius,
				0,
				mouse_over || dragMode == mode ? handle_selected_colour : handle_colour);
	}
	
}

enum DragMode
{
	
	None,
	Rotation,
	Move,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left,
	
}