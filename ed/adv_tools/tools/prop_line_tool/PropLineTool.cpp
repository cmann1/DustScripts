#include '../../../../lib/drawing/Sprite.cpp';
#include '../../../../lib/props/common.cpp';
#include '../../../../lib/string.cpp';

#include 'PropLineProp.cpp';
#include 'PropLineRotationMode.cpp';
#include 'PropLineRotationOffsets.cpp';
#include 'PropLineToolState.cpp';

const string EMBED_spr_icon_prop_line_tool = SPRITES_BASE + 'icon_prop_line_tool.png';

class PropLineTool : Tool
{
	
	private PropLineToolState state = Idle;
	private Mouse@ mouse;
	private PropTool@ prop_tool;
	private array<PropData@>@ highlighted_props;
	private PropData@ pick_data;
	
	private int prop_set = -1;
	private int prop_group;
	private int prop_index;
	private int prop_palette;
	private float x1, y1;
	private float x2, y2;
	private int scale_index = 0;
	private float scale = 1.0;
	private float scale_x, scale_y;
	private float rotation;
	private int layer, sub_layer;
	
	private Sprite spr;
	private string sprite_set, sprite_name;
	
	private bool recaclulate_props;
	/** A per prop value used to inset certain props, e.g. moon and sun */
	private float auto_spacing_adjustment;
	private float start_dx, start_dy;
	private float drag_ox, drag_oy;
	private DragHandleType drag_handle = DragHandleType::None;
	
	private int props_list_size = 32;
	private int props_count;
	private array<PropLineProp> props(props_list_size);
	
	// Settings
	
	/** If false, will snap to angle instead. */
	bool snap_to_grid = false;
	/** If false, scroll wheel adjusts rotation */
	bool scroll_spacing = true;
	PropLineSpacingMode spacing_mode = PropLineSpacingMode::Fixed;
	PropLineRotationMode rotation_mode = PropLineRotationMode::Auto;
	float spacing = 50;
	float spacing_offset;
	float rotation_offset;
	bool has_auto_rotation_offset;
	bool auto_space = true;
	
