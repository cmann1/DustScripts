#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Orientation.cpp';
#include '../events/Event.cpp';
#include 'Container.cpp';

namespace Scrollbar { const string TYPE_NAME = 'Scrollbar'; }

class Scrollbar : Element, IStepHandler
{
	
	Event scroll;
	
	protected Orientation _orientation;
	protected float _scroll_visible;
	protected float _scroll_min;
	protected float _scroll_max;
	protected float _position;
	protected bool _flexible_thumb_size = true;
	
	protected Container@ _container;
	protected EventCallback@ container_scroll_change_delegate;
	protected bool _scroll_container_at_origin = true;
	
	protected float scroll_width;
	protected float scroll_range;
	protected float position_max;
	protected float thumb_position;
	protected float thumb_size;
	
	protected bool mouse_over_thumb;
	protected bool dragging_thumb;
	protected float drag_thumb_offset;
	
	protected float previous_position;
	
	protected bool step_subscribed;
	
	Scrollbar(UI@ ui, Orientation orientation)
	{
		super(ui);
		
		this.orientation = orientation;
		
		if(_orientation == Orientation::Vertical)
		{
			_set_width  = _width = ui.style.default_scrollbar_size;
		}
		else
		{
			_set_height = _height = ui.style.default_scrollbar_size;
		}
	}
	
	string element_type { get const override { return Scrollbar::TYPE_NAME; } }
	
