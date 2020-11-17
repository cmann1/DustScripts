#include 'UI.cpp';
#include 'Element.cpp';

class Button : Element
{
	
	Element@ icon;
	ButtonClickHandler@ click_listener;
	
	bool pressed;
	
	Button(UI@ ui, Element@ icon)
	{
		super(ui);
		
		update_icon(icon);
	}
	
	void update_icon(Element@ icon)
	{
		@this.icon = icon;
		
		width = icon.width + ui.padding * 2;
		height = icon.height + ui.padding * 2;
	}
	
	/*
	 * Sets the height and attempts to match padding horizontally
	 */
	void fit_to_height(float height)
	{
		this.height = height;
		const float padding = max(height - icon.height, 0.0);
		width = icon.width + padding;
	}
	
	void draw(scene@ g, Rect rect)
	{
		Rect r(rect.x1, rect.y1, rect.x1 + width, rect.y1 + height);
		
		const bool is_mouse_over = ui.mouse_intersects(r);
		
		if(pressed && !ui.mouse.left_down)
		{
			// Clicked!
			if(is_mouse_over && @click_listener != null)
			{
				click_listener.on_button_click(this);
			}
			
			pressed = false;
		}
		
		g.draw_rectangle_hud(22, 22, r.x1, r.y1, r.x2, r.y2, 0, is_mouse_over || pressed ? ui.highlight_colour : ui.bg_colour);
		ui.border(r, is_mouse_over || pressed);
		
		if(is_mouse_over)
		{
			if(ui.mouse.left_press)
			{
				pressed = true;
			}
			
			if(ui.mouse.left_down)
			{
				r.offset(1, 1);
			}
		}
		
		r.expand(-ui.padding);
		icon.draw(g, r);
	}
	
}

interface ButtonClickHandler
{
	
	void on_button_click(Button@ button);
	
}