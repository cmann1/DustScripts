#include 'EmitterToolSettings.cpp';
#include 'EmitterToolState.cpp';
#include 'EmitterData.cpp';

class EmitterTool : Tool
{
	
	private EmitterToolState state = Idle;
	private Mouse@ mouse;
	private bool mouse_moved_after_press;
	
	private int emitter_data_pool_size;
	private int emitter_data_pool_count;
	private array<EmitterData@> emitter_data_pool(emitter_data_pool_size);
	
	private int highlighted_emitters_size = 32;
	private int highlighted_emitters_count;
	private array<EmitterData@> highlighted_emitters(highlighted_emitters_size);
	private dictionary highlighted_emitters_map;
	
	private int selected_emitters_size = 32;
	private int selected_emitters_count;
	private array<EmitterData@> selected_emitters(selected_emitters_size);
	private EmitterData@ primary_selected;
	
	private EmitterData@ hovered_emitter;
	private EmitterData@ pressed_emitter;
	private int hover_index_offset;
	
	private int action_layer;
	private float drag_start_x, drag_start_y;
	
	private WorldBoundingBox selection_bounding_box;
	
	EmitterTool()
	{
		super('Emitter Tool');
	}
	
	void create(AdvToolScript@ script, ToolGroup@ group) override
	{
		Tool::create(script, group);
		
		set_icon('editor', 'boundary');
		
		@mouse = @script.mouse;
	}
	
