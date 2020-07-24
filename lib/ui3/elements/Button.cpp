class Button : SingleContainer
{
	
	bool selectable;
	bool selected;
	
	Event select;
	
	protected bool pressed;
	
	Button(UI@ ui, Element@ content)
	{
		super(ui, content, 'btn');
		
		children_mouse_enabled = false;
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		Element::do_layout(parent_x, parent_y);
		
		if(hovered && ui.mouse.primary_press)
		{
			pressed = true;
		}
		else if(pressed)
		{
			if(selectable && hovered && ui.mouse.primary_release)
			{
				selected = !selected;
				ui._event_info.reset(EventType::SELECT, this);
				select.dispatch(ui._event_info);
			}
			
			if(!ui.mouse.primary_down)
			{
				pressed = false;
			}
		}
		
		if(@content != null)
		{
			content.x = (width  - content.width)  * 0.5;
			content.y = (height - content.height) * 0.5;
			
			if(pressed)
			{
				content.x += ui.style.button_pressed_icon_offset;
				content.y += ui.style.button_pressed_icon_offset;
			}
		}
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
		style.draw_interactive_element(this, hovered, selectable && selected, disabled);
		
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