class Tooltip : SingleContainer
{
	
	Element@ target;
	
	private float fade;
	private float fade_max;
	
	Tooltip(UI@ ui)
	{
		super(ui, null, 'ttip');
		
		mouse_enabled = false;
		children_mouse_enabled = false;
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
//		style.draw_interactive_element(this, hovered, selectable && selected, disabled);
		
		if(@content != null)
		{
			if(disabled)
				style.disable_alpha();
			
			content.draw(style, sub_frame);
			
			if(disabled)
				style.restore_alpha();
		}
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
}