	void on_init() override
	{
		
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	protected void on_select_impl()
	{
		
	}
	
	protected void on_deselect_impl()
	{
		@hovered_emitter = null;
		clear_highlighted_emitters();
	}
	
	protected void step_impl() override
	{
		for(int j = 0; j < selected_emitters_count; j++)
		{
			selected_emitters[j].pre_step_validate();
		}
		
		if(mouse.moved)
		{
			hover_index_offset = 0;
		}
		
		for(int j = highlighted_emitters_count - 1; j >= 0; j--)
		{
			highlighted_emitters[j].visible = false;
		}
		
		int i = script.query_onscreen_entities(ColType::Emitter, true);
		int mouse_inside_count = 0;
		
		while(i-- > 0)
		{
			entity@ emitter = script.g.get_entity_collision_index(i);
			
			EmitterData@ data = highlight(emitter, i);
			data.update();
			data.visible = true;
		}
		
		for(i = highlighted_emitters_count - 1; i >= 0; i--)
		{
			if(highlighted_emitters[i].is_mouse_inside == 1)
			{
				mouse_inside_count++;
			}
		}
		
		highlighted_emitters.sortAsc(0, highlighted_emitters_count);
		
		if(@hovered_emitter != null)
		{
			hovered_emitter.hovered = false;
			@hovered_emitter = null;
		}
		
		int hover_index = 0;
		
		for(i = highlighted_emitters_count - 1; i >= 0; i--)
		{
			EmitterData@ data = @highlighted_emitters[i];
			@highlighted_emitters[i] = @data;
			
			if(data.is_mouse_inside != 1)
				continue;
			
			if(hover_index++ != (hover_index_offset % mouse_inside_count))
				continue;
			
			data.hovered = true;
			@hovered_emitter = data;
			break;
		}
		
		switch(state)
		{
			case EmitterToolState::Idle: state_idle(); break;
			case EmitterToolState::Moving: state_moving(); break;
			case EmitterToolState::Rotating: state_rotating(); break;
			case EmitterToolState::Scaling: state_scaling(); break;
			case EmitterToolState::Selecting: state_selecting(); break;
			case EmitterToolState::Creating: state_creating(); break;
		}
		
		if(!mouse.left_down)
		{
			@pressed_emitter = null;
		}
		
		if(mouse.left_press)
		{
			mouse_moved_after_press = false;
		}
		else if(mouse.moved)
		{
			mouse_moved_after_press = true;
		}
		
		update_highlighted_emitters();
		
		for(int j = 0; j < selected_emitters_count; j++)
		{
			selected_emitters[j].post_step_validate();
		}
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		for(int i = 0; i < highlighted_emitters_count; i++)
		{
			EmitterData@ data = @highlighted_emitters[i];
			
			if(data.hovered)
				continue;
			
			data.draw();
		}
		
		if(@hovered_emitter != null)
		{
			hovered_emitter.draw();
		}
		
		// Selection rect
		
		if(state == Selecting)
		{
			script.draw_select_rect(drag_start_x, drag_start_y, mouse.x, mouse.y);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		// Start moving
		
		if(@pressed_emitter != null && mouse.moved)
		{
			idle_start_drag();
			return;
		}
		
		// Deselect
		
		if(mouse.left_release && !mouse_moved_after_press && @pressed_emitter == null && !script.shift && !script.ctrl && !script.alt)
		{
			select_none();
		}
		
		// Selection rect
		
		if(mouse.left_press && script.alt)
		{
			drag_start_x = mouse.x;
			drag_start_y = mouse.y;
			state = Selecting;
			return;
		}
		
		// Select emitter on click
		
		if(mouse.left_press && @hovered_emitter != null)
		{
			select_emitter(hovered_emitter, script.shift || (hovered_emitter.selected && !script.ctrl)
				? SelectAction::Add
				: script.ctrl ? SelectAction::Remove : SelectAction::Set);
			@pressed_emitter = @hovered_emitter;
		}
		
		// Adjust layer/sublayer
		
		if(mouse.scroll != 0 && (script.ctrl || script.alt))
		{
			idle_adjust_layer();
		}
		
		// Scroll hover index
		
		if(mouse.scroll != 0 && !script.space && !script.ctrl && !script.alt && !script.shift)
		{
			hover_index_offset -= mouse.scroll;
		}
	}
	
	private void idle_start_drag()
	{
		action_layer = pressed_emitter.layer;
		
		drag_start_x = mouse.x;
		drag_start_y = mouse.y;
		script.transform(drag_start_x, drag_start_y, 22, action_layer, drag_start_x, drag_start_y);
		
		for(int i = 0; i < selected_emitters_count; i++)
		{
			selected_emitters[i].start_drag();
		}
		
		state = Moving;
		@pressed_emitter = null;
	}
	
	private void idle_adjust_layer()
	{
		EmitterData@ data = null;
		IWorldBoundingBox@ bounding_box = null;
		
		if(script.shift)
		{
			selection_bounding_box.reset();
			
			for(int i = 0; i < selected_emitters_count; i++)
			{
				@data = @selected_emitters[i];
				data.shift_layer(mouse.scroll, script.alt);
				selection_bounding_box.add(
					data.aabb_x + data.aabb_x1, data.aabb_y + data.aabb_y1,
					data.aabb_x + data.aabb_x2, data.aabb_y + data.aabb_y2);
			}
		}
		else if(@hovered_emitter != null)
		{
			@data = hovered_emitter;
			data.shift_layer(mouse.scroll, script.alt);
			@bounding_box = data;
		}
		
		if(@bounding_box != null)
		{
			script.show_layer_sublayer_overlay(bounding_box, data.layer, data.sublayer);
		}
		else if(@data != null)
		{
			script.show_layer_sublayer_overlay(@selection_bounding_box, data.layer, data.sublayer);
		}
	}
	
	private void state_moving()
	{
		if(script.escape_press || !mouse.left_down)
		{
			for(int i = 0; i < selected_emitters_count; i++)
			{
				selected_emitters[i].stop_drag(!script.escape_press);
			}
			
			state = Idle;
			return;
		}
		
		float start_x, start_y;
		float mouse_x, mouse_y;
		script.transform(mouse.x, mouse.y, 22, action_layer, mouse_x, mouse_y);
		script.snap(drag_start_x, drag_start_y, start_x, start_y);
		script.snap(mouse_x, mouse_y, mouse_x, mouse_y);
		const float drag_delta_x = mouse_x - start_x;
		const float drag_delta_y = mouse_y - start_y;
		
		for(int i = 0; i < selected_emitters_count; i++)
		{
			selected_emitters[i].do_drag(drag_delta_x, drag_delta_y);
		}
	}
	
	private void state_rotating()
	{
		
	}
	
	private void state_scaling()
	{
		
	}
	
	private void state_selecting()
	{
		clear_pending_emitters();
		
		if(script.escape_press || !mouse.left_down)
		{
			
			state = Idle;
			return;
		}
		
		const float y1 = min(drag_start_y, mouse.y);
		const float y2 = max(drag_start_y, mouse.y);
		const float x1 = min(drag_start_x, mouse.x);
		const float x2 = max(drag_start_x, mouse.x);
	}
	
	private void state_creating()
	{
		
	}
	
	// //////////////////////////////////////////////////////////
	// Selection
	// //////////////////////////////////////////////////////////
	
	private void select_emitter(EmitterData@ data, const SelectAction action)
	{
		if(action == SelectAction::Set)
		{
			while(selected_emitters_count > 0)
			{
				EmitterData@ emitter_data = @selected_emitters[--selected_emitters_count];
				emitter_data.selected = false;
			}
			
			if(@primary_selected != null)
			{
				primary_selected.primary_selected = false;
				@primary_selected = null;
			}
		}
		
		if(@data == null)
			return;
		
		if(action == SelectAction::Remove && !data.selected)
			return;
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(@primary_selected != null)
			{
				primary_selected.primary_selected = false;
			}
			
			data.primary_selected = true;
			@primary_selected = data;
			
			if(data.selected)
				return;
		}
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(selected_emitters_count >= selected_emitters_size)
			{
				selected_emitters.resize(selected_emitters_size += 32);
			}
			
			@selected_emitters[selected_emitters_count++] = data;
			data.selected = true;
		}
		else
		{
			selected_emitters.removeAt(selected_emitters.findByRef(@data));
			data.selected = false;
			selected_emitters_count--;
			
			if(data.primary_selected)
			{
				data.primary_selected = false;
				@primary_selected = null;
			}
		}
	}
	
