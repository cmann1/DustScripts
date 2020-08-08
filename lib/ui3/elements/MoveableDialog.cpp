#include 'Container.cpp';

abstract class MoveableDialog : Container
{
	
	bool draggable;
	bool snap_to_screen;
	bool snap_to_siblings;
	
	Event move;
	Event move_complete;
	
	protected bool busy_dragging;
	protected float drag_offset_x;
	protected float drag_offset_y;
	protected float prev_drag_x;
	protected float prev_drag_y;
	protected bool has_moved;
	
	MoveableDialog(UI@ ui)
	{
		super(ui);
	}
	
	protected void do_drag(LayoutContext@ ctx)
	{
		if(hovered && ui.mouse.primary_press)
		{
			parent.move_to_front(this);
			
			if(draggable && is_mouse_over_draggable_region())
			{
				prev_drag_x = parent.mouse_x;
				prev_drag_y = parent.mouse_y;
				drag_offset_x = ui.mouse.x - x1;
				drag_offset_y = ui.mouse.y - y1;
				busy_dragging = true;
				has_moved = false;
			}
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
	}
	
	protected void snap(LayoutContext@ ctx)
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
	
	protected bool is_mouse_over_draggable_region()
	{
		return false;
	}
	
}