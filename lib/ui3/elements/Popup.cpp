#include '../UI.cpp';
#include '../Style.cpp';
#include '../popups/PopupOptions.cpp';
#include 'SingleContainer.cpp';

class Popup : SingleContainer
{
	
	/*protected*/ Element@ _target;
	/*protected*/ PopupOptions@ _options;
	
	Event hide;
	
	private bool active;
	private float fade;
	private float offset;
	
	private bool waiting_for_mouse;
	private bool _force_hide = false;
	private bool fading_out = false;
	
	protected float target_x1;
	protected float target_y1;
	protected float target_x2;
	protected float target_y2;
	
	Popup(UI@ ui, PopupOptions@ options, Element@ target, const bool wait_for_mouse, const string type_identifier='popup')
	{
		@this._options = @options;
		
		super(ui, _options.get_contenet_element(), type_identifier);
		
		@this._target = target;
		waiting_for_mouse = wait_for_mouse && @_target != null;
		
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
	
	PopupPosition _do_position()
	{
		PopupPosition calculatedPosition = _options.position;
		
		bool reposition_x = false;
		bool reposition_y = false;
		
		float view_x1, view_y1, view_x2, view_y2;
		ui.get_region(view_x1, view_y1, view_x2, view_y2);
		view_x1 += ui.style.spacing;
		view_x2 -= ui.style.spacing;
		view_y1 += ui.style.spacing;
		view_y2 -= ui.style.spacing;
		
		update_world_bounds();
		
		if(_options.follow_mouse)
		{
			target_x1 = ui.mouse.x;
			target_y1 = ui.mouse.y;
			target_x2 = ui.mouse.x;
			target_y2 = ui.mouse.y;
		}
		else if(!fading_out && @_target != null)
		{
			target_x1 = _target.x1;
			target_y1 = _target.y1;
			target_x2 = _target.x2;
			target_y2 = _target.y2;
		}
		
		// Initial positioning
		
		position_x(calculatedPosition);
		position_y(calculatedPosition);
		
		// Swap sides if the tooltip is outside of the view
		
		switch(calculatedPosition)
		{
			case PopupPosition::Above:
				if(y1 < view_y1 && view_y2 - target_y2 > _set_height)
				{
					calculatedPosition = PopupPosition::Below;
					reposition_y = true;
				}
				break;
			case PopupPosition::Below:
				if(y2 > view_y2 && target_y1 - view_y1 > _set_height)
				{
					calculatedPosition = PopupPosition::Below;
					reposition_y = true;
				}
				break;
			case PopupPosition::Left:
				if(x1 < view_x1 && view_x2 - target_x2 > _set_width)
				{
					calculatedPosition = PopupPosition::Right;
					reposition_x = true;
				}
				break;
			case PopupPosition::Right:
				if(x2 > view_x2 && target_x1 - view_x1 > _set_width)
				{
					calculatedPosition = PopupPosition::Left;
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
			x1  = view_x1;
			
			if(x2 > view_x2) x2 = view_x2;
		}
		else if(x2 > view_x2)
		{
			x1 -= x2 - view_x2;
			x2  = view_x2;
			
			if(x1 < view_x1) x1 = view_x1;
		}
		
		if(y1 < view_y1)
		{
			y2 += view_y1 - y1;
			y1  = view_y1;
			
			if(y2 > view_y2) y2 = view_y2;
		}
		else if(y2 > view_y2)
		{
			y1 -= y2 - view_y2;
			y2  = view_y2;
			
			if(y1 < view_y1) y1 = view_y1;
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
		
		_width  = x2 - x1;
		_height = y2 - y1;
		
		return calculatedPosition;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(@_target != null)
		{
			if(waiting_for_mouse)
			{
				if(hovered || @_target == @ui.mouse_over_element)
				{
					waiting_for_mouse = false;
				}
			}
		}
		else
		{
			waiting_for_mouse = false;
		}
		
		const PopupPosition calculatedPosition = _do_position();
		
		//
		// Fade the tooltip in or out
		//
		
		if(!_options.enabled)
		{
			_force_hide = true;
		}
		
		bool active = !_force_hide;
		
		if(_options.hide_type == PopupHideType::MouseLeave)
		{
			// Also check the pressed to allow interactive or active elements inside the tooltip to prevent the tooltip from disappearing
			active = !_force_hide && (
				@_target == null || @_target == @ui.mouse_over_element || pressed || hovered || waiting_for_mouse ||
				_options.trigger_when_hovered && _target.hovered
			);
			
			// Don't start fading if the mouse is in the space between the _target and the tooltip.
			if(!_force_hide && !_options.follow_mouse && _options.interactable && !active && _options.spacing > 0 && (
				calculatedPosition == PopupPosition::Left  || calculatedPosition == PopupPosition::Right ||
				calculatedPosition == PopupPosition::Above || calculatedPosition == PopupPosition::Below
			))
			{
				switch(calculatedPosition)
				{
					case PopupPosition::Above:
						if(ui.mouse.x >= target_x1 && ui.mouse.x <= target_x2 && ui.mouse.y <= target_y1 && ui.mouse.y >= target_y1 - _options.spacing)
							active = true;
						break;
					case PopupPosition::Below:
						if(ui.mouse.x >= target_x1 && ui.mouse.x <= target_x2 && ui.mouse.y >= target_y2 && ui.mouse.y <= target_y2 + _options.spacing)
							active = true;
						break;
					case PopupPosition::Left:
						if(ui.mouse.y >= target_y1 && ui.mouse.y <= target_y2 && ui.mouse.y <= target_y1 && ui.mouse.y >= target_y1 - _options.spacing)
							active = true;
						break;
					case PopupPosition::Right:
						if(ui.mouse.y >= target_y1 && ui.mouse.y <= target_y2 && ui.mouse.y >= target_y2 && ui.mouse.y <= target_y2 + _options.spacing)
							active = true;
						break;
				}
				
				// Also check if the mouse overlaps the tooltip or _target_
				// do_layout is called before mouse events are processed, so at this point hovered for the tooltip or _target
				// may not be set yet even if the mouse is over them
				
				if(!active && !_options.follow_mouse)
				{
					active = overlaps_point(ui.mouse.x, ui.mouse.y) || @_target == @ui.mouse_over_element;
				}
			}
		}
		else if(_options.hide_type == PopupHideType::MouseDownOutside)
		{
			if(ui.mouse.primary_press && !overlaps_point(ui.mouse.x, ui.mouse.y) && (@_target == null || !_target.overlaps_point(ui.mouse.x, ui.mouse.y)))
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
			case PopupPosition::Above:
			case PopupPosition::InsideLeftTop:
			case PopupPosition::InsideTop:
			case PopupPosition::InsideRightTop:
				y1 += offset;
				y2 += offset;
				break;
			case PopupPosition::Below:
			case PopupPosition::InsideLeftBottom:
			case PopupPosition::InsideBottom:
			case PopupPosition::InsideRightBottom:
				y1 -= offset;
				y2 -= offset;
				break;
		}
		
		switch(calculatedPosition)
		{
			case PopupPosition::Left:
			case PopupPosition::InsideLeftTop:
			case PopupPosition::InsideLeft:
			case PopupPosition::InsideLeftBottom:
				x1 -= offset;
				x2 -= offset;
				break;
			case PopupPosition::Right:
			case PopupPosition::InsideRightTop:
			case PopupPosition::InsideRight:
			case PopupPosition::InsideRightBottom:
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
			fading_out = false;
			
			if(fade < _options.fade_max)
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
				_options._on_popup_hide();
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_popup_element(this, style.tooltip_blur_inset);
	}
	
	private void update_fade()
	{
		alpha = _options.fade_max <= 0 ? 1.0 : fade / _options.fade_max;
		offset = _options.offset_max * (1 - alpha);
		
		if(alpha < 1)
		{
			mouse_enabled = children_mouse_enabled = false;
		}
		else if(active)
		{
			mouse_enabled = children_mouse_enabled = _options.interactable;
		}
	}
	
	private void position_x(const PopupPosition position)
	{
		switch(position)
		{
			case PopupPosition::Above:
			case PopupPosition::Below:
			case PopupPosition::InsideTop:
			case PopupPosition::InsideBottom:
			case PopupPosition::InsideMiddle:
				x1 = (target_x1 + target_x2) * 0.5 - _set_width * 0.5;
				x2 = x1 + _set_width;
				break;
			case PopupPosition::InsideLeftTop:
			case PopupPosition::InsideLeft:
			case PopupPosition::InsideLeftBottom:
				x1 = target_x1 + _options.spacing;
				x2 = x1 + _set_width;
				break;
			case PopupPosition::InsideRightTop:
			case PopupPosition::InsideRight:
			case PopupPosition::InsideRightBottom:
				x2 = target_x2 - _options.spacing;
				x1 = x2 - _set_width;
				break;
			case PopupPosition::Left:
				x2 = target_x1 - _options.spacing;
				x1 = x2 - _set_width;
				break;
			case PopupPosition::Right:
				x1 = target_x2 + _options.spacing;
				x2 = x1 + _set_width;
				break;
		}
	}
	
	private void position_y(const PopupPosition position)
	{
		switch(position)
		{
			case PopupPosition::Left:
			case PopupPosition::Right:
			case PopupPosition::InsideLeft:
			case PopupPosition::InsideRight:
			case PopupPosition::InsideMiddle:
				y1 = (target_y1 + target_y2) * 0.5 - _set_height * 0.5;
				y2 = y1 + _set_height;
				break;
			case PopupPosition::InsideLeftTop:
			case PopupPosition::InsideTop:
			case PopupPosition::InsideRightTop:
				y1 = target_y1 + _options.spacing;
				y2 = y1 + _set_height;
				break;
			case PopupPosition::InsideLeftBottom:
			case PopupPosition::InsideBottom:
			case PopupPosition::InsideRightBottom:
				y2 = target_y2 - _options.spacing;
				y1 = y2 - _set_height;
				break;
			case PopupPosition::Above:
				y2 = target_y1 - _options.spacing;
				y1 = y2 - _set_height;
				break;
			case PopupPosition::Below:
				y1 = target_y2 + _options.spacing;
				y2 = y1 + _set_height;
				break;
		}
	}
	
}