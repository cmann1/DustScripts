#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include '../utils/ButtonGroup.cpp';
#include 'Container.cpp';

class Toolbar : Container
{
	
	bool draggable;
	bool auto_fit;
	float max_size;
	
	Event move;
	Event move_complete;
	
	ButtonGroup@ button_group;
	
	protected FlowLayout@ flow_layout;
	protected bool busy_dragging;
	protected float drag_offset_x;
	protected float drag_offset_y;
	protected float prev_drag_x;
	protected float prev_drag_y;
	protected bool has_moved;
	
	Toolbar(UI@ ui, bool draggable = true, bool auto_fit=true, float max_size=0)
	{
		super(ui, 'tbar');
		
		this.draggable = draggable;
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
	
	// Add methods
	
	Image@ add_image(const string sprite_text, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=-0.5, const float offset_y=-0.5)
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
		const float width=-1, const float height=-1, const float offset_x=-0.5, const float offset_y=-0.5)
	{
		Button@ button = Button(ui, sprite_text, sprite_name, width, height, offset_x, offset_y);
		@button.group = button_group;
		add_child(button);
		return button;
	}
	
	Element@ add_divider()
	{
		puts('Not implemented');
		return null;
	}
	
	// /////////////////////////////////////
	// Internal/Util
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		const bool is_horizontal = flow_layout.is_horizontal;
		is_mouse_over_gripper();
		if(draggable && ui.mouse.primary_press && is_mouse_over_gripper())
		{
			drag_offset_x = ui.mouse.x - x1;
			drag_offset_y = ui.mouse.y - y1;
			prev_drag_x = parent.mouse_x;
			prev_drag_y = parent.mouse_y;
			busy_dragging = true;
			has_moved = false;
			ui.move_to_front(this);
		}
		
		if(busy_dragging)
		{
			const float mouse_x = parent.mouse_x;
			const float mouse_y = parent.mouse_y;
			
			if(prev_drag_x != mouse_x || prev_drag_y != mouse_y)
			{
				x = mouse_x - drag_offset_x;
				y = mouse_y - drag_offset_y;
				prev_drag_x = mouse_x;
				prev_drag_y = mouse_y;
				
				ui._event_info.reset(EventType::MOVE, this);
				ui._event_info.x = x;
				ui._event_info.y = y;
				move.dispatch(ui._event_info);
				
				has_moved = true;
			}
			
			if(!ui.mouse.primary_down)
			{
				busy_dragging = false;
				
				if(has_moved)
				{
					ui._event_info.reset(EventType::MOVE_COMPLETED, this);
					ui._event_info.x = x;
					ui._event_info.y = y;
					move_complete.dispatch(ui._event_info);
				}
			}
		}
		
		Element::do_layout(parent_x, parent_y);
		
		float out_x1, out_y1, out_x2, out_y2;
		
		FlowLayout@ flow_layout = @this.flow_layout != null ? @this.flow_layout : @ui._toolbar_flow_layout;
		flow_layout.fit = FlowFit::Both;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		
		float x =  is_horizontal && draggable ? ui.style.spacing + ui.style.gripper_required_space : 0;
		float y = !is_horizontal && draggable ? ui.style.spacing + ui.style.gripper_required_space : 0;
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
		
		Container::draw(style, sub_frame);
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
	bool is_mouse_over_gripper()
	{
		float gripper_x1, gripper_y1, gripper_x2, gripper_y2;
		get_gripper_bounds(gripper_x1, gripper_y1, gripper_x2, gripper_y2, false);
		
		return ui.mouse.x >= gripper_x1 && ui.mouse.x <= gripper_x2 && ui.mouse.y >= gripper_y1 && ui.mouse.y <= gripper_y2;
	}
	
	void get_gripper_bounds(float &out gripper_x1, float &out gripper_y1, float &out gripper_x2, float &out gripper_y2, const bool inner)
	{
		gripper_x1 = x1;
		gripper_y1 = y1;
		
		if(flow_layout.is_horizontal)
		{
			gripper_x2 = x1 + ui.style.spacing * 2 + ui.style.gripper_required_space;
			gripper_y2 = y2;
		}
		else
		{
			gripper_x2 = x2;
			gripper_y2 = y1 + ui.style.spacing * 2 + ui.style.gripper_required_space;
		}
		
		if(inner)
		{
			gripper_x1 += ui.style.spacing;
			gripper_y1 += ui.style.spacing;
			gripper_x2 -= ui.style.spacing;
			gripper_y2 -= ui.style.spacing;
		}
	}
	
}