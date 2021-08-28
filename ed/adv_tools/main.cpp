#include '../../lib/std.cpp';
#include '../../lib/layer.cpp';
#include '../../lib/string.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/math/geom.cpp';
#include '../../lib/enums/GVB.cpp';
#include '../../lib/enums/VK.cpp';
#include '../../lib/embed_utils.cpp';
#include '../../lib/drawing/common.cpp';
#include '../../lib/drawing/circle.cpp';
#include '../../lib/debug/Debug.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/input/common.cpp';
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
#include 'misc/ShortcutKeySorter.cpp';
#include 'misc/ToolListenerInterfaces.cpp';
#include 'misc/WorldBoundingBox.cpp';
#include 'settings/Config.cpp';
#include 'settings/Settings.cpp';
#include 'tools/edge_brush/EdgeBrushTool.cpp';
#include 'tools/emitter_tool/EmitterTool.cpp';
#include 'tools/prop_tool/PropTool.cpp';
#include 'tools/TextTool.cpp';
#include 'tools/ExtendedTriggerTool.cpp';
#include 'tools/HelpTool.cpp';
#include 'tools/ParticleEditorTool.cpp';
#include 'ToolGroup.cpp';

const string SCRIPT_BASE			= 'ed/adv_tools/';
const string SPRITES_BASE			= SCRIPT_BASE + 'sprites/';
const string EMBED_spr_icon_edit	= SPRITES_BASE + 'icon_edit.png';

const bool AS_EDITOR_PLUGIN = true;
const string SPRITE_SET = AS_EDITOR_PLUGIN ? 'plugin' : 'script';

class script : AdvToolScript {}

class AdvToolScript
{
	
	scene@ g;
	editor_api@ editor;
	input_api@ input;
	camera@ cam;
	UI@ ui;
	Mouse@ mouse;
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
	/// Kind of a hack, but can be set to "consume" a single shortcut key.
	/// Any tool shortcuts matching this will not be able to trigger
	ShortcutKey blocked_key;
	
	InfoOverlay info_overlay;
	
	bool debug_ui;
	string clipboard;
	WindowManager window_manager;
	PropsClipboardData props_clipboard;
	string selected_tool_name;
	
	Config config(this);
	
	private bool initialised;
	private bool state_persisted = true;
	private bool queue_load_config;
	private bool queue_load_config_trigger;
	
	private Toolbar@ toolbar;
	private array<ToolGroup> tool_groups;
	private array<Tool@> tools;
	private array<Tool@> tools_shortcut;
	private int num_tools_shortcut;
	private dictionary tool_groups_map;
	private dictionary tools_map;
	private ButtonGroup@ button_group;
	
	private PopupOptions@ shortcut_keys_enabled_popup;
	
	private array<Image@> icon_images;
	
	/// '_' = Tool has not been initialised yet
	private string selected_tab = '_';
	private string previous_selected_tab = '_';
	private Tool@ selected_tool;
	private int num_tool_group_popups;
	
	private EventCallback@ on_after_layout_delegate;
	
	private int pressed_key = -1;
	private bool pressed_key_active;
	private int pressed_timer;
	
	private bool ignore_toolbar_select_event;
	
	private bool hide_gui;
	private bool hide_panels_gui;
	private bool hide_layers_gui;
	
	//
	
	float view_x, view_y;
	float view_x1, view_y1;
	float view_x2, view_y2;
	float view_w, view_h;
	float screen_w, screen_h;
	
	array<float> layer_scales(23);
	/// Maps a layer index to it's order
	array<int> layer_positions(23);
	/// Maps an order to a layer index
	array<int> layer_indices(23);
	
	// //////////////////////////////////////////////////////////
	// Init
	// //////////////////////////////////////////////////////////
	
	AdvToolScript()
	{
		puts('>> Initialising AdvTools');
		@g = get_scene();
		@editor = get_editor_api();
		@input = get_input_api();
		@cam = get_active_camera();
		
		@ui = UI(true);
		@mouse = Mouse(false, 22);
		mouse.use_input(input);
		
		blocked_key.init(this);
		
		@button_group = ButtonGroup(ui, false);
		
		if(@editor == null)
		{
			initialised = true;
			return;
		}
		
		editor.hide_gui(false);
		do_load_config(false);
		create_tools();
	}
  
