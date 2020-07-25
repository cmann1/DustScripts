#include '../UI.cpp';
#include '../Style.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include 'Container.cpp';

class Toolbar : Container
{
	
	bool auto_fit;
	float max_size;
	
	protected FlowLayout@ flow_layout;
	
	// TODO: auto_fit should only control wether the main axis is fit
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
					ui._toolbar_flow_layout.wrap);
			}
			
			return @flow_layout;
		}
		set { @flow_layout = @value; }
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		// TODO: Testing
		if(ui.mouse.middle_down)
		{
			x = ui.mouse_x;
			y = ui.mouse_y;
			// TODO: Only do when starting drag
			ui.move_to_front(this);
		}
		
		Element::do_layout(parent_x, parent_y);
		
		float out_x1, out_y1, out_x2, out_y2;
		
		FlowLayout@ flow_layout = @this.flow_layout != null ? @this.flow_layout : @ui._toolbar_flow_layout;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		
		float x = 0;
		float y = 0;
		float width  = this.width;
		float height = this.height;
		
		// Temporarily expand to max width and height to allow the max number of items
		// Then shrink to fit after laying out children
		if(auto_fit)
		{
			width  = view_x2 - view_x1;
			height = view_y2 - view_y1;
			
			if(max_size > 0)
			{
				if(flow_layout.is_horizontal)
					width = max_size;
				else
					height = max_size;
			}
		}
		
		flow_layout.do_layout(ui, @children,
			x, y, x + width, y + height,
			out_x1, out_y1, out_x2, out_y2);
		
		if(auto_fit)
		{
			this.width  = out_x2 - out_x1 + x1;
			this.height = out_y2 - out_y1 + y1;
			this.x2 = x + this.width;
			this.y2 = y + this.height;
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