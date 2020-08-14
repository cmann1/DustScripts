#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include '../utils/ButtonGroup.cpp';
#include 'MoveableDialog.cpp';
#include 'Button.cpp';
#include 'Divider.cpp';

class Toolbar : MoveableDialog, IOrientationParent
{
	
	bool auto_fit;
	float max_size;
	
	ButtonGroup@ button_group;
	
	protected FlowLayout@ _flow_layout;
	
	Toolbar(UI@ ui, bool draggable = true, bool auto_fit=true, float max_size=0)
	{
		super(ui);
		
		this.draggable = draggable;
		this.auto_fit = auto_fit;
		this.max_size = max_size;
	}
	
	string element_type { get const override { return 'Toolbar'; } }
	
	Layout@ layout
	{
		get override { return @flow_layout; }
		set override {  }
	}
	
	FlowLayout@ flow_layout
	{
		get
		{
			if(@_flow_layout == null)
			{
				@_flow_layout = FlowLayout(ui,
					ui._toolbar_flow_layout.direction,
					ui._toolbar_flow_layout.justify,
					ui._toolbar_flow_layout.align,
					ui._toolbar_flow_layout.wrap, ui._toolbar_flow_layout.fit);
			}
			
			return @_flow_layout;
		}
		set
		{
			if(@value == null || @_flow_layout == @value)
				return;
			
			@_flow_layout = @value;
			validate_layout = true;
		}
	}
	
	Orientation orientation
	{
		get const
		{
			FlowLayout@ flow_layout = @this._flow_layout != null ? @this._flow_layout : @ui._toolbar_flow_layout;
			
			return flow_layout.direction == FlowDirection::Row || flow_layout.direction == FlowDirection::RowReverse
				? Orientation::Horizontal
				: Orientation::Vertical;
		}
	}
	
	// Add methods
	
	Image@ add_image(const string sprite_text, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		Image@ image = Image(ui, sprite_text, sprite_name, width, height, offset_x, offset_y);
		add_child(image);
		return image;
	}
	
	Button@ add_button(Element@ content)
	{
		Button@ button = Button(ui, content);
		@button.group = button_group;
		add_child(button);
		return button;
	}
	
	Button@ add_button(const string text)
	{
		Button@ button = Button(ui, text);
		@button.group = button_group;
		add_child(button);
		return button;
	}
	
	Button@ add_button(const string sprite_text, const string sprite_name,
		const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		Button@ button = Button(ui, sprite_text, sprite_name, width, height, offset_x, offset_y);
		@button.group = button_group;
		add_child(button);
		return button;
	}
	
	Divider@ add_divider()
	{
		Divider@ divider = Divider(ui, this);
		add_child(divider);
		return divider;
	}
	
	// /////////////////////////////////////
	// Internal/Util
	
	void _do_layout(LayoutContext@ ctx) override
	{
		FlowLayout@ flow_layout = @this._flow_layout != null ? @this._flow_layout : @ui._toolbar_flow_layout;
		flow_layout.fit = FlowFit::Both;
		
		const bool is_horizontal = flow_layout.is_horizontal;
		
		float x = 0;
		float y = 0;
		float width  = this._width;
		float height = this._height;
		
		const float gripper_space = draggable ? ui.style.spacing + ui.style.gripper_required_space : 0;
		const float hor_gripper_space = is_horizontal ? gripper_space : 0;
		const float ver_gripper_space = is_horizontal ? 0 : gripper_space;
		
		if(draggable)
		{
			switch(flow_layout.direction)
			{
				case FlowDirection::Row:
					x += gripper_space;
					width -= gripper_space;
					break;
				case FlowDirection::RowReverse:
					width -= gripper_space;
					break;
				case FlowDirection::Column:
					y += gripper_space;
					height -= gripper_space;
					break;
				case FlowDirection::ColumnReverse:
					height -= gripper_space;
					break;
			}
		}
		
		if(validate_layout)
		{
			float out_x1, out_y1, out_x2, out_y2;
			
			// Temporarily expand to max width and height to allow the max number of items
			// Then shrink to fit after laying out children
			if(auto_fit)
			{
				if(is_horizontal)
					width = max_size > 0 ? max_size : ctx.x2 - ctx.x1 - x + 1;
				else
					height = max_size > 0 ? max_size : ctx.y2 - ctx.y1 - y + 1;
			}
			
			flow_layout.do_layout(@children,
				x, y, x + width, y + height,
				out_x1, out_y1, out_x2, out_y2);
			
			if(auto_fit || is_horizontal)
			{
				this.height = out_y2 - out_y1 + ver_gripper_space;
			}
			
			if(auto_fit || !is_horizontal)
			{
				this.width  = out_x2 - out_x1 + hor_gripper_space;
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_dialog_element(this);
		
		if(draggable)
		{
			float gripper_x1, gripper_y1, gripper_x2, gripper_y2;
			get_gripper_bounds(gripper_x1, gripper_y1, gripper_x2, gripper_y2, true);
			
			if(flow_layout.is_horizontal)
			{
				style.draw_gripper(Orientation::Horizontal, gripper_x1, gripper_y1, gripper_y2);
			}
			else
			{
				style.draw_gripper(Orientation::Vertical, gripper_y1, gripper_x1, gripper_x2);
			}
		}
	}
	
	protected bool is_mouse_over_draggable_region() override
	{
		float gripper_x1, gripper_y1, gripper_x2, gripper_y2;
		get_gripper_bounds(gripper_x1, gripper_y1, gripper_x2, gripper_y2, false);
		
		return ui.mouse.x >= gripper_x1 && ui.mouse.x <= gripper_x2 && ui.mouse.y >= gripper_y1 && ui.mouse.y <= gripper_y2;
	}
	
	private void get_gripper_bounds(float &out gripper_x1, float &out gripper_y1, float &out gripper_x2, float &out gripper_y2, const bool inner)
	{
		const float spacing = ui.style.spacing;
		const float gripper_space = spacing + ui.style.gripper_required_space + spacing;
		const bool is_horizontal = flow_layout.is_horizontal;
		
		if(is_horizontal)
		{
			gripper_y1 = y1;
			gripper_y2 = y2;
			
			if(flow_layout.direction == FlowDirection::Row)
			{
				gripper_x1 = x1;
				gripper_x2 = x1 + gripper_space;
			}
			else
			{
				gripper_x1 = x2 - gripper_space;
				gripper_x2 = x2;
			}
		}
		else
		{
			gripper_x1 = x1;
			gripper_x2 = x2;
			
			if(flow_layout.direction == FlowDirection::Column)
			{
				gripper_y1 = y1;
				gripper_y2 = y1 + gripper_space;
			}
			else
			{
				gripper_y1 = y2 - gripper_space;
				gripper_y2 = y2;
			}
		}
		
		if(inner)
		{
			const bool is_reversed = flow_layout.direction == FlowDirection::RowReverse || flow_layout.direction == FlowDirection::ColumnReverse;
			
			gripper_x1 += spacing;
			gripper_y1 += spacing;
			gripper_x2 -= spacing;
			gripper_y2 -= spacing;
		}
	}
	
}