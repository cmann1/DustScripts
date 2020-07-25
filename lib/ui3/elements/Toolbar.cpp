#include '../UI.cpp';
#include '../Style.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include 'Container.cpp';

class Toolbar : Container
{
	
	bool auto_fit;
	float max_size;
	
	protected FlowLayout@ flow_layout;
	
	Toolbar(UI@ ui, bool auto_fit=true, float max_size=0)
	{
		super(ui, 'tbar');
		
		this.auto_fit = auto_fit;
		this.max_size = max_size;
	}
	
	Layout@ layout
	{
		get override { return @flow_layout; }
		set override {  }
	}
	
	FlowLayout@ flow_layout
	{
		get
		{
			if(@flow_layout == null)
			{
				@flow_layout = FlowLayout(ui,
					ui._toolbar_flow_layout.direction,
					ui._toolbar_flow_layout.justify,
					ui._toolbar_flow_layout.align,
					ui._toolbar_flow_layout.wrap, ui._toolbar_flow_layout.fit);
			}
			
			return @flow_layout;
		}
		set { if(@value != null) @flow_layout = @value; }
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		// TODO: Testing
		if(ui.mouse.middle_down && flow_layout._db)
		{
			x = ui.mouse_x;
			y = ui.mouse_y;
			// TODO: Only do when starting drag
			ui.move_to_front(this);
		}
		
		Element::do_layout(parent_x, parent_y);
		
		float out_x1, out_y1, out_x2, out_y2;
		
		FlowLayout@ flow_layout = @this.flow_layout != null ? @this.flow_layout : @ui._toolbar_flow_layout;
		flow_layout.fit = FlowFit::Both;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		
		const bool is_horizontal = flow_layout.is_horizontal;
		
		float x = 0;
		float y = 0;
		float width  = this.width - x;
		float height = this.height - y;
		
		// Temporarily expand to max width and height to allow the max number of items
		// Then shrink to fit after laying out children
		if(auto_fit)
		{
			if(is_horizontal)
				width = max_size > 0 ? max_size : view_x2 - view_x1 - x + 1;
			else if(is_horizontal)
				height = max_size > 0 ? max_size : view_y2 - view_y1 - y + 1;
		}
		
		flow_layout.do_layout(@children,
			x, y, x + width, y + height,
			out_x1, out_y1, out_x2, out_y2);
		
		if(auto_fit || is_horizontal)
		{
			this.height = y + out_y2 - out_y1;
			this.y2 = this.y1 + this.height;
		}
		
		if(auto_fit || !is_horizontal)
		{
			this.width  = x + out_x2 - out_x1;
			this.x2 = this.x1 + this.width;
		}
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