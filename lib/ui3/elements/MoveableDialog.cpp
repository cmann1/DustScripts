#include '../utils/DragMode.cpp';
#include 'Container.cpp';

abstract class MoveableDialog : Container, IStepHandler
{
	
	bool draggable;
	bool drag_anywhere;
	bool snap_to_screen = true;
	bool snap_to_siblings = true;
	float min_width;
	float min_height;
	
	Event move;
	Event move_complete;
	Event resize;
	Event resize_complete;
	
	protected bool _resizable;
	
	protected DragMode drag_mode = Idle;
	protected float drag_offset_x;
	protected float drag_offset_y;
	protected float prev_drag_x;
	protected float prev_drag_y;
	protected bool has_moved;
	protected bool has_resized;
	protected int resize_mode;
	
	protected bool step_subscribed;
	
	MoveableDialog(UI@ ui)
	{
		super(ui);
	}
	
	float x { set override { Element::set_x(ui._pixel_round(value)); } }
	float y { set override { Element::set_y(ui._pixel_round(value)); } }
	
	bool resizable
	{
		set { _resizable = value; }
		get { return _resizable; }
	}
	
	bool ui_step() override
	{
		step_subscribed = false;
		
		if(drag_mode == Idle)
			return step_subscribed;
		
		const float mouse_x = parent.mouse_x;
		const float mouse_y = parent.mouse_y;
		
		if(prev_drag_x != mouse_x || prev_drag_y != mouse_y)
		{
			if(drag_mode == Dragging)
			{
				const float prev_x = _x;
				const float prev_y = _y;
				x = ui._pixel_round(mouse_x - drag_offset_x);
				y = ui._pixel_round(mouse_y - drag_offset_y);
				
				snap();
				
				if(prev_x != _x || prev_y != _y)
				{
					ui._event_info.reset(EventType::MOVE, this);
					ui._event_info.x = _x;
					ui._event_info.y = _y;
					move.dispatch(ui._event_info);
					has_moved = true;
				}
			}
			else if(drag_mode == Resizing)
			{
				update_world_bounds(parent);
				const float prev_x1 = x1;
				const float prev_y1 = y1;
				const float prev_x2 = x2;
				const float prev_y2 = y2;
				
				if((resize_mode & DragMode::Left) != 0)
				{
					x1 = ui._pixel_round(ui.mouse.x - drag_offset_x);
					
					if((x2 - x1) < max(0.0, min_width))
					{
						x1 = ui._pixel_round(x2 - min_width);
					}
				}
				else if((resize_mode & DragMode::Right) != 0)
				{
					x2 = ui._pixel_round(ui.mouse.x - drag_offset_x);
					
					if((x2 - x1) < max(0.0, min_width))
					{
						x2 = ui._pixel_round(x1 + min_width);
					}
				}
				
				if((resize_mode & DragMode::Top) != 0)
				{
					y1 = ui._pixel_round(ui.mouse.y - drag_offset_y);
					
					if((y2 - y1) < max(0.0, min_height))
					{
						y1 = ui._pixel_round(y2 - min_height);
					}
				}
				else if((resize_mode & DragMode::Bottom) != 0)
				{
					y2 = ui._pixel_round(ui.mouse.y - drag_offset_y);
					
					if((y2 - y1) < max(0.0, min_height))
					{
						y2 = ui._pixel_round(y1 + min_height);
					}
				}
				
				if(prev_x1 != x1 || prev_y1 != y1 || prev_x2 != x2 || prev_y2 != y2)
				{
					x = x1 - parent._x;
					y = y1 - parent._y;
					width = x2 - x1;
					height = y2 - y1;
					
					snap();
					
					ui._event_info.reset(EventType::RESIZE, this);
					resize.dispatch(ui._event_info);
					has_resized = true;
				}
			}
			
			prev_drag_x = mouse_x;
			prev_drag_y = mouse_y;
		}
		
		step_subscribed = true;
		
		return step_subscribed;
	}
	
