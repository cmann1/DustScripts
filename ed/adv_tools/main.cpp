#include '../../lib/std.cpp';
#include '../../lib/layer.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/enums/GVB.cpp';
#include '../../lib/enums/VK.cpp';
#include '../../lib/embed_utils.cpp';
#include '../../lib/drawing/common.cpp';
#include '../../lib/drawing/circle.cpp';
#include '../../lib/debug/Debug.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/input/Mouse.cpp';
#include '../../lib/math/Line.cpp';
#include '../../lib/utils/colour.cpp';
#include '../../lib/utils/copy_vars.cpp';
#include '../../lib/utils/print_vars.cpp';

#include '../../lib/ui3/UI.cpp';
#include '../../lib/ui3/elements/Toolbar.cpp';
#include '../../lib/ui3/popups/PopupOptions.cpp';
#include '../../lib/ui3/window_manager/WindowManager.cpp';

#include 'handles/Handles.cpp';
#include 'misc/DragHandleType.cpp';
#include 'misc/InfoOverlay.cpp';
#include 'misc/IWorldBoundingBox.cpp';
#include 'misc/ToolListenerInterfaces.cpp';
#include 'misc/WorldBoundingBox.cpp';
#include 'settings/Settings.cpp';
#include 'tools/emitter_tool/EmitterTool.cpp';
#include 'tools/prop_tool/PropTool.cpp';
#include 'tools/TextTool.cpp';
#include 'tools/ExtendedTriggerTool.cpp';
#include 'tools/HelpTool.cpp';
#include 'ToolGroup.cpp';

const string SCRIPT_BASE			= 'ed/adv_tools/';
const string SPRITES_BASE			= SCRIPT_BASE + 'sprites/';
const string EMBED_spr_icon_edit	= SPRITES_BASE + 'icon_edit.png';

const string SPRITE_SET = 'plugin';

class script : AdvToolScript {}

class AdvToolScript
{
	
	scene@ g;
	editor_api@ editor;
	camera@ cam;
	UI@ ui = UI(true);
	Mouse mouse(false);
	Handles handles;
	Debug debug();
	Line line;
	sprites@ editor_spr;
	sprites@ script_spr;
	
	float zoom;
	bool mouse_in_gui;
	bool mouse_in_scene;
	bool scene_focus;
	bool ctrl, shift, alt, space;
	bool return_press, escape_press;
	bool space_on_press;
	bool pressed_in_scene;
	bool shortcut_keys_enabled = true;
	
	InfoOverlay info_overlay;
	
	[text] bool debug_ui;
	[hidden] string clipboard;
	[hidden] WindowManager window_manager;
	[hidden] array<BoolSetting> bool_settings;
	[hidden] array<IntSetting> int_settings;
	[hidden] array<FloatSetting> float_settings;
	[hidden] array<StringSetting> string_settings;
	[hidden] PropsClipboardData props_clipboard;
	[hidden] string selected_tool_name;
	
	private dictionary settings;
	
	private bool initialised;
	private bool state_persisted = true;
	
	private Toolbar@ toolbar;
	private array<ToolGroup> tool_groups;
	private array<Tool@> tools;
	private array<Tool@> tools_shortcut;
	private int num_tools_shortcut;
	private dictionary tool_groups_map;
	private dictionary tools_map;
	private ButtonGroup@ button_group = ButtonGroup(ui, false);
	
	private PopupOptions@ shortcut_keys_enabled_popup;
	
	private string selected_tab;
	private Tool@ selected_tool;
	private int num_tool_group_popups;
	
	private EventCallback@ on_after_layout_delegate;
	
	private int pressed_key = -1;
	private bool pressed_key_active;
	private int pressed_timer;
	
	private bool ignore_toolbar_select_event;
	
	//
	
	float view_x, view_y;
	float view_x1, view_y1;
	float view_x2, view_y2;
	float view_w, view_h;
	float screen_w, screen_h;
	
