#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/TooltipOptions.cpp';
#include 'SingleContainer.cpp';

class Tooltip : SingleContainer
{
	
	Element@ target;
	TooltipOptions@ options;
	
	Event hide;
	
	private bool active;
	private float fade;
	private float offset;
	
	private bool waiting_for_mouse;
	private bool _force_hide = false;
	private bool fading_out = false;
	
	private float target_x1;
	private float target_y1;
	private float target_x2;
	private float target_y2;
	
	Tooltip(UI@ ui, TooltipOptions@ options, Element@ target, bool wait_for_mouse)
	{
		@this.options = options;
		
		super(ui, options.get_contenet_element(), 'ttip');
		
		@this.target = target;
		waiting_for_mouse = wait_for_mouse && @target != null;
		
		fit_to_contents();
		
		active = true;
		update_fade();
		
		mouse_enabled = false;
		children_mouse_enabled = false;
	}
	
	void fit_to_contents(bool include_border, float padding_x=NAN, float padding_y=NAN) override
	{
		if(is_nan(padding_x))
			padding_x = ui.style.tooltip_padding;
		if(is_nan(padding_y))
			padding_y = ui.style.tooltip_padding;
		
		SingleContainer::fit_to_contents(include_border, padding_x, padding_y);
	}
	
	void force_hide()
	{
		_force_hide = true;
		waiting_for_mouse = false;
	}
	
