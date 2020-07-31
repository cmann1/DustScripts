#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include 'SingleContainer.cpp';
#include 'Scrollbar.cpp';

class ScrollView : LockedContainer
{
	
	bool scroll_horizontal = true;
	bool scroll_vertical = true;
	float scroll_amount = 15;
	Container@ content { get { return @_content; } }
	
	Event scroll;
	
	protected float previous_scroll_x;
	protected float previous_scroll_y;
	
	protected Container@ _content;
	protected Scrollbar@ scrollbar_horizontal;
	protected Scrollbar@ scrollbar_vertical;
	
	ScrollView(UI@ ui)
	{
		super(ui, 'scrv');
		
		@_content = Container(ui);
		Container::add_child(_content);
		_content.clip_contents = ClippingMode::Inside;
	}
	
	float scroll_x { get const { return -_content._scroll_x; } }
	
	float scroll_y { get const { return -_content._scroll_y; } }
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		// If the scrollbars are not always queued for layout, then on the first frame one becomes visible it will not be in the layout queue
		// but will still be drawn on that frame. As a result the drawing which may depend on values calculated during layout will
		// probably be incorrect and could cause the scrollbar to flash
		
		if(@scrollbar_vertical != null)// && scrollbar_vertical.visible)
			stack.push(scrollbar_vertical);
			
		if(@scrollbar_horizontal != null)// && scrollbar_horizontal.visible)
			stack.push(scrollbar_horizontal);
		
		stack.push(_content);
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const bool prev_scroll_vertical = @scrollbar_vertical != null && scrollbar_vertical.visible;
		const bool prev_scroll_horizontal = @scrollbar_horizontal != null && scrollbar_horizontal.visible;
		
		_content.x = ui.style.spacing;
		_content.y = ui.style.spacing;
		float content_width  = _width  - ui.style.spacing * 2;
		float content_height = _height - ui.style.spacing * 2;
		
		if(prev_scroll_vertical)
		{
			if(!scroll_vertical)
				scrollbar_vertical.visible = false;
			else
				content_width -= scrollbar_vertical._width;
		}
		
		if(prev_scroll_horizontal)
		{
			if(!scroll_horizontal)
				scrollbar_horizontal.visible = false;
			else
				content_height -= scrollbar_horizontal._height;
		}
		
		if(_validate_layout)
		{
			_content._validate_layout = true;
			_validate_layout = false;
		}
		
		content.width = content_width;
		content.height = content_height;
		_content._do_layout(ctx);
		_content._defer_layout = true;
		
		const bool needs_scroll_vertical   = scroll_vertical   && (_content.scroll_max_y - _content._height) >= 1;
		const bool needs_scroll_horizontal = scroll_horizontal && (_content.scroll_max_x - _content._width)  >= 1;
		
		if(needs_scroll_vertical)
		{
			if(@scrollbar_vertical == null)
			{
				@scrollbar_vertical = Scrollbar(ui, Orientation::Vertical);
				@scrollbar_vertical.container = content;
				Container::add_child(scrollbar_vertical);
			}
			else if(!scrollbar_vertical.visible)
			{
				scrollbar_vertical.visible = true;
			}
			
			if(!prev_scroll_vertical)
				content_width -= scrollbar_vertical._width;
		}
		else if(prev_scroll_vertical)
		{
			scrollbar_vertical.visible = false;
		}
		
		if(needs_scroll_horizontal)
		{
			if(@scrollbar_horizontal == null)
			{
				@scrollbar_horizontal = Scrollbar(ui, Orientation::Horizontal);
				@scrollbar_horizontal.container = content;
				Container::add_child(scrollbar_horizontal);
			}
			else if(!scrollbar_horizontal.visible)
			{
				scrollbar_horizontal.visible = true;
			}
			
			if(!prev_scroll_horizontal)
				content_height -= scrollbar_horizontal._height;
		}
		else if(prev_scroll_horizontal)
		{
			scrollbar_horizontal.visible = false;
		}
		
		if(needs_scroll_horizontal != prev_scroll_horizontal || needs_scroll_vertical != prev_scroll_vertical)
		{
			content.width = content_width;
			content.height = content_height;
			
			_content._validate_layout = true;
			_content._do_layout_internal(ctx);
		}
		
		const bool scrollbar_vertical_update   = prev_scroll_vertical && scrollbar_vertical.busy_dragging;
		const bool scrollbar_horizontal_update = prev_scroll_horizontal && scrollbar_horizontal.busy_dragging;
		
		int scroll_dir;
		const bool scrolled = !scrollbar_vertical_update && !scrollbar_horizontal_update && ui.mouse.scrolled(scroll_dir);
		const bool scrolled_x = scrolled && @scrollbar_horizontal != null && scrollbar_horizontal.hovered;
		const bool scrolled_y = scrolled && (_content.hovered || @scrollbar_vertical != null && scrollbar_vertical.hovered);
		
		if(scrolled_y && needs_scroll_vertical)
		{
			scrollbar_vertical.position += scroll_dir * scroll_amount;
		}
		
		if(scrolled_x && needs_scroll_horizontal)
		{
			scrollbar_horizontal.position += scroll_dir * scroll_amount;
		}
		
		if(needs_scroll_vertical)
			update_vertical_scrollbar();
		if(needs_scroll_horizontal)
			update_horizontal_scrollbar();
		
		if(previous_scroll_x != _content._scroll_x || previous_scroll_y != _content._scroll_y)
		{
			EventInfo@ event = ui._event_info_pool.get();
			event.reset(EventType::SCROLL, @this);
			ui._queue_event(@this.scroll, @event);
			
			previous_scroll_x = _content._scroll_x;
			previous_scroll_y = _content._scroll_y;
		}
	}
	
	protected void update_vertical_scrollbar()
	{
		if(@scrollbar_vertical == null || !scrollbar_vertical.visible)
			return;
		
		scrollbar_vertical._x = _content._x + _content._width;
		scrollbar_vertical._y = ui.style.spacing;
		scrollbar_vertical._height = _content._height;
	}
	
	protected void update_horizontal_scrollbar()
	{
		if(@scrollbar_horizontal == null || !scrollbar_horizontal.visible)
			return;
		
		scrollbar_horizontal._x = ui.style.spacing;
		scrollbar_horizontal._y = _content._y + _content._height;
		scrollbar_horizontal._width = _content._width;
	}
	
}