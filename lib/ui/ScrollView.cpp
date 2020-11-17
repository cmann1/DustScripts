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
	float scroll_amount = 33;
	
	Scrollbar@ scrollbar;
	
	float scroll;
	float content_size;
	float scroll_max;
	
	int scroll_into_view = -1;
	
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
			 
			 if(ui.mouse.scrolled(dir))
			 {
				 scroll = clamp(scroll + dir * scroll_amount, 0, scroll_max);
			 }
		}
		
		float new_scroll = -1;
		
		const bool is_horizontal = direction == Direction::Horizontal;
		const float axis1_size = is_horizontal ? rect.width : rect.height;
		const float axis2_size = is_horizontal ? rect.height : rect.width;
		
		float axis1 = 0;
		float axis2 = 0;
		uint axis1_index = 0;
		uint axis2_index = 0;
		float current_axis1_size = 0;
		
		content_size = 0;
		
		for(uint i = 0; i < children.size(); i++)
		{
			Element@ child = children[i];
			axis2_index++;
			
			float child_x1, child_y1, child_x2, child_y2;
			float child_axis1_size = is_horizontal ? child.width : child.height;
			float child_axis2_size = is_horizontal ? child.height : child.width;
			
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
			else if(scroll_into_view == int(i))
			{
				if(axis1 - scroll < 0)
				{
					new_scroll = axis1;
				}
				else if(axis1 - scroll + child_axis1_size > axis1_size)
				{
					new_scroll = axis1 + child_axis1_size - axis1_size;
				}
			}
			
			if(child_axis1_size > current_axis1_size)
			{
				current_axis1_size = child_axis1_size;
			}
			
			if(axis1 + child_axis1_size > content_size)
			{
				content_size = axis1 + child_axis1_size;
			}
			
			axis2 += child_axis2_size + padding;
			
			if(axis2_index >= columns || axis2 > axis2_size)
			{
				axis2_index = 0;
				axis2 = 0;
				axis1 += current_axis1_size + padding;
				current_axis1_size = 0;
			}
		}
		
		scroll_max = max(content_size - axis1_size, 0.0);
		
		float scrollbarScroll = scrollbar.draw(g, rect, scroll, scroll_max, direction);
		
		if(new_scroll == -1)
		{
			new_scroll = scrollbarScroll;
		}
		
		if(new_scroll >= 0)
		{
			scroll = clamp(new_scroll, 0, scroll_max);
		}
		
		if(scroll_into_view >= 0)
		{
			scroll_into_view = -1;
		}
	}
	
}