	TooltipPosition _do_position()
	{
		TooltipPosition calculatedPosition = options.position;
		
		bool reposition_x = false;
		bool reposition_y = false;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		view_x1 += ui.style.spacing;
		view_x2 -= ui.style.spacing;
		view_y1 += ui.style.spacing;
		view_y2 -= ui.style.spacing;
		
		update_world_bounds();
		
		if(options.follow_mouse)
		{
			target_x1 = ui.mouse.x;
			target_y1 = ui.mouse.y;
			target_x2 = ui.mouse.x;
			target_y2 = ui.mouse.y;
		}
		else if(!fading_out && @target != null)
		{
			target_x1 = target.x1;
			target_y1 = target.y1;
			target_x2 = target.x2;
			target_y2 = target.y2;
		}
		
		// Initial positioning
		
		position_x(calculatedPosition);
		position_y(calculatedPosition);
		
		// Swap sides if the tooltip is outside of the view
		
		switch(calculatedPosition)
		{
			case TooltipPosition::Above:
				if(y1 < view_y1 && view_y2 - target_y2 > target_y1 - view_y1)
				{
					calculatedPosition = TooltipPosition::Below;
					reposition_y = true;
				}
				break;
			case TooltipPosition::Below:
				if(y2 > view_y2 && target_y1 - view_y1 > view_y2 - target_y2)
				{
					calculatedPosition = TooltipPosition::Below;
					reposition_y = true;
				}
				break;
			case TooltipPosition::Left:
				if(x1 < view_x1 && view_x2 - target_x2 > target_x1 - view_x1)
				{
					calculatedPosition = TooltipPosition::Right;
					reposition_x = true;
				}
				break;
			case TooltipPosition::Right:
				if(x2 > view_x2 && target_x1 - view_x1 > view_x2 - target_x2)
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
		
		if(@parent != null)
		{
			_x = x1 - parent.x1;
			_y = y1 - parent.y1;
		}
		else
		{
			_x = x1;
			_y = y1;
		}
		
		return calculatedPosition;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(@target != null)
		{
			if(waiting_for_mouse)
			{
				if(hovered || @target == @ui.mouse_over_element)
				{
					waiting_for_mouse = false;
				}
			}
		}
		else
		{
			waiting_for_mouse = false;
		}
		
		const TooltipPosition calculatedPosition = _do_position();
		
		//
		// Fade the tooltip in or out
		//
		
		if(!options.enabled)
		{
			_force_hide = true;
		}
		
		bool active = !_force_hide;
		
		if(options.hide_type == TooltipHideType::MouseLeave)
		{
			active = !_force_hide && (@target == null || @target == @ui.mouse_over_element || hovered || waiting_for_mouse);
			
			// Don't start fading if the mouse is in the space between the target and the tooltip.
			if(!_force_hide && !options.follow_mouse && options.interactable && !active && options.spacing > 0 && (
				calculatedPosition == TooltipPosition::Left  || calculatedPosition == TooltipPosition::Right ||
				calculatedPosition == TooltipPosition::Above || calculatedPosition == TooltipPosition::Below
			))
			{
				switch(calculatedPosition)
				{
					case TooltipPosition::Above:
						if(ui.mouse.x >= target_x1 && ui.mouse.x <= target_x2 && ui.mouse.y <= target_y1 && ui.mouse.y >= target_y1 - options.spacing)
							active = true;
						break;
					case TooltipPosition::Below:
						if(ui.mouse.x >= target_x1 && ui.mouse.x <= target_x2 && ui.mouse.y >= target_y2 && ui.mouse.y <= target_y2 + options.spacing)
							active = true;
						break;
					case TooltipPosition::Left:
						if(ui.mouse.y >= target_y1 && ui.mouse.y <= target_y2 && ui.mouse.y <= target_y1 && ui.mouse.y >= target_y1 - options.spacing)
							active = true;
						break;
					case TooltipPosition::Right:
						if(ui.mouse.y >= target_y1 && ui.mouse.y <= target_y2 && ui.mouse.y >= target_y2 && ui.mouse.y <= target_y2 + options.spacing)
							active = true;
						break;
				}
				
				// Also check if the mouse overlaps the tooltip or target_
				// do_layout is called before mouse events are processed, so at this point hovered for the tooltip or target
				// may not be set yet even if the mouse is over them
				
				if(!active && !options.follow_mouse)
				{
					active = overlaps_point(ui.mouse.x, ui.mouse.y) || @target == @ui.mouse_over_element;
				}
			}
		}
		else if(options.hide_type == TooltipHideType::MouseDownOutside)
		{
			if(ui.mouse.primary_press && !overlaps_point(ui.mouse.x, ui.mouse.y) && (@target == null || !target.overlaps_point(ui.mouse.x, ui.mouse.y)))
			{
				active = false;
				_force_hide = true;
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
		
		_x = x1 - parent.x1;
		_y = y1 - parent.y1;
		
		if(@content != null)
		{
			content.x = ui.style.tooltip_padding;
			content.y = ui.style.tooltip_padding;
		}
		
		if(active)
		{
			if(fade < options.fade_max)
			{
				fade++;
				update_fade();
			}
		}
		else
		{
			if(fade > 0)
			{
				fading_out = true;
				fade--;
				update_fade();
			}
			else
			{
				ui._event_info.reset(EventType::HIDE, this);
				hide.dispatch(ui._event_info);
				options._on_tooltip_hide();
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_popup_element(this, style.tooltip_blur_inset);
	}
	
	private void update_fade()
	{
		alpha = options.fade_max <= 0 ? 1.0 : fade / options.fade_max;
		offset = options.offset_max * (1 - alpha);
		
		if(alpha < 1)
		{
			mouse_enabled = children_mouse_enabled = false;
		}
		else if(active)
		{
			mouse_enabled = children_mouse_enabled = options.interactable;
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
				x1 = (target_x1 + target_x2) * 0.5 - width * 0.5;
				x2 = x1 + width;
				break;
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideLeft:
			case TooltipPosition::InsideLeftBottom:
				x1 = target_x1 + options.spacing;
				x2 = x1 + width;
				break;
			case TooltipPosition::InsideRightTop:
			case TooltipPosition::InsideRight:
			case TooltipPosition::InsideRightBottom:
				x2 = target_x2 - options.spacing;
				x1 = x2 - width;
				break;
			case TooltipPosition::Left:
				x2 = target_x1 - options.spacing;
				x1 = x2 - width;
				break;
			case TooltipPosition::Right:
				x1 = target_x2 + options.spacing;
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
				y1 = (target_y1 + target_y2) * 0.5 - height * 0.5;
				y2 = y1 + height;
				break;
			case TooltipPosition::InsideLeftTop:
			case TooltipPosition::InsideTop:
			case TooltipPosition::InsideRightTop:
				y1 = target_y1 + options.spacing;
				y2 = y1 + height;
				break;
			case TooltipPosition::InsideLeftBottom:
			case TooltipPosition::InsideBottom:
			case TooltipPosition::InsideRightBottom:
				y2 = target_y2 - options.spacing;
				y1 = y2 - height;
				break;
			case TooltipPosition::Above:
				y2 = target_y1 - options.spacing;
				y1 = y2 - height;
				break;
			case TooltipPosition::Below:
				y1 = target_y2 + options.spacing;
				y2 = y1 + height;
				break;
		}
	}
	
}