	// //////////////////////////////////////////////////////////
	// Init
	// //////////////////////////////////////////////////////////
	
	AdvToolScript()
	{
		puts('>> Initialising AdvTools');
		@g = get_scene();
		@editor = get_editor_api();
		@cam = get_active_camera();
		
		if(@editor == null)
		{
			initialised = true;
			return;
		}
		
		editor.hide_gui(false);
		create_tools();
	}
  
	void editor_loaded()
	{
		if(@selected_tool != null)
		{
			selected_tool.on_editor_loaded();
		}
	}

	void editor_unloaded()
	{
		if(@selected_tool != null)
		{
			selected_tool.on_editor_unloaded();
		}
	}
	
	void build_sprites(message@ msg)
	{
		build_sprite(@msg, 'icon_edit', 0, 0);
		
		for(uint i = 0; i < tools.length(); i++)
		{
			tools[i].build_sprites(msg);
		}
	}
	
	private void initialise()
	{
		@editor_spr = create_sprites();
		editor_spr.add_sprite_set('editor');
		@script_spr = create_sprites();
		script_spr.add_sprite_set(SPRITE_SET);
		
		initialise_settings();
		initialise_ui();
		initialise_tools();
		
		select_tool(selected_tool_name != '' ? selected_tool_name : editor.editor_tab());
		
		info_overlay.init(this);
		handles.init(this);
	}
	
	private void initialise_settings()
	{
		for(uint i = 0; i < bool_settings.length(); i++)
		{
			@settings[bool_settings[i].key] = @bool_settings[i];
		}
		
		for(uint i = 0; i < int_settings.length(); i++)
		{
			@settings[int_settings[i].key] = @int_settings[i];
		}
		
		for(uint i = 0; i < float_settings.length(); i++)
		{
			@settings[float_settings[i].key] = @float_settings[i];
		}
		
		for(uint i = 0; i < string_settings.length(); i++)
		{
			@settings[string_settings[i].key] = @string_settings[i];
		}
	}
	
	private void initialise_ui()
	{
		@ui.debug = debug;
		ui.clipboard = clipboard;
		ui.clipboard_change.on(EventCallback(on_clipboard_change));
		ui.auto_fit_screen = true;
		
//		ui.style.text_clr                        = 0xffffffff;
//		ui.style.normal_bg_clr                   = 0xd9050505;
//		ui.style.normal_border_clr               = 0x33ffffff;
//		ui.style.highlight_bg_clr                = 0xd933307c;
//		ui.style.highlight_border_clr            = 0xd96663c2;
//		ui.style.selected_bg_clr                 = 0xd933307c;
//		ui.style.selected_border_clr             = 0xff7d7acb;
//		ui.style.selected_highlight_bg_clr       = 0xd9423fa0;
//		ui.style.selected_highlight_border_clr   = 0xff7d7acb;
//		ui.style.disabled_bg_clr                 = 0xa6000000;
//		ui.style.disabled_border_clr             = 0x26ffffff;
//		ui.style.secondary_bg_clr                = 0x667f7daf;
//		ui.style.scrollbar_light_bg_clr          = 0xd9111111;
		
		selected_tab = editor.editor_tab();
		
		window_manager.initialise(ui);
		
		@toolbar = Toolbar(ui, false, true);
		toolbar.name = 'ToolsToolbar';
		toolbar.x = 400;
		toolbar.y = 200;
		
		button_group.allow_reselect = true;
		button_group.select.on(EventCallback(on_tool_button_select));
		
		for(uint i = 0; i < tool_groups.length(); i++)
		{
			ToolGroup@ group = @tool_groups[i];
			group.init_ui();
			toolbar.add_child(group.button);
		}
		
		ui.add_child(toolbar);
		position_toolbar();
		
		@on_after_layout_delegate = EventCallback(on_after_layout);
		ui.after_layout.on(on_after_layout_delegate);
		
		ui.screen_resize.on(EventCallback(on_screen_resize));
		
		update_shortcut_keys_enabled_popup();
	}
	
