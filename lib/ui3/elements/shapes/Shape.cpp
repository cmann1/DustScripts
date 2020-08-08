#include '../../UI.cpp';
#include '../../Style.cpp';
#include '../Graphic.cpp';

abstract class Shape : Graphic
{
	
	bool use_highlight_colour = false;
	bool use_parent_hover = false;
	
	protected bool _use_custom_colour;
	protected uint _colour;
	
	Shape(UI@ ui)
	{
		super(ui);
		
		_use_custom_colour = false;
		
		init();
	}
	
	Shape(UI@ ui, const uint colour)
	{
		super(ui);
		
		_use_custom_colour = true;
		_colour = colour;
		
		init();
	}
	
	string element_type { get const override { return 'Shape'; } }
	
	protected void init()
	{
		
	}
	
	protected uint get_colour()
	{
		if(use_highlight_colour)
		{
			if((pressed || use_parent_hover && @parent != null && parent.pressed))
				return ui.style.selected_highlight_border_clr;
			
			if(hovered || use_parent_hover && @parent != null && parent.hovered)
				return ui.style.highlight_border_clr;
		}
		
		return _use_custom_colour ? _colour : ui.style.text_clr;
	}
	
}