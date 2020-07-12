#include '../Element.cpp';
#include '../Align.cpp';

class GroupName : Element
{
	
	string name;
	int id;
	PropSelector@ propSelector;
	
	bool selected;
	
	GroupName(UI@ ui, PropSelector@ propSelector, string name, int id = -1)
	{
		super(ui);
		
		@this.propSelector = propSelector;
		this.name = name;
		this.id = id;
		height = 28;
	}
	
	void draw(scene@ g, Rect rect) override
	{
		const bool is_mouse_over = ui.mouse_intersects(rect);
		
		g.draw_rectangle_hud(22, 22, rect.x1, rect.y1, rect.x2, rect.y1 + height, 0, is_mouse_over ? ui.hover_bg_colour : ui.bg_colour);
		ui.text(name, rect.x1 + ui.padding, rect.y1 + height * 0.5, Align::Left, Align::Middle);
		
		if(selected)
		{
			ui.highlight(rect);
		}
		
		if(is_mouse_over && ui.mouse.left_press)
		{
			propSelector.select_group(this);
		}
	}
	
}