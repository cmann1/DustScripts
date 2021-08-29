#include 'AnchorSide.cpp';
#include '../elements/MoveableDialog.cpp';

class WindowAnchor
{
	
	[hidden]
	string id;
	[hidden]
	AnchorSide side_h;
	[hidden]
	AnchorSide side_v;
	[hidden]
	float distance_h;
	[hidden]
	float distance_v;
	[hidden]
	float size_h;
	[hidden]
	float size_v;
	
	Element@ element;
	EventCallback@ on_moved_delegate;
	bool pending_reposition;
	
	void initialise()
	{
		MoveableDialog@ moveable = cast<MoveableDialog@>(element);
		
		if(@moveable != null)
		{
			@on_moved_delegate = EventCallback(on_moved);
			moveable.move_complete.on(on_moved_delegate);
			moveable.resize_complete.on(on_moved_delegate);
		}
	}
	
	void update(const float width, const float height)
	{
		size_h = width;
		size_v = height;
		
		const float left   = abs(element._x);
		const float right  = abs(width - (element._x + element._width));
		const float top    = abs(element._y);
		const float bottom = abs(height - (element._y + element._height));
		
		if(left <= right)
		{
			side_h = AnchorSide::Left;
			distance_h = left;
		}
		else
		{
			side_h = AnchorSide::Right;
			distance_h = right;
		}
		
		if(top <= bottom)
		{
			side_v = AnchorSide::Top;
			distance_v = top;
		}
		else
		{
			side_v = AnchorSide::Bottom;
			distance_v = bottom;
		}
	}
	
	void reposition(const float width, const float height, const bool relative)
	{
		float x, y;
		
		const float final_distance_h = relative ? (distance_h / size_h) * width  : distance_h;
		const float final_distance_v = relative ? (distance_v / size_v) * height : distance_v;
		
		if(side_h == AnchorSide::Left)
		{
			x = final_distance_h;
		}
		else
		{
			x = width - final_distance_h - element._width;
		}
		
		if(side_v == AnchorSide::Top)
		{
			y = final_distance_v;
		}
		else
		{
			y = height - final_distance_v - element._height;
		}
		
		if(x + element._width > width)
		{
			x = width - element._width;
		}
		if(x < 0)
		{
			x = 0;
		}
		
		if(y + element._height > height)
		{
			y = height - element._height;
		}
		if(y < 0)
		{
			y = 0;
		}
		
		element.x = x;
		element.y = y;
	}
	
	void clear()
	{
		if(@on_moved_delegate == null)
		{
			MoveableDialog@ moveable = cast<MoveableDialog@>(element);
			
			if(@moveable != null)
			{
				moveable.move_complete.off(on_moved_delegate);
			}
		}
		
		@element = null;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void on_moved(EventInfo@ event)
	{
		update(element.ui.region_width, element.ui.region_height);
	}
	
}
