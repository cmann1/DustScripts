class Tooltip : SingleContainer
{
	
	Element@ target;
	
	TooltipPosition position;
	float fade_max;
	float offset_max;
	float spacing;
	bool interactable = false;
	
	Event hide;
	
	private bool active;
	private float fade;
	private float offset;
	
	Tooltip(UI@ ui, Element@ target, Element@ contents, TooltipPosition position=TooltipPosition::Top)
	{
		super(ui, contents, 'ttip');
		
		@this.target = target;
		this.position = position;
		
		fade_max 	= max(0, ui.style.tooltip_fade_frames);
		offset_max 	= max(0, ui.style.tooltip_fade_offset);
		spacing 	= ui.style.tooltip_default_spacing;
		
		fit_to_contents();
		
		active = true;
		update_fade();
		
		mouse_enabled = false;
		children_mouse_enabled = false;
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		if(@target == null)
		{
			ui._event_info.reset(EventType::HIDE, this);
			hide.dispatch(ui._event_info);
			return;
		}
		
		TooltipPosition calculatedPosition = position;
		
		//
		// Position tool tip
		//
		
		bool reposition_x = false;
		bool reposition_y = false;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		view_x1 += ui.style.spacing;
		view_x2 -= ui.style.spacing;
		view_y1 += ui.style.spacing;
		view_y2 -= ui.style.spacing;
		
		// Initial positioning
		
		position_x(calculatedPosition);
		position_y(calculatedPosition);
		
		// Swap sides if the tooltip is outside of the view
		
		switch(calculatedPosition)
		{
			case TooltipPosition::Above:
				if(y1 < view_y1 && view_y2 - target.y2 > target.y1 - view_y1)
				{
					calculatedPosition = TooltipPosition::Below;
					reposition_y = true;
				}
				break;
			case TooltipPosition::Below:
				if(y2 > view_y2 && target.y1 - view_y1 > view_y2 - target.y2)
				{
					calculatedPosition = TooltipPosition::Below;
					reposition_y = true;
				}
				break;
			case TooltipPosition::Left:
				if(x1 < view_x1 && view_x2 - target.x2 > target.x1 - view_x1)
				{
					calculatedPosition = TooltipPosition::Right;
					reposition_x = true;
				}
				break;
			case TooltipPosition::Right:
				if(x2 > view_x2 && target.x1 - view_x1 > view_x2 - target.x2)
				{
					calculatedPosition = TooltipPosition::Left;
					reposition_x = true;
				}
				break;
		}
		
		// Reposition after swap
		
		if(reposition_x)
			position_x(calculatedPosition);
		if(reposition_y)
			position_y(calculatedPosition);
		
		// Final clamp to view
		
		if(x1 < view_x1)
		{
			x2 += view_x1 - x1;
			x1 += view_x1 - x1;
		}
		else if(x2 > view_x2)
		{
			x1 -= x2 - view_x2;
			x2 -= x2 - view_x2;
		}
		
		if(y1 < view_y1)
		{
			y2 += view_y1 - y1;
			y1 += view_y1 - y1;
		}
		else if(y2 > view_y2)
		{
			y1 -= y2 - view_y2;
			y2 -= y2 - view_y2;
		}
		
		//
		// Fade the tooltip in or out
		//
		
		bool mouse_over = target.hovered || hovered;
		
		// Don't start fading if the mouse is in the space between the target and the tooltip.
		if(interactable && !mouse_over && spacing > 0 && (
			calculatedPosition == TooltipPosition::Left  || calculatedPosition == TooltipPosition::Right ||
			calculatedPosition == TooltipPosition::Above || calculatedPosition == TooltipPosition::Below
		))
		{
			switch(calculatedPosition)
			{
				case TooltipPosition::Above:
					if(ui.mouse.x >= target.x1 && ui.mouse.x <= target.x2 && ui.mouse.y <= target.y1 && ui.mouse.y >= target.y1 - spacing)
						mouse_over = true;
					break;
				case TooltipPosition::Below:
					if(ui.mouse.x >= target.x1 && ui.mouse.x <= target.x2 && ui.mouse.y >= target.y2 && ui.mouse.y <= target.y2 + spacing)
						mouse_over = true;
					break;
				case TooltipPosition::Left:
					if(ui.mouse.y >= target.y1 && ui.mouse.y <= target.y2 && ui.mouse.y <= target.y1 && ui.mouse.y >= target.y1 - spacing)
						mouse_over = true;
					break;
				case TooltipPosition::Right:
					if(ui.mouse.y >= target.y1 && ui.mouse.y <= target.y2 && ui.mouse.y >= target.y2 && ui.mouse.y <= target.y2 + spacing)
						mouse_over = true;
					break;
			}
			
			// Also check if the mouse overlaps the tooltip or target.
			// do_layout is called before mouse events are processed, so at this point hovered for the tooltip or target
			// may not be set yet even if the mouse is over them
			
			if(!mouse_over)
			{
				mouse_over = overlaps_point(ui.mouse.x, ui.mouse.y) || target.overlaps_point(ui.mouse.x, ui.mouse.y);
			}
		}
		
		if(mouse_over)
		{
			if(fade < fade_max)
			{
				fade++;
				update_fade();
			}
		}
		else
		{
			if(fade > 0)
			{
				fade--;
				update_fade();
			}
			else
			{
				ui._event_info.reset(EventType::HIDE, this);
				hide.dispatch(ui._event_info);
			}
		}
		
		//
		// Offset while fading
		//
		
		switch(calculatedPosition)
		{
			case TooltipPosition::Above:
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideTop:
			case TooltipPosition::InsideRightTop:
				y1 += offset;
				y2 += offset;
				break;
			case TooltipPosition::Below:
			case TooltipPosition::InsideLeftBottom:
			case TooltipPosition::InsideBottom:
			case TooltipPosition::InsideRightBottom:
				y1 -= offset;
				y2 -= offset;
				break;
		}
		
		switch(calculatedPosition)
		{
			case TooltipPosition::Left:
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideLeft:
			case TooltipPosition::InsideLeftBottom:
				x1 -= offset;
				x2 -= offset;
				break;
			case TooltipPosition::Right:
			case TooltipPosition::InsideRightTop:
			case TooltipPosition::InsideRight:
			case TooltipPosition::InsideRightBottom:
				x1 += offset;
				x2 += offset;
				break;
		}
		
		x = x1;
		y = y1;
		
		if(@content != null)
		{
			content.x = ui.style.spacing;
			content.y = ui.style.spacing;
		}
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
		style.draw_popup_element(this);
		
		if(@content != null)
		{
			if(disabled)
				style.disable_alpha();
			
			content.draw(style, sub_frame);
			
			if(disabled)
				style.restore_alpha();
		}
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
	private void update_fade()
	{
		alpha = fade_max <= 0 ? 1.0 : fade / fade_max;
		offset = offset_max * (1 - alpha);
		
		if(alpha < 1)
		{
			mouse_enabled = children_mouse_enabled = false;
		}
		else if(active)
		{
			mouse_enabled = children_mouse_enabled = interactable;
		}
	}
	
	private void position_x(const TooltipPosition position)
	{
		switch(position)
		{
			case TooltipPosition::Above:
			case TooltipPosition::Below:
			case TooltipPosition::InsideTop:
			case TooltipPosition::InsideBottom:
			case TooltipPosition::InsideMiddle:
				x1 = (target.x1 + target.x2) * 0.5 - width * 0.5;
				x2 = x1 + width;
				break;
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideLeft:
			case TooltipPosition::InsideLeftBottom:
				x1 = target.x1 + spacing;
				x2 = x1 + width;
				break;
			case TooltipPosition::InsideRightTop:
			case TooltipPosition::InsideRight:
			case TooltipPosition::InsideRightBottom:
				x2 = target.x2 - spacing;
				x1 = x2 - width;
				break;
			case TooltipPosition::Left:
				x2 = target.x1 - spacing;
				x1 = x2 - width;
				break;
			case TooltipPosition::Right:
				x1 = target.x2 + spacing;
				x2 = x1 + width;
				break;
		}
	}
	
	private void position_y(const TooltipPosition position)
	{
		switch(position)
		{
			case TooltipPosition::Left:
			case TooltipPosition::Right:
			case TooltipPosition::InsideLeft:
			case TooltipPosition::InsideRight:
			case TooltipPosition::InsideMiddle:
				y1 = (target.y1 + target.y2) * 0.5 - height * 0.5;
				y2 = y1 + height;
				break;
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideTop:
			case TooltipPosition::InsideRightTop:
				y1 = target.y1 + spacing;
				y2 = y1 + height;
				break;
			case TooltipPosition::InsideLeftBottom:
			case TooltipPosition::InsideBottom:
			case TooltipPosition::InsideRightBottom:
				y2 = target.y2 - spacing;
				y1 = y2 - height;
				break;
			case TooltipPosition::Above:
				y2 = target.y1 - spacing;
				y1 = y2 - height;
				break;
			case TooltipPosition::Below:
				y1 = target.y2 + spacing;
				y2 = y1 + height;
				break;
		}
	}
	
}