	private void select_none()
	{
		select_emitter(null, SelectAction::Set);
	}
	
	private EmitterData@ highlight(entity@ emitter, const int index)
	{
		const string key = emitter.id() + '';
		EmitterData@ emitter_data;
		
		if(highlighted_emitters_map.exists(key))
		{
			return cast<EmitterData@>(highlighted_emitters_map[key]);
		}
		
		@emitter_data = emitter_data_pool_count > 0
			? @emitter_data_pool[--emitter_data_pool_count]
			: EmitterData();
		
		@highlighted_emitters_map[key] = @emitter_data;
		@highlighted_emitters[highlighted_emitters_count++] = @emitter_data;
		
		emitter_data.init(script, this, emitter, index);
		
		return emitter_data;
	}
	
	private void update_highlighted_emitters()
	{
		for(int i = highlighted_emitters_count - 1; i >= 0; i--)
		{
			EmitterData@ emitter_data = @highlighted_emitters[i];
			
			if(emitter_data.hovered || emitter_data.selected || emitter_data.visible)
			{
				emitter_data.step();
				continue;
			}
			
			if(emitter_data_pool_count >= emitter_data_pool_size)
			{
				emitter_data_pool.resize(emitter_data_pool_size += 32);
			}
			
			@emitter_data_pool[emitter_data_pool_count++] = @emitter_data;
			@highlighted_emitters[i] = @highlighted_emitters[--highlighted_emitters_count];
			highlighted_emitters_map.delete(emitter_data.key);
		}
	}
	
	private void clear_highlighted_emitters()
	{
		for(int i = highlighted_emitters_count - 1; i >= 0; i--)
		{
			EmitterData@ emitter_data = @highlighted_emitters[i];
			
			if(emitter_data_pool_count >= emitter_data_pool_size)
			{
				emitter_data_pool.resize(emitter_data_pool_size += 32);
			}
			
			@emitter_data_pool[emitter_data_pool_count++] = @emitter_data;
			@highlighted_emitters[i] = @highlighted_emitters[--highlighted_emitters_count];
			highlighted_emitters_map.delete(emitter_data.key);
		}
		
		highlighted_emitters_count = 0;
	}
	
	private void clear_pending_emitters()
	{
		for(int i = highlighted_emitters_count - 1; i >= 0; i--)
		{
			highlighted_emitters[i].pending_selection = 0;
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
}