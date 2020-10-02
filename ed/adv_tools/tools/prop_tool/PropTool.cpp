#include '../../misc/SelectAction.cpp';
#include '../../settings/PropToolSettings.cpp';
#include '../../../../lib/tiles/common.cpp';
#include '../../../../lib/props/common.cpp';
#include '../../../../lib/props/data.cpp';
#include '../../../../lib/props/outlines.cpp';
#include '../../../../lib/ui3/elements/Toolbar.cpp';

#include 'PropToolState.cpp';
#include 'PropSortingData.cpp';
#include 'PropData.cpp';

const string EMBED_spr_icon_prop_tool = SPRITES_BASE + 'icon_prop_tool.png';

class PropTool : Tool
{
	
	private PropToolState state = Idle;
	private bool performing_action;
	
	private Mouse@ mouse;
	private bool mouse_press_moved;
	private bool mouse_press_modified;
	
	private int prop_data_pool_size;
	private int prop_data_pool_count;
	private array<PropData@> prop_data_pool(prop_data_pool_size);
	
	private int props_under_mouse_size = 32;
	private array<PropSortingData> props_under_mouse(props_under_mouse_size);
	
	private int highlighted_props_size = 32;
	private int highlighted_props_count;
	private array<PropData@> highlighted_props(highlighted_props_size);
	private dictionary highlighted_props_map;
	
	private PropData@ previous_hovered_prop;
	private PropData@ hovered_prop;
	private PropData@ pressed_prop;
	private int selected_props_size = 32;
	private int selected_props_count;
	private array<PropData@> selected_props(selected_props_size);
	private bool temporary_selection;
	
	private int hover_index_offset;
	
	private BoolSetting@ pick_through_tiles;
	private BoolSetting@ pick_ignore_holes;
	private FloatSetting@ pick_radius;
	private BoolSetting@ hide_selection_highlight;
	
	private float drag_start_x, drag_start_y;
	private float drag_offset_angle;
	private bool drag_rotation_handle;
	private int select_rect_pending;
	private int action_layer;
	
	private int selection_layer;
	private float selection_x, selection_y;
	private float selection_x1, selection_y1;
	private float selection_x2, selection_y2;
	private float selection_angle;
	private float selection_drag_start_x, selection_drag_start_y;
	private float selection_drag_start_angle;
	
	// UI
	
	private Toolbar@ toolbar;
	private PopupOptions@ info_popup;
	private Label@ info_label;
	
