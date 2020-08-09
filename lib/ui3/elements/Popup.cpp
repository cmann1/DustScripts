#include '../../math/math.cpp';
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
	protected float prev_content_width;
	protected float prev_content_height;
	protected bool try_expand;
	
	Popup(UI@ ui, PopupOptions@ options, Element@ target, const bool wait_for_mouse)
	{
		@this._options = @options;
		
		super(ui, _options.get_contenet_element());
		
		@this._target = target;
		waiting_for_mouse = wait_for_mouse && @_target != null;
		
		fit_to_contents();
		prev_content_width  = _content._width;
		prev_content_height = _content._height;
		
		active = true;
		update_fade();
		
		mouse_enabled = false;
		children_mouse_enabled = false;
		
		options._on_popup_show(this);
	}
	
	string element_type { get const override { return 'Popup'; } }
	
	void force_hide()
	{
		_force_hide = true;
		waiting_for_mouse = false;
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
		
		const PopupPosition calculatedPosition = do_position();
		
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
				@_target == null || @_target == @ui.mouse_over_element || pressed || hovered || waiting_for_mouse || _options.force_open ||
				_options.trigger_when_hovered && _target.hovered ||
				_options.keep_open_while_pressed && _target.pressed
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
					active = @_target == @ui.mouse_over_element;
				}
			}
			
			if(!_force_hide && !active && _options.interactable)
			{
				active = overlaps_point(ui.mouse.x, ui.mouse.y) || @_target == @ui.mouse_over_element;
			}
			
			if(_options.force_open)
			{
				_options.force_open = false;
			}
		}
		else if(_options.hide_type == PopupHideType::MouseDownOutside)
		{
			if(
				(ui.mouse.primary_press || _options.any_mouse_down_button && (ui.mouse.left_press || ui.mouse.middle_press || ui.mouse.right_press)) &&
				!overlaps_point(ui.mouse.x, ui.mouse.y) && (@_target == null || !_target.overlaps_point(ui.mouse.x, ui.mouse.y)))
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
		
		if(@content != null)
		{
			_content._x = is_nan(_options.padding_left) ? ui.style.tooltip_padding : _options.padding_left;
			_content._y = is_nan(_options.padding_top)  ? ui.style.tooltip_padding : _options.padding_top;
			
			float new_width  = _width  - content._x - (is_nan(_options.padding_right)  ? ui.style.tooltip_padding : _options.padding_right);
			float new_height = _height - content._y - (is_nan(_options.padding_bottom) ? ui.style.tooltip_padding : _options.padding_bottom);
			
			// Try expanding again to allow for scrollbars etc.
			
			if(!approximately(new_width, prev_content_width) || !approximately(new_height, prev_content_height))
			{
				if(new_width == prev_content_width)
				{
					new_width  = _content._get_max_width(new_height);
				}
				else if(new_height == prev_content_height)
				{
					new_height = _content._get_max_height(new_width);
				}
				
				prev_content_width  = new_width;
				prev_content_height = new_height;
				try_expand = true;
				
				x2 = x1 + content._x + new_width  + (is_nan(_options.padding_right)  ? ui.style.tooltip_padding : _options.padding_right);
				y2 = y1 + content._y + new_height + (is_nan(_options.padding_bottom) ? ui.style.tooltip_padding : _options.padding_bottom);
			}
			
			_content.width  = new_width;
			_content.height = new_height;
		}
		
		_x = x1 - parent.x1;
		_y = y1 - parent.y1;
		_set_width  = _width  = x2 - x1;
		_set_height = _height = y2 - y1;
		
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
				_options._on_popup_hide(this);
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_popup_element(this,
			_options._shadow_colour, _options._background_colour, _options._border_colour, _options._border_size, _options._background_blur,
			_options._has_shadow_colour, _options._has_background_colour, _options._has_border_colour, _options._has_border_size, _options._background_blur,
			_options._has_blur_inset ? _options._blur_inset : NAN);
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
	
	protected PopupPosition do_position()
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
		
		update_world_bounds(parent);
		
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
				if(y1 < view_y1 && max(0, view_y2 - target_y2) > max(0, target_y1 - view_y1))
				{
					calculatedPosition = PopupPosition::Below;
					reposition_y = true;
				}
				break;
			case PopupPosition::Below:
				if(y2 > view_y2 && max(0, target_y1 - view_y1) > max(0, view_y2 - target_y2))
				{
					calculatedPosition = PopupPosition::Above;
					reposition_y = true;
				}
				break;
			case PopupPosition::Left:
				if(x1 < view_x1 && max(0, view_x2 - target_x2) > max(0, target_x1 - view_x1))
				{
					calculatedPosition = PopupPosition::Right;
					reposition_x = true;
				}
				break;
			case PopupPosition::Right:
				if(x2 > view_x2 && max(0, target_x1 - view_x1) > max(0, view_x2 - target_x2))
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
			if(_options.allow_target_overlap || calculatedPosition != PopupPosition::Left)
			{
				x2 += view_x1 - x1;
				x1  = view_x1;
				
				if(x2 > view_x2) x2 = view_x2;
			}
			else
			{
				x1  = view_x1;
			}
		}
		else if(x2 > view_x2)
		{
			if(_options.allow_target_overlap || calculatedPosition != PopupPosition::Right)
			{
				x1 -= x2 - view_x2;
				x2  = view_x2;
				
				if(x1 < view_x1) x1 = view_x1;
			}
			else
			{
				x2  = view_x2;
			}
		}
		
		if(y1 < view_y1)
		{
			if(_options.allow_target_overlap || calculatedPosition != PopupPosition::Above)
			{
				y2 += view_y1 - y1;
				y1  = view_y1;
				
				if(y2 > view_y2) y2 = view_y2;
			}
			else
			{
				y1  = view_y1;
			}
		}
		else if(y2 > view_y2)
		{
			if(_options.allow_target_overlap || calculatedPosition != PopupPosition::Below)
			{
				y1 -= y2 - view_y2;
				y2  = view_y2;
				
				if(y1 < view_y1) y1 = view_y1;
			}
			else
			{
				y2  = view_y2;
			}
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
	
	protected float layout_padding_left {
		get const override { return is_nan(_options.padding_left)   ? ui.style.tooltip_padding : _options.padding_left; } }
	protected float layout_padding_right {
		get const override { return is_nan(_options.padding_right)  ? ui.style.tooltip_padding : _options.padding_right; } }
	protected float layout_padding_top {
		get const override { return is_nan(_options.padding_top)    ? ui.style.tooltip_padding : _options.padding_top; } }
	protected float layout_padding_bottom {
		get const override { return is_nan(_options.padding_bottom) ? ui.style.tooltip_padding : _options.padding_bottom; } }
	
}