	void editor_loaded()
	{
		if(@selected_tool != null)
		{
			selected_tool.on_editor_loaded();
		}
		
		editor.hide_toolbar_gui(true);
		
		editor.hide_gui(hide_gui);
		editor.hide_panels_gui(hide_panels_gui);
		editor.hide_layers_gui(hide_layers_gui);
		
		store_layer_values();
	}

	void editor_unloaded()
	{
		if(@selected_tool != null)
		{
			selected_tool.on_editor_unloaded();
		}
		
		editor.hide_toolbar_gui(false);
		
		hide_gui = editor.hide_gui();
		hide_panels_gui = editor.hide_panels_gui();
		hide_layers_gui = editor.hide_layers_gui();
		editor.hide_gui(false);
		editor.hide_panels_gui(false);
		editor.hide_layers_gui(false);
	}
	
	void build_sprites(message@ msg)
	{
		build_sprite(@msg, 'icon_edit', 0, 0);
		
		for(uint i = 0; i < tools.length(); i++)
		{
			tools[i].build_sprites(msg);
		}
	}
	
	void reload_config()
	{
		queue_load_config = true;
	}
	
	private bool do_load_config(const bool trigger_load=true)
	{
		if(!config.load())
			return false;
		
		bool requires_reload = false;
		
		if(!requires_reload && !config.compare_float('UISpacing', ui.style.spacing))
			requires_reload = true;
		if(!requires_reload && !config.compare_colour('UIBGColour', ui.style.normal_bg_clr))
			requires_reload = true;
		if(!requires_reload && !config.compare_colour('UIBorderColour', ui.style.normal_border_clr))
			requires_reload = true;
		
		if(!requires_reload)
		{
			initialise_ui_style();
			
			for(uint i = 0; i < icon_images.length; i++)
			{
				icon_images[i].colour = config.UIIconColour;
			}
		}
		
		for(uint i = 0; i < tools.length(); i++)
		{
			// Just easier to reload everything if a shortcut changes
			if(tools[i].reload_shortcut_key())
			{
				return true;
			}
		}
		
		if(trigger_load && !requires_reload)
		{
			for(uint i = 0; i < tools.length(); i++)
			{
				tools[i].on_settings_loaded();
			}
			for(uint i = 0; i < tool_groups.length(); i++)
			{
				tool_groups[i].on_settings_loaded();
			}
			
			ui.after_layout.on(on_after_layout_delegate);
			position_toolbar();
		}
		
		return requires_reload;
	}
	
	private void do_full_reload()
	{
		@ui = UI(true);
		@toolbar = null;
		tool_groups.resize(0);
		tools.resize(0);
		tools_shortcut.resize(0);
		num_tools_shortcut = 0;
		tool_groups_map.deleteAll();
		tools_map.deleteAll();
		icon_images.resize(0);
		selected_tab = '';
		@selected_tool = null;
		num_tool_group_popups = 0;
		
		create_tools();
		initialised = false;
	}
	
	private void initialise()
	{
		@editor_spr = create_sprites();
		editor_spr.add_sprite_set('editor');
		@script_spr = create_sprites();
		script_spr.add_sprite_set(SPRITE_SET);
		
		initialise_ui();
		initialise_tools();
		
		select_tool(selected_tool_name != '' ? selected_tool_name : editor.editor_tab(), false);
		
		info_overlay.init(this);
		handles.init(this);
	}
	