	private void initialise_tools()
	{
		for(uint i = 0; i < tool_groups.length(); i++)
		{
			tool_groups[i].on_init();
		}
	}
	
	private void create_tools()
	{
		// Built in
		
		add_tool(Tool('Select')			.set_icon('editor',  'selecticon'));
		add_tool(Tool('Tiles')			.set_icon('editor',  'tilesicon'));
		add_tool(Tool('Props')			.set_icon('editor',  'propsicon').init_shortcut_key(VK::Q, false));
		add_tool(Tool('Entities')		.set_icon('editor',  'entityicon').init_shortcut_key(VK::E));
		add_tool(Tool('Triggers')		.set_icon('editor',  'triggersicon').init_shortcut_key(VK::T));
		add_tool(Tool('Camera')			.set_icon('editor',  'cameraicon'));
		add_tool(EmitterTool());
		add_tool(Tool('Level Settings')	.set_icon('editor',  'settingsicon'));
		add_tool(Tool('Scripts')		.set_icon('dustmod', 'scripticon').init_shortcut_key(VK::S));
		add_tool(HelpTool('Help')		.set_icon('editor',  'helpicon'));
		
		// Custom
		
		add_tool('Props',		PropTool());
		add_tool('Triggers',	TextTool());
		add_tool('Triggers',	ExtendedTriggerTool());
	}
	
	// //////////////////////////////////////////////////////////
	// Main Methods
	// //////////////////////////////////////////////////////////
	
	void editor_step()
	{
		if(@editor == null)
			return;
		
		if(!initialised)
		{
			initialise();
			initialised = true;
			
//			select_tool(get_tool('Prop Tool'));
		}
		
		screen_w = g.hud_screen_width(false);
		screen_h = g.hud_screen_height(false);
		
		view_x = cam.x();
		view_y = cam.y();
		
		cam.get_layer_draw_rect(0, 19, view_x1, view_y1, view_w, view_h);
		view_x2 = view_x1 + view_w;
		view_y2 = view_y1 + view_h;
		
		zoom = cam.editor_zoom();
		
		const string new_tab = editor.editor_tab();
//		debug.print('selected_tab: ' + new_tab, 'selected_tab');
//		debug.print('selected_tool: ' + selected_tool.name, 'selected_tool');
		
		handle_keyboard();
		handles.step();
		mouse.step();
		
		ctrl	= editor.key_check_gvb(GVB::Control);
		shift	= editor.key_check_gvb(GVB::Shift);
		alt		= editor.key_check_gvb(GVB::Alt);
		space	= editor.key_check_gvb(GVB::Space);
		
		mouse_in_gui = editor.mouse_in_gui();
		mouse_in_scene = !mouse_in_gui && !ui.is_mouse_over_ui && !ui.is_mouse_active && !space;
		scene_focus = @ui.focus ==  null;
		
		if(mouse.left_press)
		{
			space_on_press = space;
			pressed_in_scene = mouse_in_scene;
		}
		else if(mouse.left_release)
		{
			space_on_press = false;
			pressed_in_scene = false;
		}
		
		return_press = editor.key_check_pressed_gvb(GVB::Return);
		escape_press = editor.key_check_pressed_gvb(GVB::Escape);
		
		// `editor_tab` returns 'Help' when switching to the particle editor. As a workaround just ignore it for now.
		if(new_tab != selected_tab && new_tab != 'Help')
		{
			selected_tab = new_tab;
			select_tool(selected_tab, false);
			persist_state();
		}
		
		if(@ui.focus == null && shortcut_keys_enabled)
		{
			if(shift && editor.key_check_pressed_vk(VK::W))
			{
				select_next_tool(-1);
			}
			else if(shift && editor.key_check_pressed_vk(VK::E))
			{
				select_next_tool(1);
			}
			else if(!shift && !ctrl && !alt && !editor.is_polling_keyboard())
			{
				for(int i = num_tools_shortcut - 1; i >= 0; i--)
				{
					Tool@ tool = @tools_shortcut[i];
					
					if(editor.key_check_pressed_vk(tool.shortcut_key))
					{
						select_tool(tool.on_shortcut_key());
						persist_state();
						break;
					}
				}
			}
		}
		
		if(@selected_tool != null)
		{
			selected_tool.step();
		}
		
//		get_tool('Prop Tool').step();
		
		info_overlay.step();
		
		ui.step();
		debug.step();
		
		if(toolbar.hovered || num_tool_group_popups > 0)
		{
			if(toolbar.alpha < 1)
			{
				toolbar.alpha = min(toolbar.alpha + Settings::UIFadeSpeed * DT, 1.0);
			}
		}
		else if(toolbar.alpha > Settings::UIFadeAlpha)
		{
			toolbar.alpha = max(toolbar.alpha - Settings::UIFadeSpeed * DT, Settings::UIFadeAlpha);
		}
		
		state_persisted = false;
	}
	
