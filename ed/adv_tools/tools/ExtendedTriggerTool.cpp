class ExtendedTriggerTool : Tool, IToolStepListener
{
	
	entity@ clipboard;
	
	ExtendedTriggerTool()
	{
		super('Extended Trigger Tool');
		
		selectable = false;
	}
	
	void on_init() override
	{
		Tool@ tool = group.script.get_tool('Triggers');
		
		if(@tool != null)
		{
			tool.register_step_listener(this);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Tool Callbacks
	// //////////////////////////////////////////////////////////
	
	void tool_step(Tool@ tool) override
	{
		if(script.mouse.left_double_click && script.shift)
		{
			duplicate_trigger();
		}
		
		if(script.ctrl && script.editor.key_check_pressed_vk(VK::V))
		{
			paste_trigger(script.mouse.x, script.mouse.y);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
	private void duplicate_trigger()
	{
		entity@ trigger = script.pick_trigger();
			
		if(@trigger == null)
			return;
		
		@clipboard = trigger;
		
		paste_trigger(trigger.x() - 48, trigger.y());
	}
	
	private void paste_trigger(const float x, const float y)
	{
		if(@clipboard == null)
			return;
		
		entity@ copy = create_entity(clipboard.type_name());
		copy.set_xy(x, y);
		copy_vars(clipboard, copy);
		script.g.add_entity(copy);
	}
	
}