	Orientation orientation
	{
		get const { return _orientation; }
		set
		{
			if(_orientation == value)
				return;
			
			_orientation = value;
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	float scroll_visible
	{
		get const { return _scroll_visible; }
		set
		{
			if(_scroll_visible == value)
				return;
			
			_scroll_visible = value;
			calculate_scroll_values();
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	float scroll_min
	{
		get const { return _scroll_min; }
		set
		{
			if(_scroll_min == value)
				return;
			
			_scroll_min = value;
			calculate_scroll_values();
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	float scroll_max
	{
		get const { return _scroll_max; }
		set
		{
			if(_scroll_max == value)
				return;
			
			_scroll_max = value;
			calculate_scroll_values();
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	float position
	{
		get const { return _position; }
		set
		{
			if(value < scroll_min)
				value = scroll_min;
			else if(value > scroll_min + scroll_range)
				value = scroll_min + scroll_range;
			
			if(_position == value)
				return;
			
			_position = value;
			calculate_scroll_values();
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	bool flexible_thumb_size
	{
		get const { return _flexible_thumb_size; }
		set
		{
			if(_flexible_thumb_size == value)
				return;
			
			_flexible_thumb_size = value;
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}
	
	Container@ container
	{
		get const { return @_container; }
		set
		{
			if(@_container == @value)
				return;
			
			if(@_container != null)
			{
				_container.scroll_change.off(container_scroll_change_delegate);
			}
			
			@_container = @value;
			
			if(@_container != null)
			{
				if(@container_scroll_change_delegate == null)
				{
					@container_scroll_change_delegate = EventCallback(on_container_scroll_change);
				}
				
				_container.scroll_change.on(container_scroll_change_delegate);
			}
			
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}

	bool scroll_container_at_origin
	{
		get const { return _scroll_container_at_origin; }
		set
		{
			if(_scroll_container_at_origin == value)
				return;
			
			_scroll_container_at_origin = value;
			step_subscribed = ui._step_subscribe(this, step_subscribed);
			validate_layout = true;
		}
	}
	
	bool busy_dragging
	{
		get const { return dragging_thumb; }
	}
	
	bool ui_step() override
	{
		const bool is_horizontal = _orientation == Horizontal;
		
		if(dragging_thumb)
		{
			if(ui.mouse.primary_down)
			{
				const float mouse_t = position_max > 0 ? ((is_horizontal ? ui.mouse.x - x1 : ui.mouse.y - y1) - drag_thumb_offset) / position_max : 0;
				position = clamp(_scroll_min + scroll_range * mouse_t, _scroll_min, _scroll_min + scroll_range);
				calculate_scroll_values();
				
				@ui._active_mouse_element = @this;
				step_subscribed = true;
			}
			else
			{
				dragging_thumb = false;
				step_subscribed = false;
			}
		}
		else
		{
			step_subscribed = false;
		}
		
		update_position();
		
		return step_subscribed;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		calculate_scroll_values();
		
		if(_position < scroll_min)
		{
			_position = scroll_min;
			calculate_scroll_values();
		}
		else if(_position > scroll_min + scroll_range)
		{
			_position = scroll_min + scroll_range;
			calculate_scroll_values();
		}
		
		update_position();
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_rectangle(x1, y1, x2, y2, 0, background_colour != 0 ? background_colour : style.normal_bg_clr);
		
		if(thumb_size > 0)
		{
			const uint clr = dragging_thumb ? style.selected_highlight_bg_clr : (mouse_over_thumb ? style.highlight_bg_clr : style.secondary_bg_clr);
			
			if(orientation == Horizontal)
			{
				style.draw_rectangle(
					x1 + thumb_position, y1,
					x1 + thumb_position + thumb_size, y2,
					0, clr);
			}
			else
			{
				style.draw_rectangle(
					x1, y1 + thumb_position,
					x2, y1 + thumb_position + thumb_size,
					0, clr);
			}
		}
	}
	
	protected float calculate_mouse_over_thumb()
	{
		const float mouse_t = orientation == Horizontal ? ui.mouse.x - x1 : ui.mouse.y - y1;
		mouse_over_thumb = mouse_t >= thumb_position && mouse_t <= thumb_position + thumb_size;
		return mouse_t;
	}
	
	protected void calculate_scroll_values()
	{
		const bool is_horizontal = _orientation == Horizontal;
		const float size = is_horizontal ? _width : _height;
		
		if(@_container != null)
		{
			if(_scroll_container_at_origin)
			{
				_scroll_min = 0;
				_scroll_max = is_horizontal ? _container.scroll_max_x - _container.scroll_min_x : _container.scroll_max_y - _container.scroll_min_y;
			}
			else
			{
				_scroll_min = is_horizontal ? _container.scroll_min_x : _container.scroll_min_y;
				_scroll_max = is_horizontal ? _container.scroll_max_x : _container.scroll_max_y;
			}
			
			_scroll_visible = is_horizontal ? _container._width : _container._height;
			
			if(_container._scrolled_into_view)
			{
				_position = is_horizontal ? -_container._scroll_x : -_container._scroll_y;
				_container._scrolled_into_view = false;
			}
		}
		
		if(_scroll_min > _scroll_max)
		{
			const float t = _scroll_min;
			_scroll_min = _scroll_max;
			_scroll_max = t;
		}
		
		scroll_width = scroll_max - scroll_min;
		scroll_range = max(0.0, scroll_width - scroll_visible);
		thumb_size = scroll_range <= 0 ? 0 : max(0.0, round(_flexible_thumb_size
			? size * (_scroll_visible / scroll_width)
			: ui.style.scrollbar_fixed_size));
		position_max = size - thumb_size;
		thumb_position = scroll_range > 0 ? position_max * ((_position - scroll_min) / scroll_range) : 0;
		
		_position = clamp(_position, scroll_min, scroll_min + scroll_range);
		update_position();
		
		
		const float min_size = min(size, orientation == Horizontal ? _height : _width);
		
		if(thumb_size < min_size && scroll_range > 0)
			thumb_size = min_size;
	}
	
	protected void update_position()
	{
		if(previous_position == _position)
			return;
		
		if(@_container != null)
		{
			if(_orientation == Horizontal)
			{
				_container.scroll_x = -_position;
			}
			else
			{
				_container.scroll_y = -_position;
			}
		}
		
		ui._queue_event(@this.scroll, EventType::SCROLL, @this);
		previous_position = _position;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_enter(EventInfo@ event) override
	{
		calculate_mouse_over_thumb();
	}
	
	void _mouse_exit(EventInfo@ event) override
	{
		mouse_over_thumb = false;
	}
	
	void _mouse_move(EventInfo@ event) override
	{
		calculate_mouse_over_thumb();
	}
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		const float mouse_t = calculate_mouse_over_thumb();
		
		if(mouse_over_thumb)
		{
			dragging_thumb = true;
			drag_thumb_offset = mouse_t - thumb_position;
			@ui._active_mouse_element = @this;
		}
		else if(scroll_range > 0)
		{
			dragging_thumb = true;
			drag_thumb_offset = thumb_size * 0.5;
			@ui._active_mouse_element = @this;
			
			calculate_scroll_values();
			position = clamp(_scroll_min + scroll_range * (position_max > 0 ? (mouse_t - drag_thumb_offset) / position_max : 0), _scroll_min, _scroll_min + scroll_range);
			calculate_scroll_values();
		}
		
		step_subscribed = ui._step_subscribe(@this, step_subscribed);
		
		validate_layout = true;
		@ui._active_mouse_element = @this;
	}
	
	void on_container_scroll_change(EventInfo@ event)
	{
		validate_layout = true;
	}
	
}