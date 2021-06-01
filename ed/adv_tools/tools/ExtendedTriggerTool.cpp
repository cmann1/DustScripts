class ExtendedTriggerTool : Tool, IToolStepListener
{
	
	entity@ clipboard;
	
	ExtendedTriggerTool(AdvToolScript@ script)
	{
		super(script, 'Extended Trigger Tool');
		
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
		
		if(!script.ui.is_mouse_active && @script.ui.focus == null)
		{
			if(script.ctrl && script.editor.key_check_pressed_vk(VK::C))
			{
				entity@ trigger = script.editor.get_selected_trigger();
				if(@trigger != null)
				{
					@clipboard = trigger;
				}
			}
			if(script.ctrl && script.editor.key_check_pressed_vk(VK::V))
			{
				paste_trigger(script.mouse.x, script.mouse.y);
			}
			if(script.ctrl && script.editor.key_check_pressed_vk(VK::H))
			{
				entity@ trigger = script.editor.get_selected_trigger();
				if(@trigger != null && trigger.type_name() == 'text_trigger')
				{
					trigger.vars().get_var('hide').set_bool(!trigger.vars().get_var('hide').get_bool());
				}
			}
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
