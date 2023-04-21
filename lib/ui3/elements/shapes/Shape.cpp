#include '../../UI.cpp';
#include '../../Style.cpp';
#include '../Graphic.cpp';

abstract class Shape : Graphic
{
	
	bool use_highlight_colour = false;
	bool use_parent_hover = false;
	Element@ hover_delegate;
	
	Shape(UI@ ui)
	{
		super(ui);
		
		init();
	}
	
	Shape(UI@ ui, const uint colour)
	{
		super(ui);
		
		_has_colour = true;
		_colour = colour;
		
		init();
	}
	
	string element_type { get const override { return 'Shape'; } }
	
	protected void init()
	{
		
	}
	
	protected uint get_draw_colour() override
	{
		if(use_highlight_colour)
		{
			if((pressed || use_parent_hover && @parent != null && parent.pressed || @hover_delegate != null && hover_delegate.pressed))
				return ui.style.selected_highlight_border_clr;
			
			if(hovered || use_parent_hover && @parent != null && parent.hovered || @hover_delegate != null && hover_delegate.hovered)
				return ui.style.highlight_border_clr;
		}
		
		return _has_colour ? _colour : ui.style.text_clr;
	}
	
}
