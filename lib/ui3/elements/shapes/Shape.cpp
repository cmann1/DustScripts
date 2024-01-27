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
			if(pressed || use_parent_hover && @parent != null && parent.pressed || @hover_delegate != null && hover_delegate.pressed)
				return ui.style.selected_highlight_border_clr;
			
			if((hovered || use_parent_hover && @parent != null && parent.hovered || @hover_delegate != null && hover_delegate.hovered) && !ui.is_mouse_active)
				return ui.style.highlight_border_clr;
		}
		
		return _has_colour ? _colour : ui.style.text_clr;
	}
	
	protected void draw_init(float &out x, float &out y, float &out size_x, float &out size_y, float &out draw_scale_x, float &out draw_scale_y)
	{
		draw_scale_x = is_transposed ? this.draw_scale_y : this.draw_scale_x;
		draw_scale_y = is_transposed ? this.draw_scale_x : this.draw_scale_y;
		
		size_x = _graphic_width  * draw_scale_x * 0.5;
		size_y = _graphic_height * draw_scale_y * 0.5;
		x = ui._pixel_floor(x1 + draw_x + size_x);
		y = ui._pixel_floor(y1 + draw_y + size_y);
	}
	
}
