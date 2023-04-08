class ExtendedPropTool : Tool, IToolStepListener, IToolDrawListener
{
	
	private PropTool@ prop_tool;
	private PropData@ pick_data;
	
	ExtendedPropTool(AdvToolScript@ script)
	{
		super(script, 'Extended Prop Tool');
		
		selectable = false;
	}
	
	void on_init() override
	{
		@prop_tool = cast<PropTool@>(script.get_tool('Prop Tool'));
		Tool@ base_prop_tool = script.get_tool('Props');
		
		if(@base_prop_tool != null && @prop_tool != null)
		{
			base_prop_tool.register_step_listener(this);
			base_prop_tool.register_draw_listener(this);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Tool Callbacks
	// //////////////////////////////////////////////////////////
	
	void tool_step(Tool@ tool) override
	{
		if(script.alt.down && script.mouse.middle_down && script.mouse_in_scene && !script.space.down && !script.handles.mouse_over)
		{
			script.input.key_clear_gvb(GVB::MiddleClick);
			@pick_data = null;
			prop_tool.clear_hovered_props();
			prop_tool.pick_props();
			prop_tool.clear_highlighted_props();
			
			if(prop_tool.highlighted_props_list_count > 0)
			{
				@pick_data = @prop_tool.highlighted_props_list[0];
				prop@ p = pick_data.prop;
				script.editor.set_prop(p.prop_set(), p.prop_group(), p.prop_index(), p.palette());
			}
			else
			{
				@pick_data = null;
			}
		}
		else
		{
			@pick_data = null;
		}
	}
	
	void tool_draw(Tool@ tool, const float sub_frame) override
	{
		if(script.alt.down && script.mouse.middle_down)
		{
			if(@pick_data != null)
			{
				pick_data.draw(PropToolHighlight::Both);
			}
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
}