	private void handle_keyboard()
	{
		if(editor.key_check_pressed_vk(VK::Pause))
		{
			shortcut_keys_enabled = !shortcut_keys_enabled;
			update_shortcut_keys_enabled_popup();
		}
		
		if(!shortcut_keys_enabled)
			return;
		
		if(pressed_key != -1)
		{
			pressed_key_active = false;
			
			if(editor.is_polling_keyboard() || !editor.key_check_gvb(pressed_key))
			{
				pressed_key = -1;
			}
			else
			{
				if(--pressed_timer == 0)
				{
					pressed_key_active = true;
					pressed_timer = Settings::KeyRepeatPeriod;
				}
				
				return;
			}
		}
		
		if(!editor.is_polling_keyboard())
		{
		  for(int i = int(Settings::RepeatKeys.length()) - 1; i >= 0; i--)
		  {
			const int key = Settings::RepeatKeys[i];
			
			if(!editor.key_check_pressed_gvb(key))
			  continue;
			
			pressed_key = key;
			pressed_timer = Settings::KeyPressDelay;
			pressed_key_active = true;
			break;
		  }
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(@selected_tool != null)
		{
			selected_tool.draw(sub_frame);
		}
		
//		get_tool('Prop Tool').draw(sub_frame);
		
		handles.draw();
		ui.draw();
		
		if(debug_ui)
		{
			ui.debug_draw();
		}
		
		debug.draw(sub_frame);
	}
	
	// //////////////////////////////////////////////////////////
	// Public Methods
	// //////////////////////////////////////////////////////////
	
	Tool@ get_tool(const string name)
	{
		if(!tools_map.exists(name))
			return null;
		
		return cast<Tool@>(tools_map[name]);
	}
	
	bool select_tool(const string name, const bool update_editor_tab=true)
	{
		if(!tools_map.exists(name))
			return false;
		
		select_tool(cast<Tool@>(tools_map[name]), update_editor_tab);
		
		return true;
	}
	
	void select_next_tool(const int dir=1)
	{
		if(@selected_tool == null)
		{
			select_tool(tools[0]);
			return;
		}
		
		ToolGroup@ group = selected_tool.group;
		Tool@ next_tool = selected_tool;
		
		do
		{
			if(@next_tool != null)
			{
				@next_tool = group.get_next_selectable_tool(next_tool, dir);
			}
			
			if(@next_tool != null)
				break;
			
			const int index = tool_groups.findByRef(group);
			
			if(index == -1)
			{
				@group = tool_groups[0];
			}
			else
			{
				@group = tool_groups[mod(index + (dir >= 0 ? 1 : -1), tool_groups.length())];
			}
			
			@next_tool = dir >= 1
				? group.get_first_selectable_tool()
				: group.get_last_selectable_tool();
		}
		while(@next_tool == null);
		
		select_tool(next_tool);
	}
	
	void track_tool_group_popups(const bool open)
	{
		num_tool_group_popups += open ? 1 : -1;
	}
	
	void world_to_hud(const float x, const float y, float &out hud_x, float &out hud_y, const bool ui_coords=true)
	{
		hud_x = (x - view_x1) / view_w * screen_w;
		hud_y = (y - view_y1) / view_h * screen_h;
		
		if(!ui_coords)
		{
			hud_x -= screen_w * 0.5;
			hud_y -= screen_h * 0.5;
		}
	}
	
	void transform(const float x, const float y, const int from_layer, const int to_layer, float &out out_x, float &out out_y)
	{
		transform_layer_position(g, view_x, view_y, x, y, from_layer, to_layer, out_x, out_y);
	}
	
	void transform_size(const float x, const float y, const int from_layer, const int to_layer, float &out out_x, float &out out_y)
	{
		const float scale = get_layer_scale(g, from_layer, to_layer);
		out_x = x * scale;
		out_y = y * scale;
	}
	
	entity@ pick_trigger()
	{
		if(!mouse_in_scene)
			return null;
		
		int i = g.get_entity_collision(mouse.y - 10, mouse.y + 10, mouse.x - 10, mouse.x + 10, ColType::Trigger);
		
		entity@ closest = null;
		float closest_dist = 10 * 10;
		
		while(i-- > 0)
		{
			entity@ e = g.get_entity_collision_index(i);
			
			const float dist = dist_sqr(e.x(), e.y(), mouse.x, mouse.y);
			
			if(dist < closest_dist)
			{
				closest_dist = dist;
				@closest = e;
			}
		}
		
		return closest;
	}
	
	int query_onscreen_entities(const ColType type, const bool expand_for_parallax=false)
	{
		float x1, y1, x2, y2;
		transform(view_x1, view_y1, 22, 6, x1, y1);
		transform(view_x2, view_y2, 22, 6, x2, y2);
		
		if(!expand_for_parallax)
		{
			x1 = view_x1;
			y1 = view_y1;
			x2 = view_x2;
			y2 = view_y2;
		}
		else
		{
			transform(view_x1, view_y1, 22, 6, x1, y1);
			transform(view_x2, view_y2, 22, 6, x2, y2);
		}
		
		const float padding = 100;
		return g.get_entity_collision(y1 - padding, y2 + padding, x1 - padding, x2 + padding, type);
	}
	
	void hide_gui(const bool hide=true)
	{
		editor.hide_gui(hide);
		position_toolbar();
	}
	
	BoolSetting@ get_bool(Tool@ tool, const string name, const bool default_value=false)
	{
		const string key = tool.name + '.bool.' + name;
		
		if(!settings.exists(key))
		{
			bool_settings.resize(bool_settings.length() + 1);
			BoolSetting@ setting = @bool_settings[bool_settings.length() - 1];
			setting.key = key;
			setting.value = default_value;
			@settings[key] = @setting;
			return setting;
		}
		
		return cast<BoolSetting@>(settings[key]);
	}
	
	IntSetting@ get_int(Tool@ tool, const string name, const int default_value=false)
	{
		const string key = tool.name + '.int.' + name;
		
		if(!settings.exists(key))
		{
			int_settings.resize(int_settings.length() + 1);
			IntSetting@ setting = @int_settings[int_settings.length() - 1];
			setting.key = key;
			setting.value = default_value;
			@settings[key] = @setting;
			return setting;
		}
		
		return cast<IntSetting@>(settings[key]);
	}
	
	FloatSetting@ get_float(Tool@ tool, string name, const float default_value=0)
	{
		const string key = tool.name + '.float.' + name;
		
		if(!settings.exists(key))
		{
			float_settings.resize(float_settings.length() + 1);
			FloatSetting@ setting = @float_settings[float_settings.length() - 1];
			setting.key = key;
			setting.value = default_value;
			@settings[key] = @setting;
			return setting;
		}
		
		return cast<FloatSetting@>(settings[key]);
	}
	
	StringSetting@ get_string(Tool@ tool, string name, const string default_value='')
	{
		const string key = tool.name + '.string.' + name;
		
		if(!settings.exists(key))
		{
			string_settings.resize(string_settings.length() + 1);
			StringSetting@ setting = @string_settings[string_settings.length() - 1];
			setting.key = key;
			setting.value = default_value;
			@settings[key] = @setting;
			return setting;
		}
		
		return cast<StringSetting@>(settings[key]);
	}
	
	void draw_select_rect(const float x1, const float y1, const float x2, const float y2)
	{
		g.draw_rectangle_world(
			22, 22,
			x1, y1, x2, y2,
			0, Settings::SelectRectFillColour);
		
		outline_rect(g, 22, 22,
			x1, y1, x2, y2,
			Settings::SelectRectLineWidth / zoom, Settings::SelectRectLineColour);
	}
	
	void snap(const float x, const float y, float &out out_x, float &out out_y, const float custom_snap_size=5, const bool default_shift=false)
	{
		const float snap = get_snap_size(custom_snap_size, default_shift);
		
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
	
	float get_snap_size(const float custom_snap_size=5, const bool default_shift=false)
	{
		if(shift || default_shift && !ctrl && !alt)
			return 48;
		
		if(ctrl)
			return 24;
		
		if(alt)
			return custom_snap_size;
		
		return 0;
	}
	
	float get_snap_angle()
	{
		if(shift)
			return 45;
		
		if(ctrl)
			return 22.5;
		
		if(alt)
			return 5;
		
		return 0;
	}
	
	bool is_same_parallax(const int layer1, const int layer2)
	{
		return layer1 >= 12 && layer2 >= 12 || layer1 == layer2;
	}
	
	void show_layer_sublayer_overlay(const float x1, const float y1, const float x2, const float y2, const int layer, const int sublayer)
	{
		info_overlay.show(x1, y1, x2, y2, layer + '.' + sublayer, 0.75);
	}
	
	void show_layer_sublayer_overlay(IWorldBoundingBox@ target, const int layer, const int sublayer)
	{
		info_overlay.show(target, layer + '.' + sublayer, 0.75);
	}
	
	bool key_repeat_gvb(const int gvb)
	{
		return pressed_key == gvb && pressed_key_active;
	}
	
	void world_to_local(
		const float x, const float y, const int layer,
		const float to_x, const float to_y, const float to_rotation,
		const int to_layer,
		float &out local_x, float &out local_y)
	{
		transform(x, y, layer, to_layer, local_x, local_y);
		local_x -= to_x;
		local_y -= to_y;
		rotate(local_x, local_y, -to_rotation * DEG2RAD, local_x, local_y);
	}
	
	// //////////////////////////////////////////////////////////
	// Private Methods
	// //////////////////////////////////////////////////////////
	
	/// Add a tool and create a group with the same name
	private void add_tool(Tool@ tool)
	{
		add_tool(tool.name, tool);
	}
	
	/// Add a tool to the group with the specified name, creating one if it does not exist.
	private void add_tool(const string group_name, Tool@ tool)
	{
		ToolGroup@ group;
		
		if(!tool_groups_map.exists(group_name))
		{
			tool_groups.resize(tool_groups.length() + 1);
			@group = @tool_groups[tool_groups.length() - 1];
			group.create(this, group_name, button_group);
			@tool_groups_map[group_name] = group;
		}
		else
		{
			@group = cast<ToolGroup@>(tool_groups_map[group_name]);
		}
		
		add_tool(group, tool);
	}
	
	/// Add a tool to an existing tool group
	private void add_tool(ToolGroup@ group, Tool@ tool)
	{
		if(@group == null || @tool == null)
			return;
		
		if(tools_map.exists(tool.name))
		{
			puts('Tool "' + tool.name + '" already exists.');
			return;
		}
		
		group.add_tool(tool);
		@tools_map[tool.name] = tool;
		tools.insertLast(tool);
		
		if(tool.shortcut_key > 0 && tool.register_shortcut_key)
		{
			tools_shortcut.insertLast(@tool);
			num_tools_shortcut++;
		}
	}
	
	/// Select the specified tool and call the relevant callbacks. Cannot be null.
	private void select_tool(Tool@ tool, const bool update_editor_tab=true)
	{
		if(@tool == null)
			return;
		
		if(@tool == @selected_tool)
		{
			if(@selected_tool != null)
			{
				selected_tool.on_reselect();
			}
			
			do_update_editor_tab(update_editor_tab);
			return;
		}
		
		if(!tool.on_before_select())
		{
			if(@selected_tool != null)
			{
				selected_tool.group.on_select();
			}
			
			return;
		}
		
		ignore_toolbar_select_event = true;
		
		if(@selected_tool != null)
		{
			selected_tool.on_deselect();
			selected_tool.group.on_deselect();
		}
		
		@selected_tool = tool;
		selected_tool_name = selected_tool.name;
		do_update_editor_tab(update_editor_tab);
		
		selected_tool.on_select();
		selected_tool.group.on_select();
		ignore_toolbar_select_event = false;
		
		selected_tab = editor.editor_tab() != selected_tool.name
			? 'Scripts' : selected_tool.name;
		ui.mouse_enabled = true;
	}
	
	private void do_update_editor_tab(const bool do_update)
	{
		if(!do_update || @selected_tool == null)
			return;
		
		editor.editor_tab(selected_tool.name);
		
		if(editor.editor_tab() != selected_tool.name)
		{
			editor.editor_tab(selected_tab = 'Scripts');
		}
	}
	
	private void position_toolbar()
	{
		toolbar.x = (ui.region_width - toolbar.width) * 0.5;
//		toolbar.y = ui.region_height - toolbar.height;
		toolbar.y = editor.hide_gui() ? 0 : 60;
//		toolbar.y = 0;
	}
	
	private void update_shortcut_keys_enabled_popup()
	{
		if(shortcut_keys_enabled && @shortcut_keys_enabled_popup == null)
			return;
		
		if(@shortcut_keys_enabled_popup == null)
		{
			@shortcut_keys_enabled_popup = PopupOptions(ui, '', false, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::Manual);
		}
		
		if(!shortcut_keys_enabled)
		{
			shortcut_keys_enabled_popup.content_string = shortcut_keys_enabled ? 'Shortcut Keys Enabled' : 'Shortcut Keys Disabled';
			ui.show_tooltip(shortcut_keys_enabled_popup, toolbar);
		}
		else
		{
			ui.hide_tooltip(shortcut_keys_enabled_popup);
		}
	}
	
	void persist_state()
	{
		if(state_persisted)
			return;
		
//		controllable@ p = controller_controllable(0);
//		
//		if(@p != null)
//		{
//			p.x(g.get_checkpoint_x(0));
//			p.y(g.get_checkpoint_y(0));
//		}
//		
//		g.save_checkpoint(0, 0);
		state_persisted = true;
	}
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_clipboard_change(EventInfo@ event)
	{
		clipboard = event.value;
	}
	
	private void on_tool_button_select(EventInfo@ event)
	{
		if(ignore_toolbar_select_event)
			return;
		
		if(@event.target == null)
			return;
		
		select_tool(cast<Tool@>(tools_map[event.target.name]));
		persist_state();
	}
	
	//
	
	private void on_after_layout(EventInfo@ event)
	{
		position_toolbar();
		ui.after_layout.off(on_after_layout_delegate);
	}
	
	private void on_screen_resize(EventInfo@ event)
	{
		position_toolbar();
	}
	
}
