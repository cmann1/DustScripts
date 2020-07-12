#include '../math/Rect.cpp';
#include '../math/math.cpp';
#include 'Direction.cpp';

class Scrollbar
{
	
	UI@ ui;
	
	private bool dragging_thumb;
	private float drag_offset;
	
	Scrollbar(UI@ ui)
	{
		@this.ui = ui;
	}
	
	float draw(scene@ g, Rect rect, float scroll, float scrollMax, Direction direction)
	{
		float scrollFactor = scrollMax > 0 ? scroll / scrollMax : 0;
		bool mouse_over_thumb;
		
		bool is_vertical = direction == Direction::Vertical;
		
		float track_start, track_end;
		float track_left, track_right;
		float thumb_start, thumb_end;
		float mouse;
		
		if(is_vertical)
		{
			track_start = rect.y1;
			track_end = rect.y2;
			mouse = ui.mouse.y;
			track_left = rect.x2 + ui.padding;
		}
		else
		{
			track_start = rect.x1;
			track_end = rect.x2;
			mouse = ui.mouse.x;
			track_left = rect.y2 + ui.padding;
		}
		
		track_right = track_left + ui.scrollbar_width;
		
		float track_length = track_end - track_start;
		
		if(dragging_thumb)
		{
			scrollFactor = clamp01((mouse - drag_offset - (track_start + ui.scrollbar_thumb_size * 0.5)) / (track_length - ui.scrollbar_thumb_size));
		}
		
		const float track_pos = track_start + ui.scrollbar_thumb_size * 0.5 + (track_length - ui.scrollbar_thumb_size) * scrollFactor;
		
		// Draw track
		// ------------------------------
		
		if(is_vertical)
		{
			g.draw_rectangle_hud(22, 22, track_left, track_start, track_right, track_end, 0, ui.scrollbar_track_colour);
		}
		else
		{
			g.draw_rectangle_hud(22, 22, track_start, track_left, track_end, track_right, 0, ui.scrollbar_track_colour);
		}
		
		// Draw thumb
		// ------------------------------
		
		if(scrollMax > 0)
		{
			thumb_start = track_pos - ui.scrollbar_thumb_size * 0.5;
			thumb_end = track_pos + ui.scrollbar_thumb_size * 0.5;
			
			Rect thumb_rect = is_vertical
				? Rect(track_left, thumb_start, track_right, thumb_end)
				: Rect(thumb_start, track_left, thumb_end, track_right);
			
			mouse_over_thumb = ui.mouse_intersects(thumb_rect);
			
			const uint thumb_colour = mouse_over_thumb || dragging_thumb ? ui.scrollbar_thumb_hover_colour : ui.scrollbar_thumb_colour;
			g.draw_rectangle_hud(22, 22, thumb_rect.x1, thumb_rect.y1, thumb_rect.x2, thumb_rect.y2, 0, thumb_colour);
		}
		
		if(ui.mouse.left_press)
		{
			if(mouse_over_thumb)
			{
				dragging_thumb = true;
				drag_offset = mouse - (thumb_start + thumb_end) * 0.5;
			}
		}
		else if(!ui.mouse.left_down)
		{
			dragging_thumb = false;
		}
		
		return dragging_thumb ? scrollFactor * scrollMax : -1;
	}
	
}