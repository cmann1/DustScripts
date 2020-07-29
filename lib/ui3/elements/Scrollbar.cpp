#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Orientation.cpp';
#include '../events/Event.cpp';
#include 'Container.cpp';

class Scrollbar : Element
{
	
	Orientation orientation;
	float scroll_visible;
	float scroll_min;
	float scroll_max;
	float position;
	bool flexible_thumb_size = true;
	
	Container@ container;
	bool scroll_container_at_origin = true;
	
	Event scroll;
	
	protected float scroll_width;
	protected float scroll_range;
	protected float position_max;
	protected float thumb_position;
	protected float thumb_size;
	
	protected bool mouse_over_thumb;
	protected bool dragging_thumb;
	protected float drag_thumb_offset;
	
	protected float previous_position;
	
	Scrollbar(UI@ ui, Orientation orientation)
	{
		super(ui, 'sbar');
		
		this.orientation = orientation;
		
		if(orientation == Orientation::Vertical)
		{
			_width = ui.style.default_scrollbar_size;
		}
		else
		{
			_height = ui.style.default_scrollbar_size;
		}
	}
	
	bool busy_dragging { get const { return dragging_thumb; } }
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const bool is_horizontal = orientation == Horizontal;
		const float size = is_horizontal ? _width : _height;
		
		if(@container != null)
		{
			if(scroll_container_at_origin)
			{
				scroll_min = 0;
				scroll_max = is_horizontal ? container.scroll_max_x - container.scroll_min_x : container.scroll_max_y - container.scroll_min_y;
			}
			else
			{
				scroll_min = is_horizontal ? container.scroll_min_x : container.scroll_min_y;
				scroll_max = is_horizontal ? container.scroll_max_x : container.scroll_max_y;
			}
			
			scroll_visible = is_horizontal ? container._width : container._height;
			
			if(container._scrolled_into_view)
			{
				position = is_horizontal ? -container._scroll_x : -container._scroll_y;
			}
		}
		
		if(scroll_min > scroll_max)
		{
			const float t = scroll_min;
			scroll_min = scroll_max;
			scroll_max = t;
		}
		
		calculate_scroll_values();
		
		thumb_size = scroll_range <= 0 ? 0 : round(flexible_thumb_size
			? size * (scroll_visible / scroll_width)
			: min(size * 0.5, ui.style.scrollbar_fixed_size));
		
		if(position < scroll_min)
		{
			position = scroll_min;
			calculate_scroll_values();
		}
		else if(position > scroll_min + scroll_range)
		{
			position = scroll_min + scroll_range;
			calculate_scroll_values();
		}
		
		if(dragging_thumb)
		{
			if(ui.mouse.primary_down)
			{
				const float mouse_t = position_max > 0 ? ((is_horizontal ? ui.mouse.x - x1 : ui.mouse.y - y1) - drag_thumb_offset) / position_max : 0;
				position = clamp(scroll_min + scroll_range * mouse_t, scroll_min, scroll_min + scroll_range);
				calculate_scroll_values();
			}
			else
			{
				dragging_thumb = false;
			}
		}
		
		if(@container != null)
		{
			if(is_horizontal)
				container.scroll_x = -position;
			else
				container.scroll_y = -position;
		}
		
		if(hovered)
		{
			const float mouse_t = is_horizontal ? ui.mouse.x - x1 : ui.mouse.y - y1;
			mouse_over_thumb = mouse_t >= thumb_position && mouse_t <= thumb_position + thumb_size;
			
			if(ui.mouse.primary_press)
			{
				if(mouse_over_thumb)
				{
					dragging_thumb = true;
					drag_thumb_offset = mouse_t - thumb_position;
				}
				else if(scroll_range > 0)
				{
					dragging_thumb = true;
					drag_thumb_offset = thumb_size * 0.5;
					
					calculate_scroll_values();
					position = clamp(scroll_min + scroll_range * (position_max > 0 ? (mouse_t - drag_thumb_offset) / position_max : 0), scroll_min, scroll_min + scroll_range);
					calculate_scroll_values();
				}
			}
		}
		else
		{
			mouse_over_thumb = false;
		}
		
		if(previous_position != position)
		{
			EventInfo@ event = ui._event_info_pool.get();
			event.reset(EventType::SCROLL, @this);
			ui._queue_event(@this.scroll, @event);
			previous_position = position;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		const bool is_horizontal = orientation == Horizontal;
		
		style.draw_rectangle(x1, y1, x2, y2, 0, style.normal_bg_clr);
		
		if(thumb_size > 0)
		{
			const uint clr = dragging_thumb ? style.selected_highlight_bg_clr : (mouse_over_thumb ? style.highlight_bg_clr : style.secondary_bg_clr);
			
			if(!is_horizontal)
				style.draw_rectangle(
					x1, y1 + thumb_position,
					x2, y1 + thumb_position + thumb_size,
					0, clr);
			else
				style.draw_rectangle(
					x1 + thumb_position, y1,
					x1 + thumb_position + thumb_size, y2,
					0, clr);
		}
	}
	
	protected void calculate_scroll_values()
	{
		const float size = orientation == Horizontal ? _width : _height;
		
		position_max = size - thumb_size;
		scroll_width = scroll_max - scroll_min;
		scroll_range = max(0, scroll_width - scroll_visible);
		thumb_position = scroll_range > 0 ? position_max * ((position - scroll_min) / scroll_range) : 0;
	}
	
}