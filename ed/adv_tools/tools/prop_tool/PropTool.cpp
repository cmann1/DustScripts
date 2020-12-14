#include '../../misc/SelectAction.cpp';
#include '../../misc/AlignmentEdge.cpp';
#include '../../../../lib/tiles/common.cpp';
#include '../../../../lib/props/common.cpp';
#include '../../../../lib/props/data.cpp';
#include '../../../../lib/props/outlines.cpp';

#include 'PropToolSettings.cpp';
#include 'PropToolState.cpp';
#include 'PropSortingData.cpp';
#include 'PropAlignData.cpp';
#include 'PropData.cpp';
#include 'PropToolToolbar.cpp';
#include 'PropsClipboardData.cpp';
#include 'PropExportType.cpp';
#include 'PropToolExporter.cpp';

const string PROP_TOOL_SPRITES_BASE = SPRITES_BASE + 'prop_tool/';
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
	
	private int props_align_data_size = 32;
	private array<PropAlignData> props_align_data(props_align_data_size);
	
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
	
	private float drag_start_x, drag_start_y;
	private float drag_offset_angle;
	private bool drag_rotation_handle;
	private float drag_scale_start_distance;
	private float drag_selection_x1, drag_selection_y1;
	private float drag_selection_x2, drag_selection_y2;
	
	private int select_rect_pending;
	private int action_layer;
	
	private int selection_layer;
	private float selection_x, selection_y;
	private float selection_x1, selection_y1;
	private float selection_x2, selection_y2;
	private float selection_angle;
	private float selection_drag_start_x, selection_drag_start_y;
	private float selection_drag_start_angle;
	private WorldBoundingBox selection_bounding_box;
	
	float origin_align_x, origin_align_y;
	
	private bool has_custom_anchor;
	private int custom_anchor_layer = 19;
	private float custom_anchor_x, custom_anchor_y;
	private float custom_anchor_offset_x, custom_anchor_offset_y;
	
	private PropToolToolbar toolbar;
	
	// Settings
	
	BoolSetting@ pick_through_tiles;
	FloatSetting@ custom_grid;
	StringSetting@ default_origin;
	BoolSetting@ custom_anchor_lock;
	BoolSetting@ show_selection;
	BoolSetting@ show_info;
	IntSetting@ export_type;
	
	BoolSetting@ pick_ignore_holes;
	FloatSetting@ pick_radius;
	
	PropTool()
	{
		super('Prop Tool');
		
		init_shortcut_key(VK::Q);
	}
	
	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_prop_tool');
		
		toolbar.build_sprites(msg);
	}
	
	void create(AdvToolScript@ script, ToolGroup@ group) override
	{
		Tool::create(script, group);
		
		set_icon(SPRITE_SET, 'icon_prop_tool', 33, 33);
		
		@mouse = @script.mouse;
		@selection_bounding_box.script = script;
	}
	
	void on_init() override
	{
		@pick_through_tiles	= script.get_bool(this, 'pick_through_tiles', false);
		@custom_grid		= script.get_float(this, 'custom_grid', 5);
		@default_origin		= script.get_string(this, 'default_origin', 'centre');
		@custom_anchor_lock	= script.get_bool(this, 'custom_anchor_lock', false);
		@show_selection		= script.get_bool(this, 'show_selection', true);
		@show_info			= script.get_bool(this, 'show_info', true);
		
		@pick_ignore_holes	= script.get_bool(this, 'pick_ignore_holes', true);
		@pick_radius		= script.get_float(this, 'pick_radius', 2);
		
		@export_type		= script.get_int(this, 'export_type', PropExportType::SpriteBatch);
		
		update_alignments_from_origin();
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	void on_editor_unloaded() override
	{
		select_none();
		clear_highlighted_props(true);
		clear_temporary_selection();
		state = PropToolState::Idle;
		
		Tool::on_editor_unloaded();
	}
	
	Tool@ on_shortcut_key() override
	{
		return selected ? script.get_tool('Props') : @this;
	}
	
	protected void on_select_impl()
	{
		script.hide_gui(true);
		
		toolbar.show(script, this);
		
		clear_custom_anchor();
		drag_rotation_handle = false;
	}
	
	protected void on_deselect_impl()
	{
		script.hide_gui(false);
		
		select_none();
		state = Idle;
		temporary_selection = false;
		clear_highlighted_props();
		
		toolbar.hide();
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
			case PropToolState::Idle: state_idle(); break;
			case PropToolState::Moving: state_moving(); break;
			case PropToolState::Rotating: state_rotating(); break;
			case PropToolState::Scaling: state_scaling(); break;
			case PropToolState::Selecting: state_selecting(); break;
		}
		
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			highlighted_props[i].step();
		}
		
		if(@hovered_prop != null)
		{
			if(@hovered_prop != @previous_hovered_prop)
			{
				toolbar.show_prop_info(hovered_prop);
				@previous_hovered_prop = hovered_prop;
			}
		}
		else
		{
			toolbar.hide_info_popup();
			@previous_hovered_prop = null;
		}
		
		performing_action = state != Idle && state != Selecting;
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		const bool highlight = !performing_action || show_selection.value;
		
		if(highlight)
		{
			// Highlights
			
			for(int i = 0; i < highlighted_props_count; i++)
			{
				highlighted_props[i].draw();
			}
			
			// Bounding box
			
			if(selected_props_count > 0 && !temporary_selection)
			{
				draw_selection_bounding_box();
			}
		}
		
		// Anchor points
		
		if(highlight && !has_custom_anchor && selected_props_count > 0)
		{
			draw_rotation_anchor(selection_x, selection_y, selection_layer);
		}
		
		if(highlight && !has_custom_anchor && @hovered_prop != null && !hovered_prop.selected)
		{
			draw_rotation_anchor(hovered_prop.anchor_x, hovered_prop.anchor_y, hovered_prop.prop.layer());
		}
		
		if(has_custom_anchor)
		{
			draw_rotation_anchor(custom_anchor_x, custom_anchor_y, custom_anchor_layer, true);
			
			if(selected_props_count > 0 && !script.is_same_parallax(custom_anchor_layer, selection_layer))
			{
				const uint clr = multiply_alpha(Settings::BoundingBoxColour, 0.5);
				float x1, y1, x2, y2;
				
				script.transform(custom_anchor_x, custom_anchor_y, custom_anchor_layer, 22, x1, y1);
				script.transform(custom_anchor_x, custom_anchor_y, selection_layer, 22, x2, y2);
				
				draw_line(script.g, 22, 22, x1, y1, x2, y2, 1 / script.zoom, clr);
				draw_rotation_anchor(custom_anchor_x, custom_anchor_y, selection_layer, true, 1, clr);
			}
		}
		
		// Selection rect
		
		if(state == Selecting)
		{
			script.draw_select_rect(drag_start_x, drag_start_y, mouse.x, mouse.y);
		}
	}
	
	private void draw_selection_bounding_box()
	{
		float sx, sy, sx1, sy1, sx2, sy2;
		float x1, y1, x2, y2, x3, y3, x4, y4;
		script.transform(selection_x, selection_y, selection_layer, 22, sx, sy);
		script.transform_size(min(selection_x1, selection_x2), min(selection_y1, selection_y2), selection_layer, 22, sx1, sy1);
		script.transform_size(max(selection_x1, selection_x2), max(selection_y1, selection_y2), selection_layer, 22, sx2, sy2);
		
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
		
		const float thickness = Settings::BoundingBoxLineWidth / script.zoom;
		
		draw_line(script.g, 22, 22, x1, y1, x2, y2, thickness, Settings::BoundingBoxColour);
		draw_line(script.g, 22, 22, x2, y2, x3, y3, thickness, Settings::BoundingBoxColour);
		draw_line(script.g, 22, 22, x3, y3, x4, y4, thickness, Settings::BoundingBoxColour);
		draw_line(script.g, 22, 22, x4, y4, x1, y1, thickness, Settings::BoundingBoxColour);
		const float mx = (x1 + x2) * 0.5;
		const float my = (y1 + y2) * 0.5;
		
		const float nx = cos(selection_angle - HALF_PI);
		const float ny = sin(selection_angle - HALF_PI);
		const float oh = (Settings::RotationHandleOffset - Settings::RotateHandleSize) / script.zoom;
		
		draw_line(script.g, 22, 22,
			mx, my,
			mx + nx * oh, my + ny * oh,
			Settings::BoundingBoxLineWidth / script.zoom, Settings::BoundingBoxColour);
	}
	
	private void draw_rotation_anchor(float x, float y, const int from_layer, const bool lock=false, const float size = 1.4, const uint clr=Settings::BoundingBoxColour)
	{
		script.transform(x, y, from_layer, 22, x, y);
		
		const float length = (!lock ? 5 : 8) * size / script.zoom;
		
		script.g.draw_rectangle_world(22, 22,
			x - length, y - 1 / script.zoom,
			x + length, y + 1 / script.zoom, 0, clr);
		script.g.draw_rectangle_world(22, 22,
			x - length, y - 1 / script.zoom,
			x + length, y + 1 / script.zoom, 90, clr);
		
		if(lock)
		{
			draw_circle(script.g, x, y, 4 * size / script.zoom, 12, 22, 22, 1 / script.zoom, clr);
		}
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
		
		const bool start_rotating = check_rotation_handle();
		const bool start_scaling   = check_scale_handle();
		
		// Start rotating from handle
		
		if(start_rotating)
		{
			drag_rotation_handle = true;
			idle_start_rotating();
			return;
		}
		
		// Start scaling from handle
		
		if(start_scaling)
		{
			idle_start_scaling();
			return;
		}
		
		// Start dragging selection
		
		if(script.mouse_in_scene && script.alt && mouse.left_press)
		{
			if(!script.shift && !script.ctrl)
			{
				select_none();
			}
			
			select_rect_pending = script.shift ? 1 : script.ctrl ? -1 : 0;
			
			drag_start_x = mouse.x;
			drag_start_y = mouse.y;
			clear_highlighted_props();
			state = Selecting;
			script.ui.mouse_enabled = false;
			return;
		}
		
		// Start moving
		
		if(@pressed_prop != null && (mouse.delta_x != 0 || mouse.delta_y != 0))
		{
			idle_start_move();
			return;
		}
		
		// Move with arrow keys
		// Flip
		
		if(script.scene_focus)
		{
			if(script.key_repeat_gvb(GVB::LeftArrow))
			{
				shift_props(script.ctrl ? -20 : script.shift ? -10 : -1, 0);
			}
			else if(script.key_repeat_gvb(GVB::RightArrow))
			{
				shift_props(script.ctrl ? 20 : script.shift ? 10 : 1, 0);
			}
			else if(script.key_repeat_gvb(GVB::UpArrow))
			{
				shift_props(0, script.ctrl ? -20 : script.shift ? -10 : -1);
			}
			else if(script.key_repeat_gvb(GVB::DownArrow))
			{
				shift_props(0, script.ctrl ? 20 : script.shift ? 10 : 1);
			}
			else if(script.editor.key_check_pressed_gvb(GVB::BracketOpen))
			{
				flip_props(true, false);
			}
			else if(script.editor.key_check_pressed_gvb(GVB::BracketClose))
			{
				flip_props(false, true);
			}
		}
		
		// Pick props
		
		if(script.mouse_in_scene && !script.space && !script.handles.mouse_over)
		{
			pick_props();
			do_mouse_selection();
		}
		else
		{
			@pressed_prop = null;
		}
		
		// Start rotating from hovered prop
		
		if(script.mouse_in_scene && @hovered_prop != null && !script.shift && !script.alt && mouse.middle_press)
		{
			drag_rotation_handle = false;
			idle_start_rotating();
			return;
		}
		
		// Set or clear custom anchor position, or set custom anchor layer
		
		if(script.mouse_in_scene && script.shift && !script.ctrl && mouse.scroll != 0 && has_custom_anchor)
		{
			adjust_custom_anchor_layer(mouse.scroll);
			show_custom_anchor_info();
		}
		
		if(script.mouse_in_scene && mouse.middle_press)
		{
			if(script.shift || script.ctrl)
			{
				if(!has_custom_anchor || script.ctrl)
				{
					if(selected_props_count > 0)
					{
						custom_anchor_layer = selection_layer;
					}
					else if(@hovered_prop != null)
					{
						custom_anchor_layer = hovered_prop.prop.layer();
					}
				}
				
				if(!script.ctrl)
				{
					script.transform(mouse.x, mouse.y, 22, custom_anchor_layer, custom_anchor_x, custom_anchor_y);
				}
				else
				{
					custom_anchor_x = selection_x;
					custom_anchor_y = selection_y;
				}
				
				has_custom_anchor = true;
				toolbar.update_buttons(selected_props_count);
			}
			else if(script.alt)
			{
				has_custom_anchor = false;
				toolbar.update_buttons(selected_props_count);
			}
		}
		
		// Scroll hover index offset
		
		if(mouse.scroll != 0 && !script.space && !script.ctrl && !script.alt && !script.shift)
		{
			hover_index_offset -= mouse.scroll;
		}
		
		// Adjust layer/sublayer
		
		if(mouse.scroll != 0 && (script.ctrl || script.alt))
		{
			idle_adjust_layer();
		}
		
		// Delete
		
		if(script.scene_focus && script.editor.key_check_gvb(GVB::Delete))
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				script.g.remove_prop(selected_props[i].prop);
			}
			
			select_none();
		}
		
		if(script.mouse_in_scene && @hovered_prop != null && (mouse.right_press || script.shift && mouse.right_down))
		{
			if(hovered_prop.selected)
			{
				select_prop(hovered_prop, SelectAction::Remove);
			}
			
			script.g.remove_prop(hovered_prop.prop);
			hovered_prop.hovered = false;
			@hovered_prop = null;
		}
		
		// Copy/Paste
		
		if(script.scene_focus && selected_props_count > 0 && script.ctrl && script.editor.key_check_pressed_vk(VK::C))
		{
			copy_selected_props();
		}
		
		if(script.scene_focus && script.ctrl && script.editor.key_check_pressed_vk(VK::V))
		{
			paste(script.shift, script.alt);
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
		@hovered_prop = pressed_prop;
		@pressed_prop = null;
		state = Moving;
		script.ui.mouse_enabled = false;
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
		
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		const int anchor_layer = has_custom_anchor ? custom_anchor_layer : selection_layer;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].start_rotate(anchor_x, anchor_y, selection_angle * RAD2DEG);
		}
		
		float x, y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, x, y);
		drag_offset_angle = atan2(anchor_y - y, anchor_x - x) - selection_angle;
		
		if(has_custom_anchor)
		{
			custom_anchor_offset_x = selection_x - custom_anchor_x;
			custom_anchor_offset_y = selection_y - custom_anchor_y;
		}
		
		clear_highlighted_props();
		state = Rotating;
		script.ui.mouse_enabled = false;
	}
	
	private void idle_start_scaling()
	{
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].start_scale(anchor_x, anchor_y);
		}
		
		drag_selection_x1 = selection_x1;
		drag_selection_y1 = selection_y1;
		drag_selection_x2 = selection_x2;
		drag_selection_y2 = selection_y2;
		
		float x, y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, x, y);
		drag_scale_start_distance = distance(x, y, anchor_x, anchor_y);
		drag_start_x = mouse.x;
		drag_start_y = mouse.y;
		
		if(has_custom_anchor)
		{
			custom_anchor_offset_x = selection_x - custom_anchor_x;
			custom_anchor_offset_y = selection_y - custom_anchor_y;
		}
		
		clear_highlighted_props();
		state = Scaling;
		script.ui.mouse_enabled = false;
	}
	
	private void idle_adjust_layer()
	{
		PropData@ prop_data = null;
		IWorldBoundingBox@ bounding_box = null;
		
		if(script.shift)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				@prop_data = @selected_props[i];
				prop_data.shift_layer(mouse.scroll, script.alt);
			}
			
			selection_bounding_box.x1 = selection_x + selection_x1;
			selection_bounding_box.y1 = selection_y + selection_y1;
			selection_bounding_box.x2 = selection_x + selection_x2;
			selection_bounding_box.y2 = selection_y + selection_y2;
		}
		else if(@hovered_prop != null)
		{
			@prop_data = hovered_prop;
			hovered_prop.shift_layer(mouse.scroll, script.alt);
			@bounding_box = prop_data;
		}
		
		selection_angle = 0;
		update_selection_bounds();
		update_selection_layer();
		
		if(@bounding_box != null)
		{
			script.show_layer_sublayer_overlay(bounding_box, prop_data.prop.layer(), prop_data.prop.sub_layer());
		}
		else if(@prop_data != null)
		{
			selection_bounding_box.layer = selection_layer;
			script.show_layer_sublayer_overlay(@selection_bounding_box, prop_data.prop.layer(), prop_data.prop.sub_layer());
		}
		
		if(@hovered_prop != null)
		{
			toolbar.show_prop_info(hovered_prop);
		}
	}
	
	private void state_moving()
	{
		if(script.escape_press || !mouse.left_down)
		{
			if(script.escape_press)
			{
				for(int i = 0; i < selected_props_count; i++)
				{
					selected_props[i].cancel_drag();
				}
				
				selection_x = selection_drag_start_x;
				selection_y = selection_drag_start_y;
			}
			
			clear_temporary_selection();
			
			if(show_selection.value)
			{
				check_rotation_handle();
			}
			
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		float start_x, start_y;
		float mouse_x, mouse_y;
		script.transform(mouse.x, mouse.y, 22, action_layer, mouse_x, mouse_y);
		script.snap(drag_start_x, drag_start_y, start_x, start_y, custom_grid.value);
		script.snap(mouse_x, mouse_y, mouse_x, mouse_y, custom_grid.value);
		const float drag_delta_x = mouse_x - start_x;
		const float drag_delta_y = mouse_y - start_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_drag(drag_delta_x, drag_delta_y);
		}
		
		selection_x = selection_drag_start_x + drag_delta_x;
		selection_y = selection_drag_start_y + drag_delta_y;
		
		if(show_selection.value)
		{
			check_rotation_handle();
			check_scale_handle();
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
			
			if(!temporary_selection && show_selection.value)
			{
				check_rotation_handle();
				check_scale_handle();
			}
			
			drag_rotation_handle = false;
			clear_temporary_selection();
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		const int anchor_layer = has_custom_anchor ? custom_anchor_layer : selection_layer;
		
		float x, y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, x, y);
		const float angle = atan2(anchor_y - y, anchor_x - x);
		selection_angle = angle - drag_offset_angle;
		script.snap(selection_angle, selection_angle);
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_rotation(selection_angle * RAD2DEG);
		}
		
		if(has_custom_anchor)
		{
			rotate(custom_anchor_offset_x, custom_anchor_offset_y, selection_angle - selection_drag_start_angle, x, y);
			selection_x = custom_anchor_x + x;
			selection_y = custom_anchor_y + y;
		}
		
		if(show_selection.value)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].update();
			}
			
			check_rotation_handle(true);
			check_scale_handle();
		}
	}
	
	private void state_scaling()
	{
		if(script.space || script.escape_press || !mouse.left_down)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].stop_scale(script.escape_press);
			}
			
			if(!temporary_selection && show_selection.value)
			{
				check_rotation_handle();
				check_scale_handle(false);
			}
			
			clear_temporary_selection();
			state = Idle;
			script.ui.mouse_enabled = true;
			return;
		}
		
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		
		float x, y;
		script.transform(mouse.x, mouse.y, 22, selection_layer, x, y);
		const float new_drag_distance = distance(x, y, anchor_x, anchor_y);
		const float length = magnitude(drag_start_x - anchor_x, drag_start_y - anchor_y);
		project(
			x - anchor_x, y - anchor_y,
			(drag_start_x - anchor_x) / length, (drag_start_y - anchor_y) / length,
			x, y);
		const float distance_sign = dot(
				x, y,
				drag_start_x - anchor_x, drag_start_y - anchor_y) < 0 ? -1 : 1;
		const float scale = new_drag_distance / drag_scale_start_distance * distance_sign;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_scale(scale, scale);
		}
		
		if(has_custom_anchor)
		{
			selection_x = custom_anchor_x + custom_anchor_offset_x * scale;
			selection_y = custom_anchor_y + custom_anchor_offset_y * scale;
		}
		
		selection_x1 = drag_selection_x1 * scale;
		selection_y1 = drag_selection_y1 * scale;
		selection_x2 = drag_selection_x2 * scale;
		selection_y2 = drag_selection_y2 * scale;
		
		if(show_selection.value)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].update();
			}
			
			check_rotation_handle();
			check_scale_handle(true);
		}
	}
	
	private void state_selecting()
	{
		clear_highlighted_props(true);
		
		if(script.escape_press)
		{
			state = Idle;
			script.ui.mouse_enabled = true;
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
					prop_data.pending_selection = 1;
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
						prop_data.pending_selection = 1;
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
					prop_data.pending_selection = prop_data.selected ? -1 : -2;
				}
				else
				{
					select_prop(prop_data, SelectAction::Remove);
				}
			}
		}
		
		check_rotation_handle();
		check_scale_handle();
		
		// Complete selection
		
		if(!mouse.left_down)
		{
			state = Idle;
			script.ui.mouse_enabled = true;
		}
	}
	
	//
	
	private void get_handle_position(const bool vertical, const float offset, float &out x, float &out y, const bool allow_flipped=true)
	{
		float sx, sy, sx1, sy1, sx2, sy2;
		float x1, y1, x2, y2, x3, y3, x4, y4;
		script.transform(selection_x, selection_y, selection_layer, 22, sx, sy);
		
		if(!allow_flipped)
		{
			script.transform_size(min(selection_x1, selection_x2), min(selection_y1, selection_y2), selection_layer, 22, sx1, sy1);
			script.transform_size(max(selection_x1, selection_x2), max(selection_y1, selection_y2), selection_layer, 22, sx2, sy2);
		}
		else
		{
			script.transform_size(selection_x1, selection_y1, selection_layer, 22, sx1, sy1);
			script.transform_size(selection_x2, selection_y2, selection_layer, 22, sx2, sy2);
		}
		
		rotate(sx1, sy1, selection_angle, x1, y1);
		rotate(sx2, sy1, selection_angle, x2, y2);
		rotate(sx2, sy2, selection_angle, x3, y3);
		rotate(sx1, sy2, selection_angle, x4, y4);
		
		if(vertical)
		{
			sx += (x1 + x2) * 0.5;
			sy += (y1 + y2) * 0.5;
		}
		else
		{
			sx += (x2 + x3) * 0.5;
			sy += (y2 + y3) * 0.5;
		}
		
		const float angle = vertical ? selection_angle - HALF_PI : selection_angle;
		
		x = sx + cos(angle) * (offset / script.zoom);
		y = sy + sin(angle) * (offset / script.zoom);
	}
	
	private bool check_rotation_handle(const bool force_highlight=false)
	{
		if(selected_props_count == 0 || temporary_selection)
			return false;
		
		float x, y;
		get_handle_position(true, Settings::RotationHandleOffset, x, y, false);
		
		return script.handles.circle(
			x, y,
			Settings::RotateHandleSize, Settings::RotateHandleColour, Settings::RotateHandleHoveredColour, force_highlight);
	}
	
	private bool check_scale_handle(const bool force_highlight=false)
	{
		if(selected_props_count == 0 || temporary_selection)
			return false;
		
		float x, y;
		get_handle_position(false, 0, x, y);
		
		return script.handles.square(
			x, y,
			Settings::ScaleHandleSize, selection_angle * RAD2DEG,
			Settings::RotateHandleColour, Settings::RotateHandleHoveredColour, force_highlight);
	}
	
	private void show_custom_anchor_info()
	{
		float x, y;
		script.transform_size(5 / script.zoom, 5 / script.zoom, 22, custom_anchor_layer, x, y);
		
		selection_bounding_box.layer = custom_anchor_layer;
		selection_bounding_box.x1 = custom_anchor_x - x;
		selection_bounding_box.y1 = custom_anchor_y - y;
		selection_bounding_box.x2 = custom_anchor_x + x;
		selection_bounding_box.y2 = custom_anchor_y + y;
		
		script.info_overlay.show(
			selection_bounding_box,
			'Custom Anchor Layer: ' + custom_anchor_layer, 0.75);
	}
	
	private void adjust_custom_anchor_layer(int dir)
	{
		dir = sign(dir * 2 + 1);
		
		if(custom_anchor_layer >= 12)
		{
			custom_anchor_layer = dir == 1 ? 0 : 11;
		}
		else if(custom_anchor_layer == 0 && dir == -1 || custom_anchor_layer == 11 && dir == 1)
		{
			custom_anchor_layer = 19;
		}
		else
		{
			custom_anchor_layer = mod(custom_anchor_layer + mouse.scroll, 20);
		}
	}
	
	private void shift_props(const float dx, const float dy)
	{
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].move(dx, dy);
		}
		
		selection_x += dx;
		selection_y += dy;
	}
	
	private void flip_props(const bool x, const bool y)
	{
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		
		if(x)
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				PropData@ data = @selected_props[i];
				data.start_scale(anchor_x, anchor_y);
				data.do_scale(-1, 1);
				data.stop_scale(false);
			}
		}
		else
		{
			for(int i = 0; i < selected_props_count; i++)
			{
				PropData@ data = @selected_props[i];
				data.start_scale(anchor_x, anchor_y);
				data.do_scale(1, -1);
				data.stop_scale(false);
			}
		}
		
		selection_angle = 0;
		update_selection_bounds(true, selection_x, selection_y);
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
				PropData@ selected_prop_data = @selected_props[--selected_props_count];
				selected_prop_data.selected = false;
			}
			
			selection_layer = 0;
			selection_angle = 0;
			clear_custom_anchor();
		}
		
		if(@prop_data == null)
		{
			toolbar.update_buttons(selected_props_count);
			return;
		}
		
		if(
			action == SelectAction::Remove && !prop_data.selected ||
			action == SelectAction::Add && prop_data.selected
		)
			return;
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(selected_props_count >= selected_props_size)
			{
				selected_props.resize(selected_props_size = selected_props_count + 32);
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
			selected_props.resize(selected_props_size);
			prop_data.selected = false;
			selected_props_count--;
			
			if(int(prop_data.prop.layer()) >= selection_layer)
			{
				update_selection_layer();
			}
		}
		
		selection_angle = 0;
		update_selection_bounds();
		
		toolbar.update_buttons(selected_props_count);
	}
	
	private void select_none()
	{
		select_prop(null, SelectAction::Set);
	}
	
	private void update_selection_bounds(const bool set_origin=false, const float origin_x=0, const float origin_y=0)
	{
		if(selected_props_count == 0)
			return;
		
		PropData@ prop_data = @selected_props[0];
		selection_x1 = prop_data.x + prop_data.local_x1;
		selection_y1 = prop_data.y + prop_data.local_y1;
		selection_x2 = prop_data.x + prop_data.local_x2;
		selection_y2 = prop_data.y + prop_data.local_y2;
		
		for(int i = selected_props_count - 1; i >= 1; i--)
		{
			@prop_data = @selected_props[i];
			
			if(prop_data.x + prop_data.local_x1 < selection_x1) selection_x1 = prop_data.x + prop_data.local_x1;
			if(prop_data.y + prop_data.local_y1 < selection_y1) selection_y1 = prop_data.y + prop_data.local_y1;
			if(prop_data.x + prop_data.local_x2 > selection_x2) selection_x2 = prop_data.x + prop_data.local_x2;
			if(prop_data.y + prop_data.local_y2 > selection_y2) selection_y2 = prop_data.y + prop_data.local_y2;
		}
		
		if(set_origin)
		{
			selection_x = origin_x;
			selection_y = origin_y;
		}
		else
		{
			selection_x = selected_props_count > 1 ? selection_x1 + (selection_x2 - selection_x1) * origin_align_x : prop_data.anchor_x;
			selection_y = selected_props_count > 1 ? selection_y1 + (selection_y2 - selection_y1) * origin_align_y : prop_data.anchor_y;
		}
		
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
	
	private void copy_selected_props(const bool show_overlay=false)
	{
		PropsClipboardData@ props_clipboard = @script.props_clipboard;
		array<PropClipboardData>@ props = @props_clipboard.props;
		props.resize(selected_props_count);
		
		float ox = has_custom_anchor ? custom_anchor_x : selection_x;
		float oy = has_custom_anchor ? custom_anchor_y : selection_y;
		
		if(has_custom_anchor)
		{
			script.transform(ox, oy, custom_anchor_layer, selection_layer, ox, oy);
		}
		
		props_clipboard.x = ox;
		props_clipboard.y = oy;
		props_clipboard.layer = selection_layer;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			PropData@ prop_data = @selected_props[i];
			PropClipboardData@ copy_data = @props[i];
			
			copy_data.prop_set		= prop_data.prop.prop_set();
			copy_data.prop_group	= prop_data.prop.prop_group();
			copy_data.prop_index	= prop_data.prop.prop_index();
			copy_data.palette		= prop_data.prop.palette();
			copy_data.layer			= prop_data.prop.layer();
			copy_data.sub_layer		= prop_data.prop.sub_layer();
			copy_data.x				= prop_data.x - ox;
			copy_data.y				= prop_data.y - oy;
			copy_data.rotation		= prop_data.prop.rotation();
			copy_data.scale_x		= prop_data.prop.scale_x();
			copy_data.scale_y		= prop_data.prop.scale_y();
			
			float x1 = prop_data.x + prop_data.local_x1 - ox;
			float y1 = prop_data.y + prop_data.local_y1 - oy;
			float x2 = prop_data.x + prop_data.local_x2 - ox;
			float y2 = prop_data.y + prop_data.local_y2 - oy;
			
			script.transform(x1, y1, copy_data.layer, selection_layer, x1, y1);
			script.transform(x2, y2, copy_data.layer, selection_layer, x2, y2);
			
			if(i == 0)
			{
				props_clipboard.x1 = x1;
				props_clipboard.y1 = y1;
				props_clipboard.x2 = x2;
				props_clipboard.y2 = y2;
			}
			else
			{
				if(x1 < props_clipboard.x1) props_clipboard.x1 = x1;
				if(y1 < props_clipboard.y1) props_clipboard.y1 = y1;
				if(x2 > props_clipboard.x2) props_clipboard.x2 = x2;
				if(y2 > props_clipboard.y2) props_clipboard.y2 = y2;
			}
		}
		
		if(show_overlay)
		{
			script.info_overlay.show(
				selection_x + selection_x1, selection_y + selection_y1,
				selection_x + selection_x2, selection_y + selection_y2,
				selected_props_count + ' prop' + (selected_props_count != 1 ? 's' : '') + ' copied', 0.75);
		}
	}
	
	private void paste(const bool into_place=false, const bool tile_aligned=false)
	{
		PropsClipboardData@ props_clipboard = @script.props_clipboard;
		array<PropClipboardData>@ props = @props_clipboard.props;
		const int count = int(props.length());
		
		if(count == 0)
			return;
		
		float x, y;
		
		if(into_place)
		{
			x = props_clipboard.x;
			y = props_clipboard.y;
		}
		else
		{
			float mx, my;
			script.transform(mouse.x, mouse.y, 22, props_clipboard.layer, mx, my);
			x = mx - props_clipboard.x1 - (props_clipboard.x2 - props_clipboard.x1) * 0.5;
			y = my - props_clipboard.y1 - (props_clipboard.y2 - props_clipboard.y1) * 0.5;
			
			if(tile_aligned)
			{
				x = floor(x / 48) * 48 + (props_clipboard.x - floor(props_clipboard.x / 48) * 48);
				y = floor(y / 48) * 48 + (props_clipboard.y - floor(props_clipboard.y / 48) * 48);
			}
		}
		
		select_none();
		origin_align_x = -props_clipboard.x1 / (props_clipboard.x2 - props_clipboard.x1);
		origin_align_y = -props_clipboard.y1 / (props_clipboard.y2 - props_clipboard.y1);
		
		for(int i = 0; i < count; i++)
		{
			PropClipboardData@ copy_data = @props[i];
			prop@ p = create_prop();
			p.prop_set       (copy_data.prop_set);
			p.prop_group     (copy_data.prop_group);
			p.prop_index     (copy_data.prop_index);
			p.palette        (copy_data.palette);
			p.layer          (copy_data.layer);
			p.sub_layer      (copy_data.sub_layer);
			p.x              (copy_data.x + x);
			p.y              (copy_data.y + y);
			p.rotation       (copy_data.rotation);
			p.scale_x        (copy_data.scale_x);
			p.scale_y        (copy_data.scale_y);
			
			const array<array<float>>@ outline = @PROP_OUTLINES[copy_data.prop_set - 1][copy_data.prop_group][copy_data.prop_index - 1];
			
			script.g.add_prop(p);
			
			PropData@ data = highlight_prop(p, @outline);
			select_prop(data, SelectAction::Add);
		}
		
		const float dx = selection_x1 - props_clipboard.x1;
		const float dy = selection_y1 - props_clipboard.y1;
		
		selection_x += dx;
		selection_y += dy;
		selection_x1 -= dx;
		selection_y1 -= dy;
		selection_x2 -= dx;
		selection_y2 -= dy;
		
		update_alignments_from_origin();
	}
	
	// Highligts
	
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
		
		prop_data.init(script, this, prop, @outline);
		
		if(highlighted_props_count >= highlighted_props_size)
		{
			highlighted_props.resize(highlighted_props_size += 32);
		}
		
		@highlighted_props[highlighted_props_count++] = @prop_data;
		@highlighted_props_map[key] = @prop_data;
		
		return prop_data;
	}
	
	private void clear_highlighted_props(const bool clear_pending=false)
	{
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			PropData@ prop_data = @highlighted_props[i];
			
			if(clear_pending)
			{
				prop_data.pending_selection = 0;
				
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
			const float rotation = p.rotation() * DEG2RAD * (p.scale_x() >= 0 ? 1.0 : -1.0) * (p.scale_y() >= 0 ? 1.0 : -1.0);
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
				data.selected = @prop_data != null ? prop_data.selected : false;
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
	
	private void clear_custom_anchor()
	{
		if(custom_anchor_lock.value)
			return;
		
		toolbar.update_buttons(selected_props_count);
		has_custom_anchor = false;
	}
	
	// //////////////////////////////////////////////////////////
	// Other
	// //////////////////////////////////////////////////////////
	
	void update_alignments_from_origin(const bool force_selection_update=false)
	{
		if(default_origin.value == 'top_left')
		{
			origin_align_x = 0;
			origin_align_y = 0;
		}
		else if(default_origin.value == 'top')
		{
			origin_align_x = 0.5;
			origin_align_y = 0;
		}
		else if(default_origin.value == 'top_right')
		{
			origin_align_x = 1;
			origin_align_y = 0;
		}
		else if(default_origin.value == 'right')
		{
			origin_align_x = 1;
			origin_align_y = 0.5;
		}
		else if(default_origin.value == 'bottom_right')
		{
			origin_align_x = 1;
			origin_align_y = 1;
		}
		else if(default_origin.value == 'bottom')
		{
			origin_align_x = 0.5;
			origin_align_y = 1;
		}
		else if(default_origin.value == 'bottom_left')
		{
			origin_align_x = 0;
			origin_align_y = 1;
		}
		else if(default_origin.value == 'left')
		{
			origin_align_x = 0;
			origin_align_y = 0.5;
		}
		else
		{
			origin_align_x = 0.5;
			origin_align_y = 0.5;
		}
		
		if(force_selection_update)
		{
			selection_angle = 0;
			update_selection_bounds();
		}
	}
	
	void snap_custom_anchor()
	{
		if(has_custom_anchor)
		{
			script.snap(custom_anchor_x, custom_anchor_y, custom_anchor_x, custom_anchor_y, custom_grid.value, true);
		}
		else if(selected_props_count > 0)
		{
			const float x = selection_x;
			const float y = selection_y;
			script.snap(selection_x, selection_y, selection_x, selection_y, custom_grid.value, true);
			const float dx = selection_x - x;
			const float dy = selection_y - y;
			selection_x1 -= dx;
			selection_y1 -= dy;
			selection_x2 -= dx;
			selection_y2 -= dy;
		}
	}
	
	void align(const AlignmentEdge align)
	{
		if(selected_props_count < 2)
			return;
		
		const bool horizontal = align == Left || align == Centre || align == Right;
		const int dir = align == Left || align == Top ? -1 : align == Right || align == Bottom ? 1 : 0;
		
		PropData@ p = @selected_props[0];
		float min = horizontal ? (p.x + p.local_x1) : (p.y + p.local_y1);
		float max = horizontal ? (p.x + p.local_x2) : (p.y + p.local_y2);
		
		for(int i = selected_props_count - 1; i >= 0; i--)
		{
			@p = @selected_props[i];
			
			if(horizontal)
			{
				if(p.x + p.local_x1 < min) min = p.x + p.local_x1;
				if(p.x + p.local_x2 > max) max = p.x + p.local_x2;
			}
			else
			{
				if(p.y + p.local_y1 < min) min = p.y + p.local_y1;
				if(p.y + p.local_y2 > max) max = p.y + p.local_y2;
			}
		}
		
		const float pos = dir == -1
			? min
			: dir == 1 ? max
				: (min + max) * 0.5;
		
		for(int i = selected_props_count - 1; i >= 0; i--)
		{
			@p = @selected_props[i];
			
			if(horizontal)
			{
				if(dir == -1)
					p.x = pos - p.local_x1;
				else if(dir == 1)
					p.x = pos - p.local_x2;
				else
					p.x = pos - (p.local_x1 + p.local_x2) * 0.5;
				
				p.prop.x(p.x);
			}
			else
			{
				if(dir == -1)
					p.y = pos - p.local_y1;
				else if(dir == 1)
					p.y = pos - p.local_y2;
				else
					p.y = pos - (p.local_y1 + p.local_y2) * 0.5;
				
				p.prop.y(p.y);
			}
		}
		
		selection_angle = 0;
		update_selection_bounds();
	}
	
	void distribute(const AlignmentEdge align)
	{
		if(selected_props_count < 3)
			return;
		
		const bool horizontal = align == Left || align == Centre || align == Right || align == Horizontal;
		
		if(props_align_data_size < selected_props_count)
		{
			props_align_data.resize(props_align_data_size += selected_props_count + 32);
		}
		
		float props_width = 0;
		
		for(int i = selected_props_count - 1; i >= 0; i--)
		{
			PropData@ p = @selected_props[i];
			@props_align_data[i].data = p;
			props_align_data[i].x = horizontal ? p.x + p.local_x1 : p.y + p.local_y1;
			props_width += horizontal ? p.local_x2 - p.local_x1 : p.local_y2 - p.local_y1;
		}
		
		props_align_data.sortAsc(0, selected_props_count);
		
		PropData@ first = @props_align_data[0].data;
		PropData@ last  = @props_align_data[selected_props_count - 1].data;
		float min, max;
		
		switch(align)
		{
			case AlignmentEdge::Left:
			case AlignmentEdge::Top:
				min = horizontal ? first.x + first.local_x1 : first.y + first.local_y1;
				max = horizontal ? last.x + last.local_x1 : last.y + last.local_y1;
				break;
			case AlignmentEdge::Right:
			case AlignmentEdge::Bottom:
				min = horizontal ? first.x + first.local_x2 : first.y + first.local_y2;
				max = horizontal ? last.x + last.local_x2 : last.y + last.local_y2;
				break;
			case AlignmentEdge::Centre:
			case AlignmentEdge::Middle:
				min = horizontal ? first.x + (first.local_x1 + first.local_x2) * 0.5 : first.y + (first.local_y1 + first.local_y2) * 0.5;
				max = horizontal ? last.x + (last.local_x1 + last.local_x2) * 0.5 : last.y + (last.local_y1 + last.local_y2) * 0.5;
				break;
			case AlignmentEdge::Horizontal:
			case AlignmentEdge::Vertical:
				min = horizontal ? (first.x + first.local_x1) : (first.y + first.local_y1);
				max = horizontal ? (last.x  + last.local_x2)  : (last.y + last.local_y2);
				break;
		}
		
		const bool is_spaced = align == Horizontal || align == Vertical;
		const float spacing = (is_spaced
			? (max - min) - props_width
			: (max - min)
		) / (selected_props_count - 1);
		
		float x = min + spacing;
		
		if(is_spaced)
		{
			x += horizontal ? (first.local_x2 - first.local_x1) : (first.local_y2 - first.local_y1);
		}
		
		for(int i = 1; i < selected_props_count - 1; i++)
		{
			PropData@ p = @props_align_data[i].data;
			
			switch(align)
			{
				case AlignmentEdge::Horizontal:
				case AlignmentEdge::Left:		p.x = x - p.local_x1; break;
				
				case AlignmentEdge::Vertical:
				case AlignmentEdge::Top:		p.y = x - p.local_y1; break;
				
				case AlignmentEdge::Right:		p.x = x - p.local_x2; break;
				case AlignmentEdge::Bottom:		p.y = x - p.local_y2; break;
				case AlignmentEdge::Centre:		p.x = x - (p.local_x1 + p.local_x2) * 0.5; break;
				case AlignmentEdge::Middle:		p.y = x - (p.local_y1 + p.local_y2) * 0.5; break;
			}
			
			p.prop.x(p.x);
			p.prop.y(p.y);
			
			if(is_spaced)
			{
				x += horizontal ? p.local_x2 - p.local_x1 : p.local_y2 - p.local_y1;
			}
			
			x += spacing;
		}
		
		selection_angle = 0;
		update_selection_bounds();
	}
	
	bool is_custom_anchor_active()
	{
		return has_custom_anchor;
	}
	
	void export_selected_props(const PropExportType type)	
	{
		if(selected_props_count == 0)
			return;
		
		const float origin_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float origin_y = has_custom_anchor ? custom_anchor_y : selection_y;
		
		switch(type)
		{
			case PropExportType::SpriteBatch:
				PropToolExporter::sprite_batch(@selected_props, selected_props_count, origin_x, origin_y);
				break;
			case PropExportType::SpriteGroup:
				PropToolExporter::sprite_group(@selected_props, selected_props_count, origin_x, origin_y);
				break;
		}
	}
	
	void correct_prop_values()
	{
		const float anchor_x = has_custom_anchor ? custom_anchor_x : selection_x;
		const float anchor_y = has_custom_anchor ? custom_anchor_y : selection_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			PropData@ data = @selected_props[i];
			prop@ p = data.prop;
			
			data.anchor_world(anchor_x, anchor_y);
			data.set_prop_rotation(round(p.rotation()));
			data.update();
			data.init_anchors();
			
			data.start_scale(anchor_x, anchor_y);
//			puts(
//				p.scale_y(),
//				get_valid_prop_scale(abs(p.scale_y())),
//				get_valid_prop_scale(abs(p.scale_y())) / abs(p.scale_y()) );
			data.do_scale(
				get_valid_prop_scale(abs(p.scale_x())) / abs(p.scale_x()),
				get_valid_prop_scale(abs(p.scale_y())) / abs(p.scale_y())
			);
			data.stop_scale(false);
			
			p.x(round(p.x()));
			p.y(round(p.y()));
			
			data.update();
		}
		
		selection_angle = 0;
		update_selection_bounds();
	}
	
}
