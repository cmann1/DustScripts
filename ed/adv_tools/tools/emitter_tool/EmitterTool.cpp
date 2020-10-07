#include 'EmitterToolSettings.cpp';
#include 'EmitterToolState.cpp';
#include 'EmitterData.cpp';
#include 'EmitterToolWindow.cpp';
#include '../../../../lib/emitters/names.cpp';
#include '../../../../lib/emitters/common.cpp';


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
	
	private bool selection_updated;
	
	private EmitterData@ hovered_emitter;
	private EmitterData@ pressed_emitter;
	private SelectAction pressed_action;
	private int hover_index_offset;
	
	private bool force_create;
	private int select_rect_pending;
	private int action_layer;
	private float drag_start_x, drag_start_y;
	private float drag_anchor_x, drag_anchor_y;
	private float drag_offset_angle;
	private float drag_base_rotation;
	private DragHandleType dragged_handle = DragHandleType::None;
	
	private WorldBoundingBox selection_bounding_box;
	
	private EmitterToolWindow properties_window;
	IntSetting@ emitter_id;
	IntSetting@ layer;
	IntSetting@ sublayer;
	FloatSetting@ rotation;
	
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
		@emitter_id	= script.get_int(this, 'emitter_id', EmitterId::DustGround);
		@layer		= script.get_int(this, 'layer', 19);
		@sublayer	= script.get_int(this, 'sublayer', 12);
		@rotation	= script.get_float(this, 'rotation', 0);
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	protected void on_select_impl()
	{
		properties_window.show(script, this);
	}
	
	protected void on_deselect_impl()
	{
		@primary_selected = null;
		@hovered_emitter = null;
		clear_highlighted_emitters();
		
		properties_window.hide();
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
		
		if(mouse.left_release)
		{
			pressed_action = SelectAction::None;
		}
		
		if(@primary_selected != null && primary_selected.is_mouse_inside == 0)
		{
			primary_selected.hovered = false;
		}
		
		for(int j = highlighted_emitters_count - 1; j >= 0; j--)
		{
			highlighted_emitters[j].visible = false;
		}
		
		int mouse_inside_count = 0;
		
		int i = script.query_onscreen_entities(ColType::Emitter, true);
		
		while(i-- > 0)
		{
			entity@ emitter = script.g.get_entity_collision_index(i);
			
			EmitterData@ data = highlight(emitter, i);
			data.update();
			data.visible = true;
		}
		
		if(!script.ui.is_mouse_over_ui)
		{
			for(i = highlighted_emitters_count - 1; i >= 0; i--)
			{
				if(highlighted_emitters[i].is_mouse_inside == 1)
				{
					mouse_inside_count++;
				}
			}
		}
		
		highlighted_emitters.sortAsc(0, highlighted_emitters_count);
		
		if(@hovered_emitter != null)
		{
			hovered_emitter.hovered = false;
			@hovered_emitter = null;
		}
		
		int hover_index = 0;
		
		if(script.mouse_in_scene && mouse_inside_count > 0)
		{
			for(i = highlighted_emitters_count - 1; i >= 0; i--)
			{
				EmitterData@ data = @highlighted_emitters[i];
				@highlighted_emitters[i] = @data;
				
				if(data.is_mouse_inside != 1)
					continue;
				
				if(state == Idle)
				{
					if(hover_index++ != (hover_index_offset % mouse_inside_count))
						continue;
					
					data.hovered = true;
					@hovered_emitter = data;
				}
				
				break;
			}
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
		
		if(selection_updated)
		{
			properties_window.update_selection(selected_emitters, selected_emitters_count);
			selection_updated = false;
		}
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		for(int i = 0; i < highlighted_emitters_count; i++)
		{
			EmitterData@ data = @highlighted_emitters[i];
			
			if(@data == @hovered_emitter)
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
		
		// New emitter
		
		if(state == EmitterToolState::Creating)
		{
			float sx, sy;
			script.transform(drag_start_x, drag_start_y, layer.value, 22, sx, sy);
			float lx, ly;
			rotate(mouse.x - sx, mouse.y - sy, -rotation.value * DEG2RAD, lx, ly);
			const float ox = (sx + mouse.x) * 0.5;
			const float oy = (sy + mouse.y) * 0.5;
			
			script.g.draw_rectangle_world(22, 22,
				ox - lx * 0.5, oy - ly * 0.5,
				ox + lx * 0.5, oy + ly * 0.5,
				rotation.value, Settings::HoveredFillColour);
			
			outline_rotated_rect(script.g, 22, 22,
				ox, oy, lx * 0.5, ly * 0.5,
				rotation.value, Settings::DefaultLineWidth / script.zoom,
				Settings::HoveredLineColour);
			
			const float nx = cos((rotation.value - 90) * DEG2RAD);
			const float ny = sin((rotation.value - 90) * DEG2RAD);
			const float arrow_length = max(min(30 / script.zoom, abs(ly) * 0.5), 0.0);
			const float arrow_size = max(min(10 / script.zoom, min(abs(lx) * 0.5, abs(ly) * 0.5)), 0.0);
			const float m = ly < 0 ? -1 : 1;
			
			draw_arrow(script.g, 22, 22,
				ox, oy,
				ox + nx * m * (arrow_length * m), oy + ny * m * (arrow_length * m),
				Settings::DefaultLineWidth / script.zoom, arrow_size, 1,
				Settings::HoveredLineColour);
			
			const float size = 4 / script.zoom;
			script.g.draw_rectangle_world(22, 22,
				ox - size, oy - size, ox + size, oy + size, 45, Settings::HoveredLineColour);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		// Start Creating
		
		if(mouse.left_press)
		{
			force_create = script.mouse_in_scene && script.editor.key_check_vk(VK::A);
		}
		
		if(
			script.mouse_in_scene && script.pressed_in_scene &&
			!mouse.left_press && mouse.left_down && !script.space_on_press && !script.alt &&
			(@pressed_emitter == null || force_create) && mouse.moved)
		{
			idle_start_create();
			return;
		}
		
		// Start moving
		
		if(@pressed_emitter != null && mouse.moved && (pressed_action == SelectAction::None || pressed_action == SelectAction::Set))
		{
			idle_start_drag();
			return;
		}
		
		// Deselect
		
		if(script.mouse_in_scene && mouse.left_release && !mouse_moved_after_press && @pressed_emitter == null && !script.shift && !script.ctrl && !script.alt)
		{
			select_none();
		}
		
		// Move with arrow keys
		
		if(script.key_repeat_gvb(GVB::LeftArrow))
		{
			shift_emitters(script.ctrl ? -20 : script.shift ? -10 : -1, 0);
		}
		else if(script.key_repeat_gvb(GVB::RightArrow))
		{
			shift_emitters(script.ctrl ? 20 : script.shift ? 10 : 1, 0);
		}
		else if(script.key_repeat_gvb(GVB::UpArrow))
		{
			shift_emitters(0, script.ctrl ? -20 : script.shift ? -10 : -1);
		}
		else if(script.key_repeat_gvb(GVB::DownArrow))
		{
			shift_emitters(0, script.ctrl ? 20 : script.shift ? 10 : 1);
		}
		
		// Adjust layer/sublayer
		
		if(mouse.scroll != 0 && (script.ctrl || script.alt))
		{
			idle_adjust_layer();
		}
		
		// Deselect on right mouse in empty space
		
		if(script.mouse_in_scene && mouse.right_press && !script.shift && @hovered_emitter == null)
		{
			select_none();
		}
		
		// Delete
		
		if(@script.ui.focus == null && script.editor.key_check_gvb(GVB::Delete))
		{
			for(int i = 0; i < selected_emitters_count; i++)
			{
				script.g.remove_entity(selected_emitters[i].emitter);
			}
			
			select_none();
			
			if(@hovered_emitter != null)
			{
				hovered_emitter.hovered = false;
				@hovered_emitter = null;
			}
		}
		
		if(@hovered_emitter != null && (mouse.right_press || script.shift && mouse.right_down))
		{
			if(hovered_emitter.selected)
			{
				select_emitter(hovered_emitter, SelectAction::Remove);
			}
			
			script.g.remove_entity(hovered_emitter.emitter);
			hovered_emitter.hovered = false;
			hovered_emitter.is_mouse_inside = 0;
			@hovered_emitter = null;
		}
		
		if(@primary_selected != null)
		{
			dragged_handle = primary_selected.do_handles();
			
			if(dragged_handle >= DragHandleType::Right && dragged_handle <= DragHandleType::TopRight)
			{
				idle_start_scaling();
				return;
			}
			else if(dragged_handle == DragHandleType::Rotate)
			{
				idle_start_rotating();
				return;
			}
		}
		
		// Select emitter on click
		
		if(script.mouse_in_scene && mouse.left_press && @hovered_emitter != null)
		{
			const SelectAction action = script.shift || (hovered_emitter.selected && !script.ctrl)
				? SelectAction::Add
				: script.ctrl ? SelectAction::Remove : SelectAction::Set;
			
			pressed_action = hovered_emitter.selected && action != SelectAction::Remove ? SelectAction::None : action;
			
			select_emitter(hovered_emitter, action);
			@pressed_emitter = @hovered_emitter;
		}
		
		// Selection rect
		
		if(script.mouse_in_scene && mouse.left_press && script.alt)
		{
			drag_start_x = mouse.x;
			drag_start_y = mouse.y;
			select_rect_pending = script.shift ? 1 : script.ctrl ? -1 : 0;
			
			if(select_rect_pending == 0)
			{
				select_none();
			}
			
			state = Selecting;
			script.ui.mouse_enabled = false;
			return;
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
		
		do_handles();
		@pressed_emitter = null;
		state = Moving;
		script.ui.mouse_enabled = false;
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
	
	private void idle_start_scaling()
	{
		primary_selected.get_handle_position(DragHandle::opposite(dragged_handle), drag_anchor_x, drag_anchor_y);
		
		float local_x, local_y;
		script.world_to_local(
			mouse.x, mouse.y, 22,
			drag_anchor_x, drag_anchor_y, primary_selected.rotation,
			primary_selected.layer,
			drag_start_x, drag_start_y);
		
		drag_start_x = drag_start_x > 0 ? drag_start_x - primary_selected.width : drag_start_x + primary_selected.width;
		drag_start_y = drag_start_y > 0 ? drag_start_y - primary_selected.height : drag_start_y + primary_selected.height;
		
		primary_selected.start_scale();
		primary_selected.hovered = true;
		
		state = Scaling;
		script.ui.mouse_enabled = false;
	}
	
	private void idle_start_rotating()
	{
		EmitterData@ data = @primary_selected;
		
		float mx, my;
		script.transform(mouse.x, mouse.y, 22, data.layer, mx, my);
		drag_offset_angle = atan2(my - data.y, mx - data.x);
		drag_base_rotation = data.rotation * DEG2RAD;
		
		primary_selected.start_rotate();
		
		state = Rotating;
		script.ui.mouse_enabled = false;
	}
	
	private void idle_start_create()
	{
		select_none();
		
		script.transform(mouse.x, mouse.y, 22, layer.value, drag_start_x, drag_start_y);
		
		state = EmitterToolState::Creating;
		script.ui.mouse_enabled = false;
	}
	
	private void state_moving()
	{
		do_handles();
		
		if(script.escape_press || !mouse.left_down)
		{
			for(int i = 0; i < selected_emitters_count; i++)
			{
				selected_emitters[i].stop_drag(!script.escape_press);
			}
			
			state = Idle;
			script.ui.mouse_enabled = true;
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
	
	private void state_scaling()
	{
		if(script.escape_press || !mouse.left_down)
		{
			primary_selected.do_handles(dragged_handle);
			primary_selected.stop_scale(script.escape_press);
			
			if(primary_selected.is_mouse_inside == 0 && !primary_selected.mouse_over_handle)
			{
				primary_selected.hovered = false;
			}
			
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		EmitterData@ data = @primary_selected;
		
		float local_x, local_y;
		script.world_to_local(
			mouse.x, mouse.y, 22,
			drag_anchor_x, drag_anchor_y, data.rotation,
			data.layer,
			local_x, local_y);
		
		local_x -= drag_start_x;
		local_y -= drag_start_y;
		
		float width, height;
		float ix = 0, iy = 0;
		
		switch(dragged_handle)
		{
			case DragHandleType::BottomRight:
			case DragHandleType::TopRight:
			case DragHandleType::Right:
			case DragHandleType::BottomLeft:
			case DragHandleType::TopLeft:
			case DragHandleType::Left:
				width = abs(local_x);
				ix = 1;
				break;
			default:
				width = data.width;
				local_x = 0;
				break;
		}
		
		switch(dragged_handle)
		{
			case DragHandleType::Bottom:
			case DragHandleType::BottomLeft:
			case DragHandleType::BottomRight:
			case DragHandleType::Top:
			case DragHandleType::TopLeft:
			case DragHandleType::TopRight:
				height = abs(local_y);
				iy = 1;
				break;
			default:
				height = data.height;
				local_y = 0;
				break;
		}
		
		rotate(local_x, local_y, data.rotation * DEG2RAD, local_x, local_y);
		
		data.do_scale(width, height, drag_anchor_x + local_x * 0.5, drag_anchor_y + local_y * 0.5);
		data.do_handles(dragged_handle);
		data.hovered = true;
	}
	
	private void state_rotating()
	{
		if(script.escape_press || !mouse.left_down)
		{
			primary_selected.do_handles(dragged_handle);
			primary_selected.stop_rotate(script.escape_press);
			
			if(primary_selected.is_mouse_inside == 0 && !primary_selected.mouse_over_handle)
			{
				primary_selected.hovered = false;
			}
			
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		EmitterData@ data = @primary_selected;
		
		float mx, my;
		script.transform(mouse.x, mouse.y, 22, data.layer, mx, my);
		float angle = drag_base_rotation + atan2(my - data.y, mx - data.x) - drag_offset_angle;
		script.snap(angle, angle);
		
		data.do_rotate((angle) * RAD2DEG);
		data.do_handles(dragged_handle);
		data.hovered = true;
		
		properties_window.update_rotation(data.rotation);
	}
	
	private void state_selecting()
	{
		clear_pending_emitters();
		do_handles();
		
		if(script.escape_press)
		{
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		float y1 = min(drag_start_y, mouse.y);
		float y2 = max(drag_start_y, mouse.y);
		float x1 = min(drag_start_x, mouse.x);
		float x2 = max(drag_start_x, mouse.x);
		
		int i = script.g.get_entity_collision(y1, y2, x1, x2, ColType::Emitter);
		
		while(i-- > 0)
		{
			entity@ e = script.g.get_entity_collision_index(i);
			EmitterData@ data = highlight(e, i);
			
			if(select_rect_pending == 0)
			{
				if(mouse.left_down)
				{
					data.pending_selection = 1;
				}
				else
				{
					select_emitter(data, SelectAction::Add, false);
				}
			}
			else if(select_rect_pending == 1)
			{
				if(!data.selected)
				{
					if(mouse.left_down)
					{
						data.pending_selection = 1;
					}
					else
					{
						select_emitter(data, SelectAction::Add, false);
					}
				}
			}
			else if(select_rect_pending == -1 && data.selected)
			{
				if(mouse.left_down)
				{
					data.pending_selection = -1;
				}
				else
				{
					select_emitter(data, SelectAction::Remove, false);
				}
			}
		}
		
		// Complete selection
		
		if(!mouse.left_down)
		{
			state = Idle;
			script.ui.mouse_enabled = true;
		}
	}
	
	private void state_creating()
	{
		if(script.escape_press)
		{
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		if(mouse.left_down)
			return;
		
		float mx, my;
		script.transform(mouse.x, mouse.y, 22, layer.value, mx, my);
		float lx, ly;
		rotate(mx - drag_start_x, my - drag_start_y, -rotation.value * DEG2RAD, lx, ly);
		
		const float ox = (drag_start_x + mx) * 0.5;
		const float oy = (drag_start_y + my) * 0.5;
		
		entity@ emitter = create_emitter(emitter_id.value,
			ox, oy,
			ceil_int(abs(lx)), ceil_int(abs(ly)), layer.value, sublayer.value,
			round_int(rotation.value));
		script.g.add_entity(emitter);
		
		EmitterData@ data = highlight(emitter, 0);
		select_emitter(data, SelectAction::Set, true);
		data.update();
		data.visible = true;
		
		state = Idle;
		script.ui.mouse_enabled = true;
	}
	
	//
	
	private void shift_emitters(const float dx, const float dy)
	{
		for(int i = 0; i < selected_emitters_count; i++)
		{
			selected_emitters[i].move(dx, dy);
		}
	}
	
	private void do_handles(const DragHandleType current_handle=DragHandleType::None)
	{
		if(@primary_selected == null)
			return;
		
		primary_selected.do_handles(current_handle);
	}
	
	// //////////////////////////////////////////////////////////
	// Selection
	// //////////////////////////////////////////////////////////
	
	private void select_emitter(EmitterData@ data, const SelectAction action, const bool update_primary=true)
	{
		if(action == SelectAction::Set)
		{
			while(selected_emitters_count > 0)
			{
				EmitterData@ emitter_data = @selected_emitters[--selected_emitters_count];
				emitter_data.selected = false;
				emitter_data.hovered = false;
				selection_updated = true;
			}
			
			if(update_primary && @primary_selected != null)
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
			if(update_primary || action == SelectAction::Set && @primary_selected != @data)
			{
				if(@primary_selected != null)
				{
					primary_selected.primary_selected = false;
				}
				
				data.primary_selected = true;
				@primary_selected = data;
			}
			
			if(data.selected)
				return;
		}
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(selected_emitters_count >= selected_emitters_size)
			{
				selected_emitters.resize(selected_emitters_size = selected_emitters_count + 32);
			}
			
			@selected_emitters[selected_emitters_count++] = data;
			data.selected = true;
			
			selection_updated = true;
		}
		else
		{
			selected_emitters.removeAt(selected_emitters.findByRef(@data));
			selected_emitters.resize(selected_emitters_size);
			data.selected = false;
			selected_emitters_count--;
			
			if(data.primary_selected)
			{
				data.primary_selected = false;
				@primary_selected = null;
			}
			
			selection_updated = true;
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
				continue;
			
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