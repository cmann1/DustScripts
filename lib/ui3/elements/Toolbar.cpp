#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Orientation.cpp';
#include '../layouts/FlowLayout.cpp';
#include 'Container.cpp';

class Toolbar : Container
{
	
	Orientation orientation;
	
	Toolbar(UI@ ui, Orientation orientation)
	{
		super(ui, 'tbar');
		
		this.orientation = orientation;
		@_layout = ui._toolbar_flow_layout;
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		// TODO: Testing
		if(ui.mouse.left_down)
		{
			x = ui.mouse_x;
			y = ui.mouse_y;
			// TODO: Only do when startign drag
			ui.move_to_front(this);
		}
		
		Element::do_layout(parent_x, parent_y);
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
		style.draw_dialog_element(this);
		
		Container::draw(style, sub_frame);
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
}