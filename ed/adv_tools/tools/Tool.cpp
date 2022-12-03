class Tool
{
	
	string name = '';
	string base_tool_name = '';
	string icon_sprite_set;
	string icon_sprite_name;
	float icon_width;
	float icon_height;
	float icon_offset_x;
	float icon_offset_y;
	float icon_rotation;
	
	bool selectable = true;
	
	AdvToolScript@ script;
	ToolGroup@ group;
	Button@ toolbar_button;
	
	array<Tool@>@ shortcut_key_group;
	ShortcutKey key;
	
	/** Setting this to true while a tool is performing an action while prevent other tool shortcut keys from being run. */
	bool active;
	
	protected array<IToolEditorLoadListener@> editor_load_listeners;
	protected array<IToolSelectListener@> select_listeners;
	protected array<IToolStepListener@> step_listeners;
	protected array<IToolDrawListener@> draw_listeners;
	protected int num_editor_load_listeners;
	protected int num_select_listeners;
	protected int num_step_listeners;
	protected int num_draw_listeners;
	
	protected bool selected = false;
	
	private string default_key_config = '';
	private int default_key=-1;
	private int default_key_modifiers=ModifierKey::None;
	
	protected void construct(AdvToolScript@ script, const string & in base_tool_name, const string &in name)
	{
		@this.script = script;
		this.base_tool_name = base_tool_name;
		this.name = name;
		
		key.init(script);
	}
	
	Tool(AdvToolScript@ script)
	{
		construct(script, '', '');
	}
	
	Tool(AdvToolScript@ script, const string name)
	{
		construct(script, name, name);
	}
	
	Tool(AdvToolScript@ script, const string &in base_tool_name, const string &in name)
	{
		construct(script, base_tool_name, name);
	}
	
	void create(ToolGroup@ group)
	{
		@this.group = group;
	}
	
	void on_init()
	{
		
	}
	
	void build_sprites(message@ msg)
	{
		
	}
	
	Tool@ set_icon(
		const string sprite_set, const string sprite_name, const float width=-1, const float height=-1,
		const float offset_x=0, const float offset_y=0, const float rotation=0)
	{
		this.icon_sprite_set = sprite_set;
		this.icon_sprite_name = sprite_name;
		this.icon_width = width;
		this.icon_height = height;
		this.icon_offset_x = offset_x;
		this.icon_offset_y = offset_y;
		this.icon_rotation = rotation;
		
		return this;
	}
	
	Tool@ init_shortcut_key(
		const string &in config_name, const int default_key=-1,
		const int default_modifiers=ModifierKey::None, const int priority=0)
	{
		this.default_key_config = config_name;
		this.default_key = default_key;
		this.default_key_modifiers = default_modifiers;
		
		// First check this tool's name (with spaces stripped) for a key config
		key.from_config('Key' + string::replace(name, ' ', ''), '', priority);
		
		// Use the provided default
		if(key.vk <= 0)
		{
			key.set(default_key, default_modifiers);
		}
		
		// If there isn't a config or default, check the parent tool config
		if(key.vk <= 0)
		{
			key.from_config('Key' + config_name);
		}
		
		return @this;
	}
	
	Tool@ init_shortcut_key(const int shortcut_key, const int modifiers=ModifierKey::None, const int priority=0)
	{
		return init_shortcut_key(base_tool_name, shortcut_key, modifiers, priority);
	}
	
	bool reload_shortcut_key()
	{
		const int prev_vk = key.vk;
		const int prev_mod = key.modifiers;
		init_shortcut_key(default_key_config, default_key, default_key_modifiers, key.priority);
		
		return prev_vk != key.vk || prev_mod != key.modifiers;
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
	// Select
	
	void register_editor_load_listener(IToolEditorLoadListener@ listener)
	{
		if(editor_load_listeners.findByRef(listener) != -1)
			return;
		
		editor_load_listeners.insertLast(listener);
		num_editor_load_listeners++;
	}
	
	void deregister_editor_load_listener(IToolEditorLoadListener@ listener)
	{
		const int index = editor_load_listeners.findByRef(listener);
		
		if(index == -1)
			return;
		
		editor_load_listeners.removeAt(index);
		num_editor_load_listeners--;
	}
	
	// Select
	
	void register_select_listener(IToolSelectListener@ listener)
	{
		if(select_listeners.findByRef(listener) != -1)
			return;
		
		select_listeners.insertLast(listener);
		num_select_listeners++;
	}
	
	void deregister_select_listener(IToolSelectListener@ listener)
	{
		const int index = select_listeners.findByRef(listener);
		
		if(index == -1)
			return;
		
		select_listeners.removeAt(index);
		num_select_listeners--;
	}
	
	// Step
	
	void register_step_listener(IToolStepListener@ listener)
	{
		if(step_listeners.findByRef(listener) != -1)
			return;
		
		step_listeners.insertLast(listener);
		num_step_listeners++;
	}
	
	void deregister_step_listener(IToolStepListener@ listener)
	{
		const int index = step_listeners.findByRef(listener);
		
		if(index == -1)
			return;
		
		step_listeners.removeAt(index);
		num_step_listeners--;
	}
	
	// Draw
	
	void register_draw_listener(IToolDrawListener@ listener)
	{
		if(draw_listeners.findByRef(listener) != -1)
			return;
		
		draw_listeners.insertLast(listener);
		num_draw_listeners++;
	}
	
	void deregister_draw_listener(IToolDrawListener@ listener)
	{
		const int index = draw_listeners.findByRef(listener);
		
		if(index == -1)
			return;
		
		draw_listeners.removeAt(index);
		num_draw_listeners--;
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	void on_editor_loaded()
	{
		for(int i = num_editor_load_listeners - 1; i >= 0; i--)
		{
			editor_load_listeners[i].tool_editor_loaded(this);
		}
	}

	void on_editor_unloaded()
	{
		for(int i = num_editor_load_listeners - 1; i >= 0; i--)
		{
			editor_load_listeners[i].tool_editor_unloaded(this);
		}
	}
	
	Tool@ on_shortcut_key()
	{
		if(@shortcut_key_group == null)
			return this;
		
		Tool@ tool = this;
		
		// Cycle through tools with the same shortcut key
		for(uint i = 0, length = shortcut_key_group.length; i < length; i++)
		{
			Tool@ t = shortcut_key_group[i];
			
			if(t.selected)
			{
				@tool = shortcut_key_group[((int(i) - 1) % length + length) % length];
				break;
			}
		}
		
		return tool;
	}
	
	void on_reselect()
	{
		
	}
	
	bool on_before_select()
	{
		return true;
	}
	
	void on_select() final
	{
		selected = true;
		active = false;
		
		if(@group != null)
		{
			group.set_tool(this);
		}
		
		if(@toolbar_button != null)
		{
			toolbar_button.override_alpha = 1;
		}
		
		on_select_impl();
		
		for(int i = num_select_listeners - 1; i >= 0; i--)
		{
			select_listeners[i].tool_select(this);
		}
	}
	
	void on_deselect() final
	{
		selected = false;
		active = false;
		
		if(@toolbar_button != null)
		{
			toolbar_button.override_alpha = -1;
		}
		
		on_deselect_impl();
		
		for(int i = num_select_listeners - 1; i >= 0; i--)
		{
			select_listeners[i].tool_deselect(this);
		}
	}
	
	protected void on_select_impl()
	{
		
	}
	
	protected void on_deselect_impl()
	{
		
	}
	
	void step() final
	{
		step_impl();
		
		for(int i = num_step_listeners - 1; i >= 0; i--)
		{
			step_listeners[i].tool_step(this);
		}
	}
	
	void draw(const float sub_frame) final
	{
		draw_impl(sub_frame);
		
		for(int i = num_draw_listeners - 1; i >= 0; i--)
		{
			draw_listeners[i].tool_draw(this, sub_frame);
		}
	}
	
	void on_settings_loaded()
	{
		
	}
	
	protected void draw_impl(const float sub_frame)
	{
		
	}
	
	protected void step_impl()
	{
		
	}
	
}
