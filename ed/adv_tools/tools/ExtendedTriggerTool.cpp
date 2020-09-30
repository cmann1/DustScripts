class ExtendedTriggerTool : Tool, IToolStepListener
{
	
	ExtendedTriggerTool()
	{
		super('Extended Trigger Tool');
		
		selectable = false;
	}
	
	void on_init(AdvToolScript@ script, ToolGroup@ group) override
	{
		Tool::on_init(script, group);
		
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
		if(script.mouse.left_double_click && script.editor.key_check_gvb(GVB::Shift))
		{
			copy_trigger();
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
	private void copy_trigger()
	{
		entity@ trigger = script.pick_trigger();
			
		if(@trigger == null)
			return;
		
		entity@ copy = create_entity(trigger.type_name());
		copy.set_xy(trigger.x() - 48, trigger.y());
		copy_vars(trigger, copy);
		script.g.add_entity(copy);
	}
	
}