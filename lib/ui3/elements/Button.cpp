class Button : Container
{
	
	bool selectable;
	bool selected;
	
	Element@ _content;
	
	Event on_selected;
	
	protected bool pressed;
	
	Button(UI@ ui, Element@ content)
	{
		super(ui, 'btn');
		
		children_mouse_enabled = false;
		
		@this.content = content;
	}
	
	Element@ content
	{
		get { return @_content; }
		set
		{
			if(@_content == @value)
				return;
			
			if(@_content != null)
			{
				Container::remove_child(_content);
			}
			
			@_content = @value;
			
			if(@_content != null)
			{
				Container::add_child(_content);
			}
		}
	}
	
	bool add_child(Element@ child) override
	{
		puts('add_child not supported on button. Use content instead');
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		puts('remove_child not supported on button. Use content instead');
		return false;
	}
	
	protected bool _remove_child_internal(Element@ child) override
	{
		puts('_remove_child_internal not supported on button. Use content instead');
		return false;
	}
	
	void fit_to_contents(float padding_x=-1, float padding_y=-1)
	{
		if(padding_x < 0)
		{
			padding_x = ui.style.spacing;
		}
		
		if(padding_y < 0)
		{
			padding_y = ui.style.spacing;
		}
		
		if(@content == null)
		{
			width  = padding_x * 2;
			height = padding_y * 2;
			return;
		}
		
		width  = content.width + padding_x * 2;
		height = content.height + padding_y * 2;
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
				ui._event_info.reset(EventType::SELECTED, this);
				on_selected.dispatch(ui._event_info);
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
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(@content != null)
		{
			stack.push(content);
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