	protected void snap()
	{
		if(!snap_to_screen && !snap_to_siblings)
			return;
		
		float view_x1, view_y1, view_x2, view_y2;
		
		if(@parent != null)
		{
			view_x1 = parent.x1;
			view_y1 = parent.y1;
			view_x2 = parent.x2;
			view_y2 = parent.y2;
		}
		else
		{
			ui.get_region(view_x1, view_y1, view_x2, view_y2);
		}
		
		update_world_bounds(parent);
		
		const float snap_distance = ui.style.snap_distance;
		
		float closest_x1 = -snap_distance - 1;
		float closest_y1 = -snap_distance - 1;
		float closest_x2 = +snap_distance + 1;
		float closest_y2 = +snap_distance + 1;
		
		const bool do_snap_x1 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Left) != 0;
		const bool do_snap_y1 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Top) != 0;
		const bool do_snap_x2 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Right) != 0;
		const bool do_snap_y2 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Bottom) != 0;
		
		if(snap_to_screen)
		{
			if(do_snap_x1 && abs(closest_x1) > abs(view_x1 - x1))
				closest_x1 = view_x1 - x1;
			if(do_snap_y1 && abs(closest_y1) > abs(view_y1 - y1))
				closest_y1 = view_y1 - y1;
			if(do_snap_x2 && abs(closest_x2) > abs(view_x2 - x2))
				closest_x2 = view_x2 - x2;
			if(do_snap_y2 && abs(closest_y2) > abs(view_y2 - y2))
				closest_y2 = view_y2 - y2;
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
					if(do_snap_x1 && abs(closest_x1) > abs(sibling.x2 - x1))
						closest_x1 = sibling.x2 - x1;
					if(do_snap_x2 && abs(closest_x2) > abs(sibling.x1 - x2))
						closest_x2 = sibling.x1 - x2;
				}
				
				if(x2 >= sibling.x1 + snap_threshold && x1 <= sibling.x2 - snap_threshold)
				{
					if(do_snap_y1 && abs(closest_y1) > abs(sibling.y2 - y1))
						closest_y1 = sibling.y2 - y1;
					if(do_snap_y2 && abs(closest_y2) > abs(sibling.y1 - y2))
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
					if(do_snap_x1 && sibling.x1 != snap_x1 && abs(closest_x1) > abs(sibling.x1 - snap_x1))
						closest_x1 = sibling.x1 - snap_x1;
					if(do_snap_x2 && sibling.x2 != snap_x2 && abs(closest_x2) > abs(sibling.x2 - snap_x2))
						closest_x2 = sibling.x2 - snap_x2;
				}
				
				if(snap_x1 == sibling.x2 || snap_x2 == sibling.x1)
				{
					if(do_snap_y1 && sibling.y1 != snap_y1 && abs(closest_y1) > abs(sibling.y1 - snap_y1))
						closest_y1 = sibling.y1 - snap_y1;
					if(do_snap_y2 && sibling.y2 != snap_y2 && abs(closest_y2) > abs(sibling.y2 - snap_y2))
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
			const bool do_snap_x1 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Left) != 0;
			const bool do_snap_x2 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Right) != 0;
			
			if(do_snap_x1)
			{
				x  += snap_x;
				x1 += snap_x;
			}
			
			if(do_snap_x2)
			{
				x2 += snap_x;
			}
			
			_width = _set_width = x2 - x1;
		}
		
		if(abs(snap_y) <= snap_distance)
		{
			const bool do_snap_y1 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Top) != 0;
			const bool do_snap_y2 = resize_mode == DragMode::Idle || (resize_mode & DragMode::Bottom) != 0;
			
			if(do_snap_y1)
			{
				y  += snap_y;
				y1 += snap_y;
			}
			
			if(do_snap_y2)
			{
				y2 += snap_y;
			}
			
			_height = _set_height = y2 - y1;
		}
	}
	
	protected bool is_mouse_over_draggable_region()
	{
		return false;
	}
	
	protected void calculate_resize_flags()
	{
		const float x = ui.mouse.x;
		const float y = ui.mouse.y;
		const float padding = ui.style.spacing;
		resize_mode = DragMode::Idle;
		
		if(x >= x1 && x <= x1 + padding)
			resize_mode |= DragMode::Left;
		else if(x <= x2 && x >= x2 - padding)
			resize_mode |= DragMode::Right;
		
		if(y >= y1 && y <= y1 + padding)
			resize_mode |= DragMode::Top;
		else if(y <= y2 && y >= y2 - padding)
			resize_mode |= DragMode::Bottom;
	}
	
	protected bool is_drag_anywhere()
	{
		return @ui.mouse_over_element == @this;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(EventInfo@ event) override
	{
		parent.move_to_front(this);
		
		if(event.button != ui.primary_button)
			return;
		
		if(drag_mode == Idle && _resizable)
		{
			calculate_resize_flags();
			
			if(resize_mode != DragMode::Idle)
			{
				drag_mode = Resizing;
				has_resized = false;
				drag_offset_x = (resize_mode & DragMode::Left) != 0
					? ui.mouse.x - x1
					: (resize_mode & DragMode::Right) != 0 ? ui.mouse.x - x2
						: 0;
				drag_offset_y = (resize_mode & DragMode::Top) != 0
					? ui.mouse.y - y1
					: (resize_mode & DragMode::Bottom) != 0 ? ui.mouse.y - y2
						: 0;
			}
		}
		
		if(drag_mode == Idle && draggable &&
			((drag_anywhere && is_drag_anywhere()) || is_mouse_over_draggable_region()))
		{
			drag_mode = Dragging;
			has_moved = false;
			drag_offset_x = ui.mouse.x - x1;
			drag_offset_y = ui.mouse.y - y1;
		}
		
		if(drag_mode != Idle)
		{
			prev_drag_x = parent.mouse_x;
			prev_drag_y = parent.mouse_y;
			
			@ui._active_mouse_element = @this;
			step_subscribed = ui._step_subscribe(@this, step_subscribed);
		}
	}
	
	void _mouse_release(EventInfo@ event) override
	{
		if(drag_mode != Idle && event.button == ui.primary_button)
		{
			drag_mode = Idle;
			resize_mode = DragMode::Idle;
			@ui._active_mouse_element = null;
			
			if(has_resized)
			{
				ui._event_info.reset(EventType::RESIZE_COMPLETED, this);
				ui._event_info.x = x;
				ui._event_info.y = y;
				resize_complete.dispatch(ui._event_info);
			}
			
			if(has_moved)
			{
				ui._event_info.reset(EventType::MOVE_COMPLETED, this);
				ui._event_info.x = x;
				ui._event_info.y = y;
				move_complete.dispatch(ui._event_info);
			}
		}
	}
	
}
