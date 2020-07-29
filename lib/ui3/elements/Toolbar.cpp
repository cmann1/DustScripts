#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include '../utils/ButtonGroup.cpp';
#include 'Container.cpp';
#include 'Button.cpp';
#include 'Divider.cpp';

class Toolbar : Container, IOrientationParent
{
	
	bool draggable;
	bool auto_fit;
	float max_size;
	
	bool snap_to_screen;
	bool snap_to_siblings;
	
	Event move;
	Event move_complete;
	
	ButtonGroup@ button_group;
	
	protected FlowLayout@ _flow_layout;
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
			_validate_layout = true;
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
		
		if(hovered && ui.mouse.primary_press)
		{
			ui.move_to_front(this);
		}
		
		if(draggable && hovered && ui.mouse.primary_press && is_mouse_over_gripper())
		{
			drag_offset_x = ui.mouse.x - x1;
			drag_offset_y = ui.mouse.y - y1;
			prev_drag_x = parent.mouse_x;
			prev_drag_y = parent.mouse_y;
			busy_dragging = true;
			has_moved = false;
		}
		
		if(busy_dragging)
		{
			const float mouse_x = parent.mouse_x;
			const float mouse_y = parent.mouse_y;
			
			if(prev_drag_x != mouse_x || prev_drag_y != mouse_y || true)
			{
				x = mouse_x - drag_offset_x;
				y = mouse_y - drag_offset_y;
				prev_drag_x = mouse_x;
				prev_drag_y = mouse_y;
				
				snap(ctx);
				
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
		
		if(_validate_layout)
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
			
			_validate_layout = false;
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
	
	private void snap(LayoutContext@ ctx)
	{
		if(!snap_to_screen && !snap_to_siblings)
			return;
		
		update_world_bounds(parent);
		
		const float snap_distance = ui.style.snap_distance;
		
		float closest_x1 = -snap_distance - 1;
		float closest_y1 = -snap_distance - 1;
		float closest_x2 = +snap_distance + 1;
		float closest_y2 = +snap_distance + 1;
		
		if(snap_to_screen)
		{
			if(abs(closest_x1) > abs(ctx.x1 - x1))
				closest_x1 = ctx.x1 - x1;
			if(abs(closest_y1) > abs(ctx.y1 - y1))
				closest_y1 = ctx.y1 - y1;
			if(abs(closest_x2) > abs(ctx.x2 - x2))
				closest_x2 = ctx.x2 - x2;
			if(abs(closest_y2) > abs(ctx.y2 - y2))
				closest_y2 = ctx.y2 - y2;
		}
		
		array<Element@>@ potential_snaps = @ui._element_array;
		int potential_snaps_index = 0;
		int potential_snaps_size = potential_snaps.length();
		
		if(snap_to_siblings && @parent != null)
		{
			const float snap_threshold = ui.style.snap_threshold;
			
			array<Element@>@ siblings = @parent.get_children();
			
			for(int i = int(siblings.length() - 1); i >= 0; i--)
			{
				Element@ sibling = @siblings[i];
				
				if(!sibling.visible || @sibling == @this)
					continue;
				
				if(
					abs(y2 - sibling.y1) <= snap_distance || abs(y1 - sibling.y2) <= snap_distance ||
					abs(x2 - sibling.x1) <= snap_distance || abs(x1 - sibling.x2) <= snap_distance)
				{
					if(potential_snaps_index == potential_snaps_size)
						potential_snaps.resize(potential_snaps_size += 16);
					
					@potential_snaps[potential_snaps_index++] = sibling;
				}
				
				if(y2 >= sibling.y1 + snap_threshold && y1 <= sibling.y2 - snap_threshold)
				{
					if(abs(closest_x1) > abs(sibling.x2 - x1))
						closest_x1 = sibling.x2 - x1;
					if(abs(closest_x2) > abs(sibling.x1 - x2))
						closest_x2 = sibling.x1 - x2;
				}
				
				if(x2 >= sibling.x1 + snap_threshold && x1 <= sibling.x2 - snap_threshold)
				{
					if(abs(closest_y1) > abs(sibling.y2 - y1))
						closest_y1 = sibling.y2 - y1;
					if(abs(closest_y2) > abs(sibling.y1 - y2))
						closest_y2 = sibling.y1 - y2;
				}
			}
		}
		
		float snap_x, snap_y, snap_x1, snap_y1, snap_x2, snap_y2;
		
		calculate_snap(
			snap_distance,
			closest_x1, closest_y1, closest_x2, closest_y2,
			snap_x, snap_y, snap_x1, snap_y1, snap_x2, snap_y2);
		
		if(snap_to_siblings)
		{
			for(int i = potential_snaps_index - 1; i >= 0; i--)
			{
				Element@ sibling = @potential_snaps[i];
				
				if(snap_y1 == sibling.y2 || snap_y2 == sibling.y1)
				{
					if(sibling.x1 != snap_x1 && abs(closest_x1) > abs(sibling.x1 - snap_x1))
						closest_x1 = sibling.x1 - snap_x1;
					if(sibling.x2 != snap_x2 && abs(closest_x2) > abs(sibling.x2 - snap_x2))
						closest_x2 = sibling.x2 - snap_x2;
				}
				
				if(snap_x1 == sibling.x2 || snap_x2 == sibling.x1)
				{
					if(sibling.y1 != snap_y1 && abs(closest_y1) > abs(sibling.y1 - snap_y1))
						closest_y1 = sibling.y1 - snap_y1;
					if(sibling.y2 != snap_y2 && abs(closest_y2) > abs(sibling.y2 - snap_y2))
						closest_y2 = sibling.y2 - snap_y2;
				}
			}
			
			calculate_snap(
				snap_distance,
				closest_x1, closest_y1, closest_x2, closest_y2,
				snap_x, snap_y, snap_x1, snap_y1, snap_x2, snap_y2);
		}
		
		x  = snap_x;
		x1 = snap_x1;
		x2 = snap_x2;
		y  = snap_y;
		y1 = snap_y1;
		y2 = snap_y2;
	}
	
	protected void calculate_snap(
		const float snap_distance,
		const float closest_x1, const float closest_y1, const float closest_x2, const float closest_y2,
		float &out x, float &out y, float &out x1, float &out y1, float &out x2, float &out y2)
	{
		float snap_x = (abs(closest_x1) < closest_x2 ? closest_x1 : closest_x2);
		float snap_y = (abs(closest_y1) < closest_y2 ? closest_y1 : closest_y2);
		
		x = this._x;
		y = this._y;
		x1 = this.x1;
		y1 = this.y1;
		x2 = this.x2;
		y2 = this.y2;
		
		if(abs(snap_x) <= snap_distance)
		{
			x  += snap_x;
			x1 += snap_x;
			x2 += snap_x;
		}
		
		if(abs(snap_y) <= snap_distance)
		{
			y  += snap_y;
			y1 += snap_y;
			y2 += snap_y;
		}
	}
	
	private bool is_mouse_over_gripper()
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