class ExtendedTriggerTool : Tool
{
	
	entity@ clipboard;
	
	ExtendedTriggerTool(AdvToolScript@ script)
	{
		super(script, 'Triggers');
		
		init_shortcut_key(VK::T);
	}
	
	void create(ToolGroup@ group) override
	{
		set_icon('editor', 'triggersicon');
	}
	
	// //////////////////////////////////////////////////////////
	// Tool Callbacks
	// //////////////////////////////////////////////////////////
	
	protected void step_impl() override
	{
		if(script.mouse.left_double_click && script.shift.down)
		{
			duplicate_trigger();
		}
		
		if(!script.ui.is_mouse_active && @script.ui.focus == null && !script.input.is_polling_keyboard())
		{
			if(script.ctrl.down && script.input.key_check_pressed_vk(VK::C))
			{
				entity@ trigger = script.editor.selected_trigger;
				if(@trigger != null)
				{
					@clipboard = trigger;
				}
			}
			if(script.ctrl.down && script.input.key_check_pressed_vk(VK::V))
			{
				entity@ trigger = paste_trigger(script.mouse.x, script.mouse.y);
				if(@trigger != null)
				{
					@script.editor.selected_trigger = trigger;
				}
			}
			if(script.ctrl.down && script.input.key_check_pressed_vk(VK::H))
			{
				entity@ trigger = script.editor.selected_trigger;
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
	
	private entity@ paste_trigger(const float x, const float y)
	{
		if(@clipboard == null)
			return null;
		
		entity@ copy = create_entity(clipboard.type_name());
		copy.set_xy(x, y);
		copy_vars(clipboard, copy);
		script.g.add_entity(copy);
		return copy;
	}
	
}
