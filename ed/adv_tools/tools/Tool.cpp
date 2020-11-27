class Tool
{
	
	string name;
	string icon_sprite_set;
	string icon_sprite_name;
	float icon_width;
	float icon_height;
	float icon_offset_x;
	float icon_offset_y;
	
	bool selectable = true;
	
	AdvToolScript@ script;
	ToolGroup@ group;
	Button@ toolbar_button;
	
	int shortcut_key = -1;
	bool register_shortcut_key = true;
	
	protected array<IToolEditorLoadListener@> editor_load_listeners;
	protected array<IToolSelectListener@> select_listeners;
	protected array<IToolStepListener@> step_listeners;
	protected array<IToolDrawListener@> draw_listeners;
	protected int num_editor_load_listeners;
	protected int num_select_listeners;
	protected int num_step_listeners;
	protected int num_draw_listeners;
	
	protected bool selected = false;
	
	Tool()
	{
		
	}
	
	Tool(const string name)
	{
		this.name = name;
	}
	
	void create(AdvToolScript@ script, ToolGroup@ group)
	{
		@this.script = script;
		@this.group = group;
	}
	
	void on_init()
	{
		
	}
	
	void build_sprites(message@ msg)
	{
		
	}
	
	Tool@ set_icon(const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		this.icon_sprite_set = sprite_set;
		this.icon_sprite_name = sprite_name;
		this.icon_width = width;
		this.icon_height = height;
		this.icon_offset_x = offset_x;
		this.icon_offset_y = offset_y;
		
		return this;
	}
	
	Tool@ init_shortcut_key(const int shortcut_key, bool register_shortcut_key=true)
	{
		this.shortcut_key = shortcut_key;
		this.register_shortcut_key = register_shortcut_key;
		
		return @this;
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
	
	void deregistedrawep_listener(IToolDrawListener@ listener)
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
		return @this;
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
		group.set_tool(this);
		
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
	
	protected void draw_impl(const float sub_frame)
	{
		
	}
	
	protected void step_impl()
	{
		
	}
	
}