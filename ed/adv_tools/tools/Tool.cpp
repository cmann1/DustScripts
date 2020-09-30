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
	
	array<IToolSelectListener@> select_listeners;
	array<IToolStepListener@> step_listeners;
	array<IToolDrawListener@> draw_listeners;
	int num_select_listeners;
	int num_step_listeners;
	int num_draw_listeners;
	
	Tool()
	{
		
	}
	
	Tool(const string name)
	{
		this.name = name;
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
	
	void on_init(AdvToolScript@ script, ToolGroup@ group)
	{
		@this.script = script;
		@this.group = group;
	}
	
	void build_sprites(message@ msg)
	{
		
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
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
	
	void step()
	{
		for(int i = num_step_listeners - 1; i >= 0; i--)
		{
			step_listeners[i].tool_step(this);
		}
	}
	
	void draw(const float sub_frame)
	{
		for(int i = num_draw_listeners - 1; i >= 0; i--)
		{
			draw_listeners[i].tool_draw(this, sub_frame);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	bool on_before_select()
	{
		return true;
	}
	
	void on_select()
	{
		group.set_tool(this);
		
		for(int i = num_select_listeners - 1; i >= 0; i--)
		{
			select_listeners[i].tool_select(this);
		}
	}
	
	void on_deselect()
	{
		for(int i = num_select_listeners - 1; i >= 0; i--)
		{
			select_listeners[i].tool_deselect(this);
		}
	}
	
}