	PropLineTool(AdvToolScript@ script)
	{
		super(script, 'Props', 'Prop Line Tool');
		
		init_shortcut_key(VK::Q, ModifierKey::Alt);
	}
	
	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_prop_line_tool');
	}
	
	void create(ToolGroup@ group) override
	{
		Tool::create(group);
		
		set_icon(SPRITE_SET, 'icon_prop_line_tool');
		
		@mouse = @script.mouse;
	}
	
	void on_init() override
	{
		@prop_tool = cast<PropTool@>(script.get_tool('Prop Tool'));
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	void on_editor_unloaded() override
	{
		reset();
		Tool::on_editor_unloaded();
	}
	
	protected void on_select_impl()
	{
		script.editor.hide_panels_gui(true);
	}
	
	protected void on_deselect_impl()
	{
		script.hide_info_popup();
		script.editor.hide_panels_gui(false);
		
		reset();
	}
	
	protected void step_impl() override
	{
		if(prop_set == -1 || state == PropLineToolState::Picking)
		{
			show_pick_popup();
		}
		
		switch(state)
		{
			case PropLineToolState::Idle: state_idle(); break;
			case PropLineToolState::Picking: state_picking(); break;
			case PropLineToolState::Dragging: state_dragging(); break;
			case PropLineToolState::Pending: state_pending(); break;
		}
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		if(state == PropToolState::Idle || state == PropToolState::Picking)
		{
			if(prop_set != -1)
			{
				spr.draw(
					script.layer, script.sub_layer,
					0, prop_palette, x1, y1,
					rotation + (rotation_mode == PropLineRotationMode::Auto ? rotation_offset : 0.0),
					scale_x * scale, scale_y * scale,
					state == PropToolState::Picking ? 0x77ffffff : 0xffffffff,
					calc_bg_scale(script.layer));
			}
		}
		
		if(state == PropToolState::Picking)
		{
			if(@pick_data != null)
			{
				pick_data.draw(PropToolHighlight::Both);
			}
		}
		
		if(state >= PropToolState::Dragging)
		{
			for(int i = 0; i < props_count; i++)
			{
				PropLineProp@ p = @props[i];
				spr.draw(
					layer, sub_layer,
					0, prop_palette, p.x, p.y, p.rotation, p.scale_x, p.scale_y,
					0xffffffff, calc_bg_scale(layer));
			}
			
			const uint clr = 0x77ffffff;
			float x3, y3, x4, y4;
			script.transform(x1, y1, script.layer, 22, x3, y3);
			script.transform(x2, y2, script.layer, 22, x4, y4);
			draw_line(script.g, 22, 22, x3, y3, x4, y4, 1 / script.zoom, clr);
		}
	}
	
	private void user_update_layer()
	{
		if(script.shift.down || script.ctrl.down && script.alt.down)
			return;
		
		script.scroll_layer(true, true, false, LayerInfoDisplay::Compound, null, script.main_toolbar, 0.75);
		layer = script.layer;
		sub_layer = script.sub_layer;
		update_start_point();
	}
	
	private void user_update_rotation()
	{
		if(mouse.scroll == 0 || (!script.shift.down && (script.ctrl.down || script.alt.down)))
			return;
		
		float dir = sign(mouse.scroll);
		
		if(script.shift.down && script.ctrl.down)
			dir *= 1;
		else if(script.shift.down)
			dir *= 5;
		else
			dir *= 10;
		
		const float rotation_prev = rotation;
		rotation = repeat(rotation - dir, 360);
		
		if(rotation != rotation_prev)
		{
			recaclulate_props = true;
		}
	}
	
	private void user_update_scale()
	{
		if(mouse.scroll == 0 || !script.ctrl.down || !script.alt.down)
			return;
		
		const int prev_scale_index = scale_index;
		
		scale_index = int(min(24, max(-24, scale_index + mouse.scroll)));
		scale = pow(50.0, scale_index / 24.0);
		
		if(scale_index != prev_scale_index)
		{
			script.show_info('Scale: ' + string::nice_float(scale, 3));
		}
	}
	
	private void user_update_mirror()
	{
		if(!script.scene_focus)
			return;
		
		bool updated = false;
		
		if(script.input.key_check_pressed_gvb(GVB::BracketOpen))
		{
			scale_x = -scale_x;
			updated = true;
		}
		
		if(script.input.key_check_pressed_gvb(GVB::BracketClose))
		{
			scale_y = -scale_y;
			updated = true;
		}
		
		if(updated)
		{
			recaclulate_props = true;
		}
	}
	
	private void user_update_snap()
	{
		if(script.shift.double_press)
		{
			snap_to_grid = !snap_to_grid;
			script.show_info('Snap: ' + (snap_to_grid ? 'Grid' : 'Angle'));
		}
	}
	
	private void user_update_scroll_mode()
	{
		if(!script.alt.double_press)
			return;
		
		scroll_spacing = !scroll_spacing;
		script.show_info('Scroll: ' + (scroll_spacing ? 'Spacing' : 'Rotation'));
	}
	
	private void user_update_spacing()
	{
		if(mouse.scroll == 0)
			return;
		
		if(scroll_spacing)
		{
			float amount = 2;
			
			if(script.shift.down)
				amount = 20;
			else if(script.ctrl.down)
				amount = 5;
			else if(script.alt.down)
				amount = 1;
			
			float spacing_display;
			
			if(auto_space)
			{
				const float spacing_offset_prev = spacing_offset;
				spacing_offset = spacing_offset - mouse.scroll * amount;
				recaclulate_props = recaclulate_props || spacing_offset != spacing_offset_prev;
				
				spacing_display = spacing_offset;
			}
			else
			{
				const float spacing_prev = spacing;
				spacing = max(spacing - mouse.scroll * amount, 0.0);
				recaclulate_props = recaclulate_props || spacing != spacing_prev;
				
				spacing_display = spacing;
			}
			
			script.show_info('Spacing: ' + int(spacing_display));
		}
		else if(rotation_mode == PropLineRotationMode::Auto)
		{
			float dir = sign(mouse.scroll);
			
			if(script.shift.down && script.ctrl.down)
				dir *= 1;
			else if(script.shift.down)
				dir *= 5;
			else
				dir *= 10;
			
			const float rotation_offset_prev = rotation_offset;
			rotation_offset = normalize_degress(rotation_offset - dir);
			has_auto_rotation_offset = false;
			
			if(rotation_offset != rotation_offset_prev)
			{
				if(auto_space)
				{
					calculate_auto_spacing();
				}
				
				recaclulate_props = true;
			}
			
			script.show_info('Rotation Offset: ' + int(rotation_offset));
		}
		else
		{
			user_update_rotation();
		}
	}
	
	private void calculate_auto_spacing()
	{
		if(prop_set == -1)
			return;
		
		const float bg_scale = calc_bg_scale(layer);
		
		const float length = distance(x1, y1, x2, y2);
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float nx = length > 0 ? dx / length : 1.0;
		const float ny = length > 0 ? dy / length : 0.0;
		const float angle = atan2(dy, dx);
		
		// Caclulate the line in local prop space
		// --------------------------------------
		
		const float prop_angle = rotation_mode == PropLineRotationMode::Auto
			? rotation_offset * DEG2RAD
			: rotation * DEG2RAD;
		
		const float ray_size = sqrt(
			spr.sprite_width * spr.sprite_width * 0.25 +
			spr.sprite_height * spr.sprite_height * 0.25) * 1.5;
		float lox = 0;
		float loy = 0;
		float l1x = -ray_size;
		float l1y = 0;
		float l2x = ray_size;
		float l2y = 0;
		
		if(
			rotation_mode != PropLineRotationMode::Auto &&
			(state == PropLineToolState::Dragging || state == PropLineToolState::Pending))
		{
			rotate(l1x, l1y, angle, l1x, l1y);
			rotate(l2x, l2y, angle, l2x, l2y);
		}
		
		rotate(l1x, l1y, -prop_angle, l1x, l1y);
		rotate(l2x, l2y, -prop_angle, l2x, l2y);
		
		lox *= scale_x; loy *= scale_y;
		l1x *= scale_x; l1y *= scale_y;
		l2x *= scale_x; l2y *= scale_y;
		
		float prop_ox = spr.sprite_offset_x - spr.sprite_width * spr.origin_x;
		float prop_oy = spr.sprite_offset_y - spr.sprite_height * spr.origin_y;
		
		lox -= prop_ox; loy -= prop_oy;
		l1x -= prop_ox; l1y -= prop_oy;
		l2x -= prop_ox; l2y -= prop_oy;
		
		// --------------------------------------
		
		float t1_max = -1;
		float t2_max = -1;
		
		const array<array<float>>@ outline = @PROP_OUTLINES[prop_set - 1][prop_group][prop_index - 1];
		for(int i = 0, path_count = int(outline.length()); i < path_count; i++)
		{
			const array<float>@ path = @outline[i];
			const int count = int(path.length());
			float p1x = path[count - 2];
			float p1y = path[count - 1];
			
			for(int j = 0; j < count; j += 2)
			{
				float p2x = path[j];
				float p2y = path[j + 1];
				
				float ix, iy, t;
				if(line_line_intersection(lox, loy, l1x, l1y, p1x, p1y, p2x, p2y, ix, iy, t) && t > t1_max)
				{
					t1_max = t;
				}
				if(line_line_intersection(lox, loy, l2x, l2y, p1x, p1y, p2x, p2y, ix, iy, t) && t > t2_max)
				{
					t2_max = t;
				}
				
				p1x = p2x;
				p1y = p2y;
			}
		}
		
		if(t1_max == -1 || t2_max == -1)
			return;
		
		const float spacing_prev = spacing;
		spacing = max(ray_size * t1_max + ray_size * t2_max, 0.0) * scale;
		recaclulate_props = recaclulate_props || spacing != spacing_prev;
			
		//script.show_info('Spacing: ' + int(spacing));
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		update_start_point();
		layer = script.layer;
		sub_layer = script.sub_layer;
		
		if(mouse.left_press && prop_set != -1)
		{
			update_end_point();
			calculate_props();
			state = PropLineToolState::Dragging;
			active = true;
			return;
		}
		
		if(mouse.right_press)
		{
			show_pick_popup();
			state = PropLineToolState::Picking;
			return;
		}
		
		user_update_scroll_mode();
		user_update_layer();
		user_update_rotation();
		user_update_scale();
		user_update_mirror();
	}
	
	private void state_picking()
	{
		update_start_point();
		
		prop_tool.clear_hovered_props();
		@pick_data = null;
		
		if(script.mouse_in_scene && !script.space.down && !script.handles.mouse_over)
		{
			prop_tool.pick_props();
		}
		
		prop_tool.clear_highlighted_props();
		
		if(prop_tool.highlighted_props_list_count > 0)
		{
			prop@ prev_prop = @pick_data != null ? pick_data.prop : null;
			@pick_data = @prop_tool.highlighted_props_list[0];
			prop_set = pick_data.prop.prop_set();
			prop_group = pick_data.prop.prop_group();
			prop_index = pick_data.prop.prop_index();
			prop_palette = pick_data.prop.palette();
			scale_x = sign(pick_data.prop.scale_x());
			scale_y = sign(pick_data.prop.scale_y());
			scale_index = round_int(log(pick_data.prop.scale_x()) / log(50.0) * 24.0);
			scale = pow(50.0, scale_index / 24.0);
			rotation = pick_data.prop.rotation();
			update_sprite();
			
			const string key = prop_set + '.' + prop_group + '.' + prop_index;
			
			auto_spacing_adjustment = PropLineAutoOffsets.exists(key)
				? float(PropLineAutoOffsets[key])
				: 0.0;
			
			if(rotation_mode == PropLineRotationMode::Auto)
			{
				if(PropLineRotationOffsets.exists(key))
				{
					rotation_offset = float(PropLineRotationOffsets[key]);
					has_auto_rotation_offset = true;
				}
				else if(has_auto_rotation_offset)
				{
					rotation_offset = 0.0;
					has_auto_rotation_offset = false;
				}
			}
		}
		
		if(prop_set != -1 && !mouse.right_down)
		{
			if(auto_space)
			{
				calculate_auto_spacing();
			}
			
			state = PropLineToolState::Idle;
			script.hide_info_popup();
		}
	}
	
	private void state_dragging()
	{
		user_update_snap();
		user_update_scroll_mode();
		user_update_spacing();
		user_update_mirror();
		update_end_point(!mouse.right_down);
		
		if(mouse.right_press)
		{
			start_dx = x1 - x2;
			start_dy = y1 - y2;
		}
		else if(mouse.right_down)
		{
			x1 = x2 + start_dx;
			y1 = y2 + start_dy;
			recaclulate_props = true;
		}
		
		if(recaclulate_props)
		{
			if(auto_space)
			{
				calculate_auto_spacing();
			}
			
			calculate_props();
		}
		
		if(script.input.key_check_pressed_gvb(GVB::Return))
		{
			script.show_info_popup(
				'Enter : Place props\n' + 
				'Escape: Cancel',
				null , PopupPosition::Below);
			state = PropLineToolState::Pending;
			active = false;
			return;
		}
		
		if(!mouse.left_down || script.escape_press)
		{
			if(!script.escape_press)
			{
				place_props();
			}
			
			state = PropLineToolState::Idle;
			update_start_point();
			active = false;
			return;
		}
	}
	
	private void state_pending()
	{
		user_update_snap();
		user_update_scroll_mode();
		user_update_spacing();
		user_update_mirror();
		
		const bool drag_p1 = script.handles.circle(x1, y1, Settings::RotateHandleSize,
			Settings::RotateHandleColour, Settings::RotateHandleHoveredColour);
		const bool drag_p2 = script.handles.circle(x2, y2, Settings::RotateHandleSize,
			Settings::RotateHandleColour, Settings::RotateHandleHoveredColour);
		const bool drag_line = script.handles.line(x1, y1, x2, y2, Settings::RotateHandleSize * 0.5,
			Settings::RotateHandleColour, Settings::RotateHandleHoveredColour);
		
		if(drag_handle == DragHandleType::None)
		{
			if(drag_p1 || drag_p2 || drag_line)
			{
				float mx, my;
				script.transform(mouse.x, mouse.y, 22, layer, mx, my);
				drag_handle = drag_p1 ? DragHandleType::Start : drag_p2 ? DragHandleType::End : DragHandleType::Segment;
				drag_ox = (drag_p1 || drag_line ? x1 : x2) - mx;
				drag_oy = (drag_p1 || drag_line ? y1 : y2) - my;
				
				start_dx = x1 - x2;
				start_dy = y1 - y2;
			}
		}
		else if(mouse.left_down)
		{
			if(drag_handle == DragHandleType::Start)
			{
				update_start_point(true, drag_ox, drag_oy);
			}
			else if(drag_handle == DragHandleType::End)
			{
				update_end_point(true, drag_ox, drag_oy);
			}
			else
			{
				update_start_point(true, drag_ox, drag_oy);
				x2 = x1 - start_dx;
				y2 = y1 - start_dy;
			}
		}
		else
		{
			drag_handle = DragHandleType::None;
		}
		
		if(recaclulate_props)
		{
			calculate_props();
		}
		
		if(script.input.key_check_pressed_gvb(GVB::Return) || script.escape_press)
		{
			if(!script.escape_press)
			{
				place_props();
			}
			
			state = PropLineToolState::Idle;
			update_start_point();
			active = false;
			script.hide_info_popup();
			return;
		}
	}
	
	private void show_pick_popup()
	{
		script.show_info_popup(
			prop_set == -1 ? 'Hold right mouse to select a prop' : 'Release to select a prop',
			null , PopupPosition::Below);
	}
	
	private void update_start_point(const bool allow_angle_snap=false, const float ox=0, const float oy=0)
	{
		const float x1_prev = x1;
		const float y1_prev = y1;
		
		script.transform(mouse.x + ox, mouse.y + oy, 22, layer, x1, y1);
		
		if(snap_to_grid || !allow_angle_snap)
		{
			script.snap(x1, y1, x1, y1);
		}
		else
		{
			snap_angle(x2, y2, x1, y1, x1, y1);
		}
		
		recaclulate_props = recaclulate_props || (x1 != x1_prev || y1 != y1_prev);
	}
	
	private void update_end_point(const bool allow_angle_snap=true, const float ox=0, const float oy=0)
	{
		const float x2_prev = x2;
		const float y2_prev = y2;
		
		script.transform(mouse.x + ox, mouse.y + oy, 22, layer, x2, y2);
		
		if(snap_to_grid || !allow_angle_snap)
		{
			script.snap(x2, y2, x2, y2);
		}
		else
		{
			snap_angle(x1, y1, x2, y2, x2, y2);
		}
		
		recaclulate_props = recaclulate_props || (x2 != x2_prev || y2 != y2_prev);
	}
	
	private void snap_angle(const float x1, const float y1, const float x2, const float y2, float &out ox, float &out oy)
	{
		float angle = atan2(y2 - y1, x2 - x1);
		float snapped_angle;
		script.snap(angle, snapped_angle);
		
		if(angle != snapped_angle)
		{
			const float length = distance(x1, y1, x2, y2);
			ox = x1 + cos(snapped_angle) * length;
			oy = y1 + sin(snapped_angle) * length;
		}
		else
		{
			ox = x2;
			oy = y2;
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Other
	// //////////////////////////////////////////////////////////
	
	private void reset()
	{
		state = PropLineToolState::Idle;
	}
	
	private void update_sprite()
	{
		sprite_from_prop(prop_set, prop_group, prop_index, sprite_set, sprite_name);
		spr.set(sprite_set, sprite_name);
	}
	
	private void calculate_props()
	{
		const float length = distance(x1, y1, x2, y2);
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float nx = length > 0 ? dx / length : 1.0;
		const float ny = length > 0 ? dy / length : 0.0;
		const float angle = atan2(dy, dx);
		
		float x = x1;
		float y = y1;
		
		float final_rotation;
		
		if(rotation_mode == PropLineRotationMode::Auto)
		{
			rotation = angle * RAD2DEG;
			final_rotation = rotation + rotation_offset;
		}
		else
		{
			final_rotation = rotation;
		}
		
		float spacing = this.spacing + spacing_offset;
		
		if(auto_space)
		{
			spacing += auto_spacing_adjustment;
		}
		
		spacing = max(layer <= 5 ? spacing * 16 : spacing, 0.1);
		props_count = ceil_int(max((length + spacing * 0.5) / spacing, 1.0));
		
		while(props_count > props_list_size)
		{
			props.resize(props_list_size *= 2);
		}
		
		if(spacing_mode == PropLineSpacingMode::Fill)
		{
			if(props_count > 1)
			{
				spacing = (length - spacing * 0.5) / (props_count - 1);
			}
		}
		
		for(int i = 0; i < props_count; i++)
		{
			PropLineProp@ p = @props[i];
			p.x = x;
			p.y = y;
			p.rotation = final_rotation;
			p.scale_x = scale_x * scale;
			p.scale_y = scale_y * scale;
			
			x += nx * spacing;
			y += ny * spacing;
		}
		
		recaclulate_props = false;
	}
	
	/// Must call `calculate_props` before calling this.
	private void place_props()
	{
		for(int i = 0; i < props_count; i++)
		{
			PropLineProp@ data = @props[i];
			float px, py;
			spr.real_position(data.x, data.y, data.rotation, px, py, data.scale_x, data.scale_y,
				calc_bg_scale(layer));
			prop@ p = create_prop(prop_set, prop_group, prop_index, px, py, layer, sub_layer, data.rotation);
			p.scale_x(data.scale_x);
			p.scale_y(data.scale_y);
			script.g.add_prop(p);
		}
	}
	
	private float calc_bg_scale(const int layer)
	{
		return layer <= 5 ? 2.0 / script.g.layer_scale(layer) : 1.0;
	}
	
}