	private void initialise_ui()
	{
		@ui.debug = debug;
		ui.clipboard = clipboard;
		ui.clipboard_change.on(EventCallback(on_clipboard_change));
		ui.auto_fit_screen = true;
		
		initialise_ui_style();
		
		// ui.style.text_clr                        = 0xffffffff;
		// ui.style.normal_bg_clr                   = 0xd9050505;
		// ui.style.normal_border_clr               = 0x33ffffff;
		// ui.style.highlight_bg_clr                = 0xd933307c;
		// ui.style.highlight_border_clr            = 0xd96663c2;
		// ui.style.selected_bg_clr                 = 0xd933307c;
		// ui.style.selected_border_clr             = 0xff7d7acb;
		// ui.style.selected_highlight_bg_clr       = 0xd9423fa0;
		// ui.style.selected_highlight_border_clr   = 0xff7d7acb;
		// ui.style.disabled_bg_clr                 = 0xa6000000;
		// ui.style.disabled_border_clr             = 0x26ffffff;
		// ui.style.secondary_bg_clr                = 0x667f7daf;
		// ui.style.scrollbar_light_bg_clr          = 0xd9111111;
		
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
	
	private void initialise_ui_style()
	{
		ui.style.spacing = config.get_float('UISpacing', ui.style.spacing);
		
		if(config.has_value('UITextColour'))
			ui.style.auto_text_colour(config.get_colour('UITextColour'));
		if(config.has_value('UIBGColour'))
			ui.style.auto_base_colour(config.get_colour('UIBGColour'));
		if(config.has_value('UIBorderColour'))
			ui.style.auto_border_colour(config.get_colour('UIBorderColour'));
		if(config.has_value('UIAccentColour'))
			ui.style.auto_accent_colour(config.get_colour('UIAccentColour'));
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
		
		add_tool(Tool(this, 'Select')			.set_icon('editor',  'selecticon').init_shortcut_key(VK::R));
		add_tool(Tool(this, 'Tiles')			.set_icon('editor',  'tilesicon').init_shortcut_key(VK::W));
		add_tool(Tool(this, 'Props')			.set_icon('editor',  'propsicon').init_shortcut_key(VK::Q));
		add_tool(Tool(this, 'Entities')			.set_icon('editor',  'entityicon').init_shortcut_key(VK::E));
		add_tool(Tool(this, 'Triggers')			.set_icon('editor',  'triggersicon').init_shortcut_key(VK::T));
		add_tool(Tool(this, 'Camera')			.set_icon('editor',  'cameraicon').init_shortcut_key(VK::C));
		add_tool(EmitterTool(this));
		add_tool(Tool(this, 'Level Settings')	.set_icon('editor',  'settingsicon'));
		add_tool(Tool(this, 'Scripts')			.set_icon('dustmod', 'scripticon').init_shortcut_key(VK::S));
		add_tool(HelpTool(this, 'Help')			.set_icon('editor',  'helpicon'));
		
		@tools_map[''] = null;
		@tools_map['Wind'] = null;
		@tools_map['Particle'] = ParticleEditorTool(this);
		
		// Custom
		
		add_tool('Tiles',		EdgeBrushTool(this));
		add_tool('Props',		PropTool(this));
		add_tool('Triggers',	TextTool(this));
		add_tool('Triggers',	ExtendedTriggerTool(this));
		
		sort_shortcut_tools();
	}
	
	private void sort_shortcut_tools()
	{
		array<ShortcutKeySorter> sort_list(tools_shortcut.length);
		for(uint i = 0; i < tools_shortcut.length; i++)
		{
			@sort_list[i].tool = tools_shortcut[i];
			sort_list[i].index = i;
		}
		
		dictionary shortcut_map;
		sort_list.sortAsc();
		for(uint i = 0; i < sort_list.length; i++)
		{
			Tool@ tool = sort_list[i].tool;
			const string map_key = tool.key.to_string();
			
			@tool.shortcut_key_group = shortcut_map.exists(map_key)
				? cast<Tool@>(shortcut_map[map_key]).shortcut_key_group
				: array<Tool@>();
			tool.shortcut_key_group.insertLast(tool);
			
			@tools_shortcut[i] = tool;
			@shortcut_map[map_key] = tool;
		}
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
		}
		
		screen_w = g.hud_screen_width(false);
		screen_h = g.hud_screen_height(false);
		
		view_x = cam.x();
		view_y = cam.y();
		
		cam.get_layer_draw_rect(0, 19, view_x1, view_y1, view_w, view_h);
		view_x2 = view_x1 + view_w;
		view_y2 = view_y1 + view_h;
		
		zoom = cam.editor_zoom();
		
		ctrl	= input.key_check_gvb(GVB::Control);
		shift	= input.key_check_gvb(GVB::Shift);
		alt		= input.key_check_gvb(GVB::Alt);
		space	= input.key_check_gvb(GVB::Space);
		
		mouse_in_gui = editor.mouse_in_gui();
		mouse_in_scene = !mouse_in_gui && !ui.is_mouse_over_ui && !ui.is_mouse_active && !space;
		scene_focus = @ui.focus ==  null;
		
		handle_keyboard();
		handles.step();
		mouse.step(space || !mouse_in_scene);
		
		const string new_selected_tab = editor.editor_tab();
		if(
			new_selected_tab != selected_tab && (
			new_selected_tab == '' || new_selected_tab == 'Help' ||
			new_selected_tab == 'Particle' || new_selected_tab == 'Wind'))
		{
			select_tool(new_selected_tab, false);
		}
		
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
		
		return_press = input.key_check_pressed_gvb(GVB::Return);
		escape_press = input.key_check_pressed_gvb(GVB::Escape);
		
		if(config.EnableShortcuts && @ui.focus == null && shortcut_keys_enabled && !input.is_polling_keyboard())
		{
			if(config.KeyPrevTool.check())
			{
				select_next_tool(-1);
			}
			else if(config.KeyNextTool.check())
			{
				select_next_tool(1);
			}
			else
			{
				//if(selected_tab == 'Particle')
				for(int i = num_tools_shortcut - 1; i >= 0; i--)
				{
					Tool@ tool = @tools_shortcut[i];
					
					if(!tool.key.matches(blocked_key) && tool.key.check())
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
		
		// get_tool('Prop Tool').step();
		
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
		
		if(queue_load_config)
		{
			if(do_load_config())
			{
				do_full_reload();
			}
			
			queue_load_config = false;
		}
	}
	
	private void handle_keyboard()
	{
		if(input.key_check_pressed_vk(VK::Pause))
		{
			shortcut_keys_enabled = !shortcut_keys_enabled;
			update_shortcut_keys_enabled_popup();
		}
		
		if(!shortcut_keys_enabled)
			return;
		
		if(pressed_key != -1)
		{
			pressed_key_active = false;
			
			if(input.is_polling_keyboard() || !input.key_check_gvb(pressed_key))
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
		
		if(!input.is_polling_keyboard())
		{
		  for(int i = int(Settings::RepeatKeys.length()) - 1; i >= 0; i--)
		  {
			const int key = Settings::RepeatKeys[i];
			
			if(!input.key_check_pressed_gvb(key))
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
		
		// get_tool('Prop Tool').draw(sub_frame);
		
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
	
	bool select_tool(const string &in name, const bool update_editor_tab=true)
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
	
	void init_icon(Image@ img)
	{
		if(@img == null)
			return;
		
		img.colour = config.UIIconColour;
		icon_images.insertLast(img);
	}
	
	void init_icon(Button@ button)
	{
		init_icon(button.icon);
	}
	
	void init_icon(MultiButton@ mbutton)
	{
		for(uint i = 0, count = mbutton.num_items; i < count; i++)
		{
			init_icon(mbutton.get_image(i));
		}
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
	
	int layer_position(const int layer_index)
	{
		return layer_positions[layer_index];
	}
	
	int layer_index(const int layer_position)
	{
		return layer_indices[layer_position];
	}
	
	float layer_scale(const int layer_index)
	{
		return layer_scales[layer_index];
	}
	
	float layer_scale(const int from_layer, const int to_layer)
	{
		return layer_scales[from_layer] / layer_scales[to_layer];
	}
	
	void transform(const float x, const float y, const int from_layer, const int to_layer,
		float &out out_x, float &out out_y)
	{
		transform_layer_position(x, y, from_layer, to_layer, out_x, out_y);
	}
	
	float transform_size(const float size, const int from_layer, const int to_layer)
	{
		return size * (layer_scales[from_layer] / layer_scales[to_layer]);
	}
	
	void transform_size(const float x, const float y, const int from_layer, const int to_layer,
		float &out out_x, float &out out_y)
	{
		const float scale = layer_scales[from_layer] / layer_scales[to_layer];
		out_x = x * scale;
		out_y = y * scale;
	}
	
	void mouse_layer(const int to_layer, float &out out_x, float &out out_y)
	{
		transform_layer_position(mouse.x, mouse.y, mouse.layer, to_layer, out_x, out_y);
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
	
	void snap(const float x, const float y, float &out out_x, float &out out_y, const float custom_snap_size=5,
		const bool default_shift=false)
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
	
	bool circle_segments(const float radius, const uint segments, uint &out out_segments,
		const float threshold=0, const bool world=true)
	{
		// Dynamically adjust the number of segments used to draw the circle
		// based on how big it is on screen
		const float view_height = 1080 / (world ? zoom : 1);
		float t = clamp01((radius * 2) / (view_height * 0.5));
		// Adjust the curve between high and low detail so the transition
		// to low detail doesn't happen too soon.
		// Easing out cubic
		t--;
		t = t * t * t + 1;
		
		// Don't draw circles that get too small
		if(segments * t < threshold)
		{
			out_segments = 0;
			return false;
		}
		
		out_segments = int(min(float(segments), 5 + segments * t));
		return true;
	}
	
	void circle(
		const uint layer, const uint sub_layer,
		const float x, const float y, const float radius, uint segments,
		const float thickness=2, const uint colour=0xFFFFFFFF,
		const float threshold=0, const bool world=true)
	{
		if(!circle_segments(radius, segments, segments, threshold, world))
			return;
		
		drawing::circle(
			g, layer, sub_layer,
			x, y, radius, segments,
			thickness / zoom, colour, world);
	}
	
	void fill_circle(
		const uint layer, const uint sub_layer,
		const float x, const float y, const float radius, uint segments,
		const uint inner_colour, const uint outer_colour,
		const float threshold=0, const bool world=true)
	{
		if(!circle_segments(radius, segments, segments, threshold, world))
			return;
		
		drawing::fill_circle(
			g, layer, sub_layer,
			x, y, radius / zoom, segments,
			inner_colour, outer_colour, world);
	}
	
	bool is_same_parallax(const int layer1, const int layer2)
	{
		return layer1 >= 12 && layer2 >= 12 || layer1 == layer2;
	}
	
	void show_layer_sublayer_overlay(const float x1, const float y1, const float x2, const float y2,
		const int layer, const int sublayer)
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
	
	void store_layer_values()
	{
		for(uint i = 0; i < 23; i++)
		{
			layer_positions[i] = g.get_layer_position(i);
			layer_indices[layer_positions[i]] = i;
			layer_scales[i] = g.layer_scale(i);
		}
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
		if(@tool == null)
			return;
		
		if(tools_map.exists(tool.name))
		{
			puts('Tool "' + tool.name + '" already exists.');
			return;
		}
		
		@tools_map[tool.name] = tool;
		tools.insertLast(tool);
		
		if(@group != null)
		{
			group.add_tool(tool);
		}
		
		if(tool.key.is_set())
		{
			tools_shortcut.insertLast(@tool);
			num_tools_shortcut++;
		}
	}
	
	/// Select the specified tool and call the relevant callbacks.
	private void select_tool(Tool@ tool, const bool update_editor_tab=true, const bool allow_reselected=true)
	{
		// Don't trigger a reselect if this is the firs t tool to be selected ('_')
		if(@tool == @selected_tool && selected_tab != '_')
		{
			if(@selected_tool != null)
			{
				selected_tool.on_reselect();
			}
			
			do_update_editor_tab(update_editor_tab);
			on_tool_changed(allow_reselected);
			return;
		}
		
		if(@tool != null && !tool.on_before_select())
		{
			if(@selected_tool.group != null)
			{
				selected_tool.group.on_select();
			}
			
			return;
		}
		
		ignore_toolbar_select_event = true;
		
		// Don't allow the user to deselect, but allow select_tool(null) to deselect tool buttons
		button_group.allow_deselect = true;
		
		if(@selected_tool != null)
		{
			selected_tool.on_deselect();
			
			if(@selected_tool.group != null)
			{
				selected_tool.group.on_deselect();
			}
		}
		
		store_layer_values();
		
		@selected_tool = tool;
		selected_tool_name = @selected_tool != null ? selected_tool.name : '';
		do_update_editor_tab(update_editor_tab);
		
		if(@selected_tool != null)
		{
			selected_tool.on_select();
			
			if(@selected_tool.group != null)
			{
				selected_tool.group.on_select();
			}
		}
		
		ignore_toolbar_select_event = false;
		button_group.allow_deselect = false;
		
		selected_tab = selected_tool_name == ''
			? editor.editor_tab() : selected_tool_name;
		ui.mouse_enabled = true;
		on_tool_changed(false);
	}
	
	private void do_update_editor_tab(const bool do_update)
	{
		if(!do_update || @selected_tool == null)
			return;
		
		editor.editor_tab(selected_tool.name);
		
		if(editor.editor_tab() != selected_tool.name)
		{
			editor.editor_tab('Scripts');
		}
	}
	
	private void position_toolbar()
	{
		toolbar.x = (ui.region_width - toolbar.width) * 0.5;
		toolbar.y = 0;
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
	
	private void transform_layer_position(
		const float x, const float y,
		const int from_layer, const int to_layer,
		float &out out_x, float &out out_y)
	{
		const float scale = layer_scales[from_layer] / layer_scales[to_layer];
		
		const float dx = (x - view_x) * scale;
		const float dy = (y - view_y) * scale;
		
		out_x = view_x + dx;
		out_y = view_y + dy;
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
	
	private void on_tool_changed(const bool reselected)
	{
		previous_selected_tab = selected_tab;
	}
	
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