	PropTool()
	{
		super('Prop Tool');
		
		init_shortcut_key(VK::Q);
	}
	
	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_prop_tool');
	}
	
	void create(AdvToolScript@ script, ToolGroup@ group) override
	{
		Tool::create(script, group);
		
		set_icon('script', 'icon_prop_tool', 33, 33);
		
		@mouse = @script.mouse;
	}
	
	void on_init() override
	{
		@pick_through_tiles	= script.get_bool(this, 'pick_through_tiles', false);
		@pick_ignore_holes	= script.get_bool(this, 'pick_ignore_holes', true);
		@pick_radius		= script.get_float(this, 'pick_radius', 2);
		
		@hide_selection_highlight	= script.get_bool(this, 'hide_selection_highlight', true);
		// TODO: REMOVE
		hide_selection_highlight.value = false;
	}
	
	private void create_ui()
	{
		if(@toolbar != null)
			return;
		
		UI@ ui = script.ui;
		Style@ style = ui.style;
		
		@toolbar = Toolbar(ui, true, true);
		toolbar.name = 'PropToolToolbar';
		toolbar.x = 20;
		toolbar.y = 20;
		
		toolbar.add_button('PropTool');
		
		@info_label = Label(ui, '', true, font::SANS_BOLD, 20);
		info_label.scale_x = 0.75;
		info_label.scale_y = 0.75;
		
		@info_popup = PopupOptions(ui, info_label, false, PopupPosition::BelowLeft, PopupTriggerType::Manual, PopupHideType::Manual);
		info_popup.spacing = 0;
		info_popup.background_colour = multiply_alpha(style.normal_bg_clr, 0.5);
		
		script.window_manager.register_element(toolbar);
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	Tool@ on_shortcut_key() override
	{
		return selected ? script.get_tool('Props') : @this;
	}
	
	protected void on_select_impl()
	{
		script.hide_gui(true);
		
		create_ui();
		script.ui.add_child(toolbar);
	}
	
	void on_deselect_impl()
	{
		script.hide_gui(false);
		
		select_none();
		state = Idle;
		temporary_selection = false;
		
		script.ui.remove_child(toolbar);
	}
	
	protected void step_impl() override
	{
		// Reset hover index offset when the mouse moves
		
		if(mouse.delta_x != 0 || mouse.delta_y != 0)
		{
			hover_index_offset = 0;
			mouse_press_moved = true;
		}
		
		if(mouse.left_press)
		{
			mouse_press_moved = false;
		}
		
		switch(state)
		{
			case Idle: state_idle(); break;
			case Moving: state_moving(); break;
			case Rotating: state_rotating(); break;
			case Scaling: state_scaling(); break;
			case Selecting: state_selecting(); break;
		}
		
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			highlighted_props[i].step();
		}
		
		if(@hovered_prop != null)
		{
			if(@hovered_prop != @previous_hovered_prop)
			{
				show_prop_info(hovered_prop);
				@previous_hovered_prop = hovered_prop;
			}
		}
		else
		{
			script.ui.hide_tooltip(info_popup);
			@previous_hovered_prop = null;
		}
		
		performing_action = state != Idle && state != Selecting;
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		const bool highlight = !performing_action || !hide_selection_highlight.value;
		
		if(highlight || drag_rotation_handle)
		{
			// Highlights
			
			for(int i = 0; i < highlighted_props_count; i++)
			{
				highlighted_props[i].draw();
			}
			
			// Bounding box
			
			if(selected_props_count > 0 && !temporary_selection)
			{
				float sx, sy, sx1, sy1, sx2, sy2;
				float x1, y1, x2, y2, x3, y3, x4, y4;
				script.transform(selection_x, selection_y, selection_layer, 22, sx, sy);
				script.transform_size(selection_x1, selection_y1, selection_layer, 22, sx1, sy1);
				script.transform_size(selection_x2, selection_y2, selection_layer, 22, sx2, sy2);
				rotate(sx1, sy1, selection_angle, x1, y1);
				rotate(sx2, sy1, selection_angle, x2, y2);
				rotate(sx2, sy2, selection_angle, x3, y3);
				rotate(sx1, sy2, selection_angle, x4, y4);
				x1 += sx;
				y1 += sy;
				x2 += sx;
				y2 += sy;
				x3 += sx;
				y3 += sy;
				x4 += sx;
				y4 += sy;
				
				const float thickness = PropToolSettings::BoundingBoxLineWidth / script.zoom;
				
				draw_line(script.g, 22, 22, x1, y1, x2, y2, thickness, PropToolSettings::BoundingBoxColour);
				draw_line(script.g, 22, 22, x2, y2, x3, y3, thickness, PropToolSettings::BoundingBoxColour);
				draw_line(script.g, 22, 22, x3, y3, x4, y4, thickness, PropToolSettings::BoundingBoxColour);
				draw_line(script.g, 22, 22, x4, y4, x1, y1, thickness, PropToolSettings::BoundingBoxColour);
				const float mx = (x1 + x2) * 0.5;
				const float my = (y1 + y2) * 0.5;
				
				const float nx = cos(selection_angle - HALF_PI);
				const float ny = sin(selection_angle - HALF_PI);
				const float oh = PropToolSettings::RotationHandleOffset / script.zoom;
				
				draw_line(script.g, 22, 22,
					mx, my,
					mx + nx * oh, my + ny * oh,
					PropToolSettings::BoundingBoxLineWidth / script.zoom, PropToolSettings::BoundingBoxColour);
			}
		}
		
		// Anchor points
		
		if(highlight && selected_props_count > 0)
		{
			draw_rotation_anchor(selection_x, selection_y, selection_layer);
		}
		
		if(highlight && @hovered_prop != null && !hovered_prop.selected)
		{
			draw_rotation_anchor(hovered_prop.anchor_x, hovered_prop.anchor_y, hovered_prop.prop.layer());
		}
		
		// Selection rect
		
		if(state == Selecting)
		{
			script.g.draw_rectangle_world(
				22, 22,
				drag_start_x, drag_start_y, mouse.x, mouse.y,
				0, PropToolSettings::SelectRectFillColour);
			
			outline_rect(script.g, 22, 22,
				drag_start_x, drag_start_y, mouse.x, mouse.y,
				PropToolSettings::SelectRectLineWidth / script.zoom, PropToolSettings::SelectRectLineColour);
		}
	}
	
	private void draw_rotation_anchor(float x, float y, const int from_layer)
	{
		script.transform(x, y, from_layer, 22, x, y);
		
		script.g.draw_rectangle_world(22, 22,
			x - 5 / script.zoom, y - 1 / script.zoom,
			x + 5 / script.zoom, y + 1 / script.zoom, 0, PropToolSettings::BoundingBoxColour);
		script.g.draw_rectangle_world(22, 22,
			x - 5 / script.zoom, y - 1 / script.zoom,
			x + 5 / script.zoom, y + 1 / script.zoom, 90, PropToolSettings::BoundingBoxColour);
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		// Clear hovered state
		
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			highlighted_props[i].hovered = false;
		}
		
		@hovered_prop = null;
		
		// Start rotating from handle
		
		if(check_rotation_handle())
		{
			drag_rotation_handle = true;
			idle_start_rotating();
			return;
		}
		
		// Start dragging selection
		
		if(script.alt && mouse.left_press)
		{
			if(!script.shift && !script.ctrl)
			{
				select_none();
			}
			
			select_rect_pending = script.shift ? 1 : script.ctrl ? -1 : 0;
			
			drag_start_x = mouse.x;
			drag_start_y = mouse.y;
			state = Selecting;
			clear_highlighted_props();
			return;
		}
		
		// Start moving
		
		if(@pressed_prop != null && (mouse.delta_x != 0 || mouse.delta_y != 0))
		{
			idle_start_move();
			return;
		}
		
		// Pick props
		
		if(!script.space && !script.handles.mouse_over && !script.ui.is_mouse_over_ui)
		{
			pick_props();
			do_mouse_selection();
		}
		else
		{
			@pressed_prop = null;
		}
		
		// Start rotating from hovered prop
		
		if(@hovered_prop != null && mouse.middle_press)
		{
			drag_rotation_handle = false;
			idle_start_rotating();
			return;
		}
		
		// Scroll hover index offset
		
		if(mouse.scroll != 0 && !script.space && !script.ctrl && !script.alt)
		{
			hover_index_offset -= mouse.scroll;
		}
		
		// Adjust layer/sublayer
		
		if(mouse.scroll != 0 && (script.ctrl || script.alt))
		{
			idle_adjust_layer();
		}
		
		// Delete
		
		if(script.editor.key_check_gvb(GVB::Delete))
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				script.g.remove_prop(selected_props[i].prop);
			}
			
			select_none();
		}
		
		if(@hovered_prop != null && mouse.right_press)
		{
			script.g.remove_prop(hovered_prop.prop);
			hovered_prop.hovered = false;
			hovered_prop.selected = false;
			@hovered_prop = null;
		}
		
		clear_highlighted_props();
	}
	
	private void idle_start_move()
	{
		if(!pressed_prop.selected)
		{
			select_prop(pressed_prop, SelectAction::Set);
			clear_highlighted_props();
			temporary_selection = true;
		}
		
		if(selected_props_count == 0)
		{
			selection_layer = pressed_prop.prop.layer();
		}
		
		action_layer = pressed_prop.prop.layer();
		
		drag_start_x = mouse.x;
		drag_start_y = mouse.y;
		script.transform(drag_start_x, drag_start_y, 22, action_layer, drag_start_x, drag_start_y);
		selection_drag_start_x = selection_x;
		selection_drag_start_y = selection_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].start_drag();
		}
		
		pressed_prop.hovered = true;
		@pressed_prop = null;
		state = Moving;
	}
	
	private void idle_start_rotating()
	{
		if(!drag_rotation_handle && !hovered_prop.selected)
		{
			select_prop(hovered_prop, SelectAction::Set);
			clear_highlighted_props();
			temporary_selection = true;
		}
		
		if(selected_props_count == 1 && temporary_selection)
		{
			selection_layer = hovered_prop.prop.layer();
			selection_angle = hovered_prop.prop.rotation() * DEG2RAD;
		}
		
		selection_drag_start_angle = selection_angle;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].start_rotate(selection_x, selection_y, selection_angle * RAD2DEG);
		}
		
		float mouse_x, mouse_y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, mouse_x, mouse_y);
		drag_offset_angle = atan2(selection_y - mouse_y, selection_x - mouse_x) - selection_angle;
		
		clear_highlighted_props();
		state = Rotating;
	}
	
	private void idle_adjust_layer()
	{
		PropData@ prop_data = null;
		float x1, y1, x2, y2;
		
		if(script.shift)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				@prop_data = @selected_props[i];
				prop_data.shift_layer(mouse.scroll, script.alt);
			}
			
			x1 = selection_x + selection_x1;
			y1 = selection_y + selection_y1;
			x2 = selection_x + selection_x2;
			y2 = selection_y + selection_y2;
		}
		else if(@hovered_prop != null)
		{
			@prop_data = hovered_prop;
			hovered_prop.shift_layer(mouse.scroll, script.alt);
			x1 = prop_data.draw_x + prop_data.x1;
			y1 = prop_data.draw_y + prop_data.y1;
			x2 = prop_data.draw_x + prop_data.x2;
			y2 = prop_data.draw_y + prop_data.y2;
		}
		
		if(@prop_data != null)
		{
			script.info_overlay.show(
				x1, y1, x2, y2,
				prop_data.prop.layer() + '.' + prop_data.prop.sub_layer(), 0.75);
		}
		
		if(@hovered_prop != null)
		{
			show_prop_info(hovered_prop);
		}
		
		selection_angle = 0;
		update_selection_bounds();
		update_selection_layer();
	}
	
	private void state_moving()
	{
		if(script.space || script.escape_press || !mouse.left_down)
		{
			if(script.escape_press)
			{
				for(int i = 0; i < selected_props_count; i++)
				{
					selected_props[i].cancel_drag();
				}
			}
			
			clear_temporary_selection();
			
			if(!hide_selection_highlight.value)
			{
				check_rotation_handle();
			}
			
			state = Idle;
			return;
		}
		
		float start_x, start_y;
		float mouse_x, mouse_y;
		script.transform(mouse.x, mouse.y, 22, action_layer, mouse_x, mouse_y);
		snap(drag_start_x, drag_start_y, start_x, start_y);
		snap(mouse_x, mouse_y, mouse_x, mouse_y);
		const float drag_delta_x = mouse_x - start_x;
		const float drag_delta_y = mouse_y - start_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_drag(drag_delta_x, drag_delta_y);
		}
		
		selection_x = selection_drag_start_x + drag_delta_x;
		selection_y = selection_drag_start_y + drag_delta_y;
		
		if(!hide_selection_highlight.value)
		{
			check_rotation_handle();
		}
	}
	
	private void state_rotating()
	{
		if(script.space || script.escape_press || (drag_rotation_handle ? !mouse.left_down : !mouse.middle_down))
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].stop_rotate(script.escape_press);
			}
			
			if(script.escape_press)
			{
				selection_angle = selection_drag_start_angle;
			}
			
			if(!temporary_selection)
			{
				check_rotation_handle();
			}
			
			clear_temporary_selection();
			state = Idle;
			return;
		}
		
		float mouse_x, mouse_y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, mouse_x, mouse_y);
		const float angle = atan2(selection_y - mouse_y, selection_x - mouse_x);
		selection_angle = angle - drag_offset_angle;
		snap(selection_angle, selection_angle);
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_rotation(selection_angle * RAD2DEG);
		}
		
		if(!hide_selection_highlight.value || drag_rotation_handle)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].update();
			}
			
			check_rotation_handle(true);
		}
	}
	
	private void state_scaling()
	{
	}
	
	private void state_selecting()
	{
		clear_highlighted_props(true);
		
		if(script.escape_press)
		{
			state = Idle;
			return;
		}
		
		// Find props in selection rect
		
		const float y1 = min(drag_start_y, mouse.y);
		const float y2 = max(drag_start_y, mouse.y);
		const float x1 = min(drag_start_x, mouse.x);
		const float x2 = max(drag_start_x, mouse.x);
		
		int i = script.g.get_prop_collision(y1, y2, x1, x2);
		
		while(i-- > 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			const array<array<float>>@ outline = @PROP_OUTLINES[p.prop_set() - 1][p.prop_group()][p.prop_index() - 1];
			PropData@ prop_data = highlight_prop(p, outline);
			
			if(select_rect_pending == 0)
			{
				if(mouse.left_down)
				{
					prop_data.pending = 1;
				}
				else
				{
					select_prop(prop_data, SelectAction::Add);
				}
			}
			else if(select_rect_pending == 1)
			{
				if(!prop_data.selected)
				{
					if(mouse.left_down)
					{
						prop_data.pending = 1;
					}
					else
					{
						select_prop(prop_data, SelectAction::Add);
					}
				}
			}
			else if(select_rect_pending == -1)
			{
				if(mouse.left_down)
				{
					prop_data.pending = prop_data.selected ? -1 : -2;
				}
				else
				{
					select_prop(prop_data, SelectAction::Remove);
				}
			}
		}
		
		// Complete selection
		
		if(!mouse.left_down)
		{
			// Select
			state = Idle;
			return;
		}
	}
	
	//
	
	private bool check_rotation_handle(const bool force_highlight=false)
	{
		if(selected_props_count == 0 || temporary_selection)
			return false;
		
		float sx, sy, sx1, sy1, sx2, sy2;
		script.transform(selection_x, selection_y, selection_layer, 22, sx, sy);
		script.transform_size(selection_x1, selection_y1, selection_layer, 22, sx1, sy1);
		script.transform_size(selection_x2, selection_y2, selection_layer, 22, sx2, sy2);
		
		const float height = (sy2 - sy1) * 0.5;
		
		if(selected_props_count == 1)
		{
			sx = (sx + sx1 + sx + sx2) * 0.5;
			sy = (sy + sy1 + sy + sy2) * 0.5;
		}
		
		return script.handles.circle(
			sx + cos(selection_angle - HALF_PI) * (height + PropToolSettings::RotationHandleOffset / script.zoom),
			sy + sin(selection_angle - HALF_PI) * (height + PropToolSettings::RotationHandleOffset / script.zoom),
			PropToolSettings::RotateHandleSize, PropToolSettings::RotateHandleColour, PropToolSettings::RotateHandleHighlightColour, force_highlight);
	}
	
	// //////////////////////////////////////////////////////////
	// Selection
	// //////////////////////////////////////////////////////////
	
	private void do_mouse_selection()
	{
		if(mouse.left_press)
		{
			// Add or remove from selection on shift/ctrl press
			if(script.shift || script.ctrl)
			{
				select_prop(hovered_prop, script.shift ? SelectAction::Add : SelectAction::Remove);
				mouse_press_modified = true;
			}
			else
			{
				@pressed_prop = hovered_prop;
			}
		}
		else if(mouse.left_release)
		{
			// Deselect all on normal click in empty space
			if(!mouse_press_moved && !mouse_press_modified)
			{
				select_prop(hovered_prop, SelectAction::Set);
			}
			
			mouse_press_modified = false;
			@pressed_prop = null;
		}
	}
	
	private void select_prop(PropData@ prop_data, const SelectAction action)
	{
		if(action == SelectAction::Set)
		{
			while(selected_props_count > 0)
			{
				PropData@ selected_pro_data = @selected_props[--selected_props_count];
				selected_pro_data.selected = false;
			}
			
			selection_layer = 0;
			selection_angle = 0;
		}
		
		if(
			@prop_data == null ||
			action == SelectAction::Remove && !prop_data.selected ||
			action == SelectAction::Add && prop_data.selected
		)
			return;
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(selected_props_count >= selected_props_size)
			{
				selected_props.resize(selected_props_size += 32);
			}
			
			@selected_props[selected_props_count++] = prop_data;
			prop_data.selected = true;
			
			if(int(prop_data.prop.layer()) > selection_layer)
			{
				selection_layer = prop_data.prop.layer();
			}
		}
		else
		{
			selected_props.removeAt(selected_props.findByRef(@prop_data));
			prop_data.selected = false;
			selected_props_count--;
			
			if(int(prop_data.prop.layer()) >= selection_layer)
			{
				update_selection_layer();
			}
		}
		
		selection_angle = 0;
		update_selection_bounds();
	}
	
	private void select_none()
	{
		select_prop(null, SelectAction::Set);
	}
	
	private void update_selection_bounds()
	{
		if(selected_props_count == 0)
			return;
		
		PropData@ prop_data = @selected_props[0];
		selection_x1 = prop_data.x + prop_data.x1;
		selection_y1 = prop_data.y + prop_data.y1;
		selection_x2 = prop_data.x + prop_data.x2;
		selection_y2 = prop_data.y + prop_data.y2;
		
		for(int i = selected_props_count - 1; i >= 1; i--)
		{
			@prop_data = @selected_props[i];
			
			if(prop_data.x + prop_data.x1 < selection_x1) selection_x1 = prop_data.x + prop_data.x1;
			if(prop_data.y + prop_data.y1 < selection_y1) selection_y1 = prop_data.y + prop_data.y1;
			if(prop_data.x + prop_data.x2 > selection_x2) selection_x2 = prop_data.x + prop_data.x2;
			if(prop_data.y + prop_data.y2 > selection_y2) selection_y2 = prop_data.y + prop_data.y2;
		}
		
		selection_x = selected_props_count > 1 ? (selection_x1 + selection_x2) * 0.5 : prop_data.anchor_x;
		selection_y = selected_props_count > 1 ? (selection_y1 + selection_y2) * 0.5 : prop_data.anchor_y;
		selection_x1 -= selection_x;
		selection_y1 -= selection_y;
		selection_x2 -= selection_x;
		selection_y2 -= selection_y;
	}
	
	private void update_selection_layer()
	{
		selection_layer = 0;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			prop@ p = selected_props[i].prop;
			
			if(int(p.layer()) > selection_layer)
			{
				selection_layer = p.layer();
			}
		}
	}
	
	private void clear_temporary_selection()
	{
		if(!temporary_selection)
			return;
		
		select_none();
		temporary_selection = false;
	}
	
	// Highligts
	
	private void show_prop_info(PropData@ prop_data)
	{
		prop@ p = prop_data.prop;
		int index = prop_index_to_array_index(p.prop_set(), p.prop_group(), p.prop_index());
		const PropIndex@ prop_index = @PROP_INDICES[p.prop_group()][index];
		
		info_label.text =
			// Group and name
			string::nice(PROP_GROUP_NAMES[p.prop_group()]) + '::' + prop_index.name + '\n' +
			// Layer
			p.layer() + '.' + p.sub_layer();
		
		script.ui.show_tooltip(info_popup, toolbar);
	}
	
	private PropData@ is_prop_highlighted(prop@ prop)
	{
		const string key = prop.id() + '';
		
		return highlighted_props_map.exists(key)
			? cast<PropData@>(highlighted_props_map[key])
			: null;
	}
	
	private PropData@ highlight_prop(prop@ prop, const array<array<float>>@ outline)
	{
		const string key = prop.id() + '';
		PropData@ prop_data;
		
		if(highlighted_props_map.exists(key))
			return cast<PropData@>(highlighted_props_map[key]);
		
		@prop_data = prop_data_pool_count > 0
			? @prop_data_pool[--prop_data_pool_count]
			: PropData();
		
		@prop_data.prop = prop;
		prop_data.key = key;
		prop_data.pending = 0;
		@prop_data.outline = @outline;
		
		if(highlighted_props_count >= highlighted_props_size)
		{
			highlighted_props.resize(highlighted_props_size += 32);
		}
		
		@highlighted_props[highlighted_props_count++] = @prop_data;
		@highlighted_props_map[key] = @prop_data;
		
		prop_data.init(script);
		
		return prop_data;
	}
	
	private void clear_highlighted_props(const bool clear_pending=false)
	{
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			PropData@ prop_data = @highlighted_props[i];
			
			if(clear_pending)
			{
				prop_data.pending = 0;
				
				if(prop_data.selected)
					continue;
			}
			else if(prop_data.hovered || prop_data.selected)
			{
				continue;
			}
			
			if(prop_data_pool_count >= prop_data_pool_size)
			{
				prop_data_pool.resize(prop_data_pool_size += 32);
			}
			
			@prop_data_pool[prop_data_pool_count++] = @prop_data;
			@highlighted_props[i] = @highlighted_props[--highlighted_props_count];
			highlighted_props_map.delete(prop_data.key);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Picking
	// //////////////////////////////////////////////////////////
	
	private void pick_props()
	{
		// Find all props under the mouse
		
		const float radius = max(PropToolSettings::SmallPropRadius, pick_radius.value) / script.zoom;
		int i = script.g.get_prop_collision(mouse.y - radius, mouse.y + radius, mouse.x - radius, mouse.x + radius);
		
		array<PropSortingData>@ props_under_mouse = @this.props_under_mouse;
		int num_props_under_mouse = 0;
		
		if(props_under_mouse_size < i)
		{
			props_under_mouse.resize(props_under_mouse_size = i + 32);
		}
		
		while(i-- > 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			
			const float prop_x = p.x();
			const float prop_y = p.y();
			const float rotation = p.rotation() * DEG2RAD * sign(p.scale_x()) * sign(p.scale_y());
			const float layer_scale = p.layer() <= 5 ? script.g.layer_scale(p.layer()) : 1.0;
			const float backdrop_scale = p.layer() <= 5 ? 2.0 : 1.0;
			const float scale_x = p.scale_x() / layer_scale * backdrop_scale;
			const float scale_y = p.scale_y() / layer_scale * backdrop_scale;
			
			// Calculate mouse "local" position relative to prop rotation and scale
			
			float local_x, local_y;
			const float layer_mx = script.g.mouse_x_world(0, p.layer());
			const float layer_my = script.g.mouse_y_world(0, p.layer());
			
			rotate(
				(layer_mx - prop_x) / scale_x,
				(layer_my - prop_y) / scale_y,
				-rotation, local_x, local_y);
			
			// Check for overlap with tiles, but allow interacting with selected props through tiles
			
			PropData@ prop_data = is_prop_highlighted(p);
			
			if((@prop_data == null || !prop_data.selected) && !pick_through_tiles.value && hittest_tiles(p.layer(), p.sub_layer()))
				continue;
			
			// Check if the mouse is inside to the prop
			
			const array<array<float>>@ outline = @PROP_OUTLINES[p.prop_set() - 1][p.prop_group()][p.prop_index() - 1];
			const array<bool>@ holes = @PROP_OUTLINES_HOLES_INFO[p.prop_set() - 1][p.prop_group()][p.prop_index() - 1];
			
			bool is_inside = hittest_prop(p, local_x, local_y, outline, holes);
			
			// If the mouse is not directly inside the prop outline, check if it close by, within some threshold
			
			const bool is_close = !is_inside
				? check_prop_distance(p, local_x, local_y, outline, holes)
				: false;
			
			if(is_inside || is_close)
			{
				PropSortingData@ data = @props_under_mouse[num_props_under_mouse++];
				@data.prop = @p;
				@data.outline = @outline;
				data.is_inside = is_inside ? 1 : 0;
				data.scene_index = i;
			}
		}
		
		if(num_props_under_mouse == 0)
			return;
		
		// Sort to find the top most prop
		
		props_under_mouse.sortAsc(0, num_props_under_mouse);
		const int selected_index = (num_props_under_mouse - (hover_index_offset % num_props_under_mouse) - 1) % num_props_under_mouse;
		
		PropSortingData@ sorting_data = @props_under_mouse[selected_index];
		PropData@ prop_data = highlight_prop(@sorting_data.prop, @sorting_data.outline);
		prop_data.hovered = true;
		@hovered_prop = prop_data;
	}
	
	private bool hittest_tiles(const int prop_layer, const int prop_sublayer)
	{
		for(int layer = prop_layer; layer <= 20; layer++)
		{
			if(layer == prop_layer && prop_sublayer >= 10)
				continue;
			
			const float mx = script.g.mouse_x_world(0, layer);
			const float my = script.g.mouse_y_world(0, layer);
			const int tx = floor_int(mx / 48);
			const int ty = floor_int(my / 48);
			tileinfo@ tile = script.g.get_tile(tx, ty, layer);
			
			if(!tile.solid())
				continue;
			
			float normal_x, normal_y;
			
			if(point_in_tile(mx, my, tx, ty, tile.type(), normal_x, normal_y, layer))
				return true;
		}
		
		return false;
	}
	
	private bool hittest_prop(prop@ p, const float local_x, const float local_y,
		const array<array<float>>@ outline,
		const array<bool>@ holes)
	{
		bool inside = false;
		
		for(uint i = 0; i < outline.length(); i++)
		{
			if(point_in_polygon(local_x, local_y, @outline[i]))
			{
				if(pick_ignore_holes.value)
					return true;
				
				inside = holes[i] ? !inside : true;
			}
		}
		
		return inside;
	}
	
	private bool check_prop_distance(prop@ p, const float local_x, const float local_y,
		const array<array<float>>@ outline,
		const array<bool>@ holes)
	{
		const string prop_key = p.prop_set() + '.' + p.prop_group() + '.' + p.prop_index();
		const float prop_radius = (PropToolSettings::PropRadii.exists(prop_key)
			? float(PropToolSettings::PropRadii[prop_key])
			: pick_radius.value) / script.zoom;
		
		for(uint i = 0; i < outline.length(); i++)
		{
			if(distance_to_polygon_sqr(local_x, local_y, @outline[0]) <= prop_radius * prop_radius)
				return true;
			
			if(pick_ignore_holes.value)
				break;
		}
		
		return false;
	}
	
	// //////////////////////////////////////////////////////////
	// Other
	// //////////////////////////////////////////////////////////
	
	private void snap(const float x, const float y, float &out out_x, float &out out_y)
	{
		const float snap = get_snap_size();
		
		if(snap != 0)
		{
			out_x = round(x / snap) * snap;
			out_y = round(y / snap) * snap;
		}
		else
		{
			out_x = x;
			out_y = y;
		}
	}
	
	void snap(const float angle, float &out out_angle)
	{
		const float snap = get_snap_angle() * DEG2RAD;
		
		if(snap != 0)
		{
			out_angle = round(angle / snap) * snap;
		}
		else
		{
			out_angle = angle;
		}
	}
	
	private float get_snap_size()
	{
		if(script.shift)
			return 48;
		
		if(script.ctrl)
			return 24;
		
		if(script.alt)
			return 5;
		
		return 0;
	}
	
	private float get_snap_angle()
	{
		if(script.shift)
			return 45;
		
		if(script.ctrl)
			return 22.5;
		
		if(script.alt)
			return 5;
		
		return 0;
	}
	
}