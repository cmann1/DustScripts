#include '../math/math.cpp';
#include '../drawing/common.cpp';
#include '../math/Rect.cpp';
#include 'Container.cpp';
#include 'Direction.cpp';
#include 'Scrollbar.cpp';

class ScrollView : Container
{
	
	Direction direction = Direction::Vertical;
	uint columns = 1;
	float padding = 0;
	float scrollAmount = 33;
	
	Scrollbar@ scrollbar;
	
	float scroll;
	float contentSize;
	float scrollMax;
	
	ScrollView(UI@ ui)
	{
		super(ui);
		@scrollbar = Scrollbar(ui);
	}
	
	void draw(scene@ g, Rect rect) override
	{
		const bool is_mouse_over = ui.mouse_intersects(rect);
		
		if(is_mouse_over)
		{
			 int dir;
			 
			 if(ui.mouse_scroll(dir))
			 {
				 scroll = clamp(scroll + dir * scrollAmount, 0, scrollMax);
			 }
		}
		
		const bool is_horizontal = direction == Direction::Horizontal;
		const float axis2_end = is_horizontal ? rect.y2 : rect.x2;
		const float axis2_size = is_horizontal ? rect.height : rect.width;
		
		float axis1 = 0;
		float axis2 = 0;
		uint axis1_index = 0;
		uint axis2_index = 0;
		float axis1_size = 0;
		
		contentSize = 0;
		
		for(uint i = 0; i < children.size(); i++)
		{
			Element@ child = children[i];
			axis2_index++;
			
			float child_x1, child_y1, child_x2, child_y2;
			
			if(is_horizontal)
			{
				child_x1 = rect.x1 + axis1 - scroll;
				child_y1 = rect.y1 + axis2;
				child_x2 = child_x1 + child.width;
				child_y2 = child_y1 + child.height;
			}
			else
			{
				child_x1 = rect.x1 + axis2;
				child_y1 = rect.y1 + axis1 - scroll;
				child_x2 = child_x1 + child.width;
				child_y2 = child_y1 + child.height;
			}
			
			if(child_x1 >= rect.x1 && child_y1 >= rect.y1 && child_x2 <= rect.x2 && child_y2 <= rect.y2)
			{
				child.draw(g, Rect(child_x1, child_y1, child_x2, child_y2));
			}
			
			float axis2_extents = is_horizontal ? child_y2 : child_x2;
			float child_axis1_size = is_horizontal ? child.width : child.height;
			
			if(child_axis1_size > axis1_size)
			{
				axis1_size = child_axis1_size;
			}
			
			if(axis1 + child_axis1_size > contentSize)
			{
				contentSize = axis1 + child_axis1_size;
			}
			
			if(axis2_index >= columns || axis2_extents > axis2_end)
			{
				axis2_index = 0;
				axis2 = 0;
				axis1 += axis1_size + padding;
				axis1_size = 0;
			}
			else
			{
				axis2 += (is_horizontal ? child.width : child.height) + padding;
			}
		}
		
		scrollMax = max(contentSize - (is_horizontal ? rect.width : rect.height), 0);
		
		float newScroll = scrollbar.draw(g, rect, scroll, scrollMax, direction);
		
		if(newScroll >= 0)
		{
			scroll = clamp(newScroll, 0, scrollMax);
		}
	}
	
}