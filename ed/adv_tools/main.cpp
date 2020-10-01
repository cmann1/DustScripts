#include '../../lib/std.cpp';
#include '../../lib/layer.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/enums/GVB.cpp';
#include '../../lib/enums/VK.cpp';
#include '../../lib/embed_utils.cpp';
#include '../../lib/drawing/common.cpp';
#include '../../lib/debug/Debug.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/input/Mouse.cpp';
#include '../../lib/math/Line.cpp';
#include '../../lib/utils/colour.cpp';
#include '../../lib/utils/copy_vars.cpp';
#include '../../lib/utils/print_vars.cpp';

#include '../../lib/ui3/UI.cpp';
#include '../../lib/ui3/elements/Toolbar.cpp';
#include '../../lib/ui3/window_manager/WindowManager.cpp';

#include 'ToolGroup.cpp';
#include 'misc/ToolListenerInterfaces.cpp';
#include 'settings/Settings.cpp';
#include 'tools/prop_tool/PropTool.cpp';
#include 'tools/TextTool.cpp';
#include 'tools/ExtendedTriggerTool.cpp';
#include 'tools/HelpTool.cpp';

const string SCRIPT_BASE			= 'ed/adv_tools/';
const string SPRITES_BASE			= SCRIPT_BASE + 'sprites/';
const string EMBED_spr_icon_edit	= SPRITES_BASE + 'icon_edit.png';

class script : AdvToolScript {}

class AdvToolScript
{
	
	scene@ g;
	editor_api@ editor;
	camera@ cam;
	UI@ ui = UI(true);
	Mouse mouse(false);
	Debug debug();
	Line line;
	
	float zoom;
	bool ctrl, shift, alt, space;
	bool return_press, escape_press;
	
	[text] bool debug_ui;
	[hidden] string clipboard;
	[hidden] WindowManager window_manager;
	[hidden] array<BoolSetting> bool_settings;
	[hidden] array<FloatSetting> float_settings;
	
	private dictionary settings;
	
	private bool initialised;
	
	private Toolbar@ toolbar;
	private array<ToolGroup> tool_groups;
	private array<Tool@> tools;
	private array<Tool@> tools_shortcut;
	private int num_tools_shortcut;
	private dictionary tool_groups_map;
	private dictionary tools_map;
	private ButtonGroup@ button_group = ButtonGroup(ui, false);
	
	private string selected_tab;
	private Tool@ selected_tool;
	private int num_tool_group_popups;
	
	private EventCallback@ on_after_layout_delegate;
	
	//
	
	float view_x, view_y;
	float view_x1, view_y1;
	float view_x2, view_y2;
	float view_w, view_h;
	float screen_w, screen_h;
	
	private int size_onscreen_triggers = 32;
	private int num_onscreen_triggers = -1;
	private array<entity@> onscreen_triggers(size_onscreen_triggers);
	
	// //////////////////////////////////////////////////////////
	// Init
	// //////////////////////////////////////////////////////////
	
	AdvToolScript()
	{
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
		initialise_settings();
		initialise_ui();
		initialise_tools();
		
		select_tool(editor.editor_tab());
	}
	
	private void initialise_settings()
	{
		for(uint i = 0; i < bool_settings.length(); i++)
		{
			@settings[bool_settings[i].key] = @bool_settings[i];
		}
		
		for(uint i = 0; i < float_settings.length(); i++)
		{
			@settings[float_settings[i].key] = @float_settings[i];
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
		add_tool(Tool('Props')			.set_icon('editor',  'propsicon'));
		add_tool(Tool('Entities')		.set_icon('editor',  'entityicon'));
		add_tool(Tool('Triggers')		.set_icon('editor',  'triggersicon'));
		add_tool(Tool('Camera')			.set_icon('editor',  'cameraicon'));
		add_tool(Tool('Emitters')		.set_icon('editor',  'emittericon'));
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
		
		mouse.step(ui.is_mouse_over_ui || editor.mouse_in_gui());
		
		ctrl	= editor.key_check_gvb(GVB::Control);
		shift	= editor.key_check_gvb(GVB::Shift);
		alt		= editor.key_check_gvb(GVB::Alt);
		space	= editor.key_check_gvb(GVB::Space);
		
		return_press = editor.key_check_pressed_gvb(GVB::Return);
		escape_press = editor.key_check_pressed_gvb(GVB::Escape);
		
		if(new_tab != selected_tab)
		{
			selected_tab = new_tab;
			select_tool(selected_tab);
		}
		
		if(@ui.focus == null)
		{
			for(int i = num_tools_shortcut - 1; i >= 0; i--)
			{
				Tool@ tool = @tools_shortcut[i];
				
				if(editor.key_check_pressed_vk(tool.shortcut_key))
				{
					select_tool(tool.on_shortcut_key());
					break;
				}
			}
		}
		
		if(@selected_tool != null)
		{
			selected_tool.step();
		}
		
		num_onscreen_triggers = -1;
		
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
	}
	
	void editor_draw(float sub_frame)
	{
		if(@selected_tool != null)
		{
			selected_tool.draw(sub_frame);
		}
		
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
	
	bool select_tool(const string name)
	{
		if(!tools_map.exists(name))
			return false;
		
		select_tool(cast<Tool@>(tools_map[name]));
		
		return true;
	}
	
	void track_tool_group_popups(const bool open)
	{
		num_tool_group_popups += open ? 1 : -1;
	}
	
	int get_onscreen_triggers(array<entity@>@ &out triggers)
	{
		if(num_onscreen_triggers == -1)
		{
			num_onscreen_triggers = g.get_entity_collision(view_y1, view_y2, view_x1, view_x2, ColType::Trigger);
			
			for(int i = 0; i < num_onscreen_triggers; i++)
			{
				@triggers[i] = g.get_entity_collision_index(i);
			}
		}
		
		@triggers = @onscreen_triggers;
		return num_onscreen_triggers;
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
	
	entity@ pick_trigger()
	{
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
		
		if(tool.shortcut_key > 0)
		{
			tools_shortcut.insertLast(@tool);
			num_tools_shortcut++;
		}
	}
	
	/// Select the specified tool and call the relevant callbacks. Cannot be null.
	private void select_tool(Tool@ tool)
	{
		if(@tool == null || @tool == @selected_tool)
			return;
		
		if(!tool.on_before_select())
		{
			if(@selected_tool != null)
			{
				selected_tool.group.on_select();
			}
			
			return;
		}
		
		if(@selected_tool != null)
		{
			selected_tool.on_deselect();
			selected_tool.group.on_deselect();
		}
		
		editor.editor_tab(tool.name);
		
		if(editor.editor_tab() != tool.name)
		{
			editor.editor_tab(selected_tab = 'Scripts');
		}
		
		@selected_tool = tool;
		selected_tool.on_select();
		selected_tool.group.on_select();
	}
	
	private void position_toolbar()
	{
		toolbar.x = (ui.region_width - toolbar.width) * 0.5;
//		toolbar.y = ui.region_height - toolbar.height;
		toolbar.y = editor.hide_gui() ? 0 : 60;
//		toolbar.y = 0;
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
		if(@event.target == null)
			return;
		
		select_tool(cast<Tool@>(tools_map[event.target.name]));
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

//enum TextToolState { Disabled, Idle, Active, }