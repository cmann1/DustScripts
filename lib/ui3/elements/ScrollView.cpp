#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include 'SingleContainer.cpp';
#include 'Scrollbar.cpp';

namespace ScrollView { const string TYPE_NAME = 'ScrollView'; }

class ScrollView : LockedContainer
{
	
	bool scroll_horizontal = true;
	bool scroll_vertical = true;
	float scroll_amount = 15;
	Container@ content { get { return @_content; } }
	
	Event scroll;
	
	/// Controls the amount of space around the outside of the contents. Leave as NAN to use the default style spacing.
	/// Overrides the content layout padding.
	float content_padding_left = NAN;
	/// Same as padding_left.
	float content_padding_right = NAN;
	/// Same as padding_left.
	float content_padding_top = NAN;
	/// Same as padding_left.
	float content_padding_bottom = NAN;
	
	protected float previous_scroll_x;
	protected float previous_scroll_y;
	
	protected Container@ _content;
	protected Scrollbar@ scrollbar_horizontal;
	protected Scrollbar@ scrollbar_vertical;
	
	ScrollView(UI@ ui)
	{
		super(ui);
		
		@_content = Container(ui);
		Container::add_child(_content);
		_content.clip_contents = ClippingMode::Inside;
	}
	
	string element_type { get const override { return ScrollView::TYPE_NAME; } }
	
	float scroll_x
	{
		get const { return -_content._scroll_x; }
		set
		{
			_content._scroll_x = value;
			
			if(@scrollbar_horizontal != null)
				scrollbar_horizontal.position = value;
		}
	}
	
	float scroll_y
	{
		get const { return -_content._scroll_y; }
		set
		{
			_content._scroll_y = value;
			
			if(@scrollbar_vertical != null)
				scrollbar_vertical.position = value;
		}
	}
	
	float _get_max_width(const float max_height=-1) override
	{
		do_fit_contents(true);
		
		float width = scroll_max_x - scroll_min_x + layout_padding_left + layout_padding_right + layout_border_size * 2;
		
		if(@scrollbar_vertical != null)
		{
			width += scrollbar_vertical._width + ui.style.spacing;
		}
		else if(scroll_max_y > (max_height >= 0 ? min(max_height, _height) : _height) - ui.style.spacing * 2)
		{
			width += ui.style.default_scrollbar_size + ui.style.spacing;
		}
		
		return width;
	}
	
	float _get_max_height(const float max_width=-1) override
	{
		do_fit_contents(true);
		
		float height = scroll_max_y - scroll_min_y + layout_padding_top + layout_padding_bottom + layout_border_size * 2;
		
		if(@scrollbar_horizontal != null && scrollbar_horizontal._visible)
		{
			height += scrollbar_horizontal._height + ui.style.spacing;
		}
		else if(scroll_max_x - ((max_width >= 0 ? min(max_width, _width) : _width) - ui.style.spacing * 2) > EPSILON)
		{
			height += ui.style.default_scrollbar_size + ui.style.spacing;
		}
		
		return height;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		// If the scrollbars are not always queued for layout, then on the first frame one becomes visible it will not be in the layout queue
		// but will still be drawn on that frame. As a result the drawing which may depend on values calculated during layout will
		// probably be incorrect and could cause the scrollbar to flash
		
		if(@scrollbar_vertical != null)// && scrollbar_vertical.visible)
			stack.push(scrollbar_vertical);
			
		if(@scrollbar_horizontal != null)// && scrollbar_horizontal.visible)
			stack.push(scrollbar_horizontal);
		
		if(_content.validate_layout)
		{
			validate_layout = true;
		}
		
		stack.push(_content);
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const bool prev_scroll_vertical = @scrollbar_vertical != null && scrollbar_vertical._visible;
		const bool prev_scroll_horizontal = @scrollbar_horizontal != null && scrollbar_horizontal._visible;
		
		const float padding_left	= is_nan(this.content_padding_left)		? ui.style.spacing : this.content_padding_left;
		const float padding_right	= is_nan(this.content_padding_right)	? ui.style.spacing : this.content_padding_right;
		const float padding_top		= is_nan(this.content_padding_top)		? ui.style.spacing : this.content_padding_top;
		const float padding_bottom	= is_nan(this.content_padding_bottom)	? ui.style.spacing : this.content_padding_bottom;
		const float padding_h		= padding_left + padding_right;
		const float padding_v		= padding_top + padding_bottom;
		
		_content.x = padding_left;
		_content.y = padding_top;
		float content_width  = _width  - padding_h;
		float content_height = _height - padding_v;
		
		if(@_content._layout != null)
		{
			// Set to some really small number to prevent elements exactly on the edge from possibly being clipped
			_content._layout.padding_left	= EPSILON;
			_content._layout.padding_top	= EPSILON;
			_content._layout.padding_right	= EPSILON;
			_content._layout.padding_bottom	= EPSILON;
		}
		
		if(prev_scroll_vertical)
		{
			if(scroll_vertical)
			{
				content_width -= scrollbar_vertical._width;
				
				if(@_content._layout != null)
					_content._layout.padding_right = content_padding_right;
			}
			else
			{
				scrollbar_vertical.visible = false;
				
				if(@_content._layout != null)
					_content._layout.padding_right = 0;
			}
		}
		
		if(prev_scroll_horizontal)
		{
			if(scroll_horizontal)
			{
				content_height -= scrollbar_horizontal._height;
				
				if(@_content._layout != null)
					_content._layout.padding_bottom = content_padding_bottom;
			}
			else
			{
				scrollbar_horizontal.visible = false;
				
				if(@_content._layout != null)
					_content._layout.padding_bottom = 0;
			}
		}
		
		if(validate_layout)
		{
			_content.validate_layout = true;
		}
		
		content.width = content_width;
		content.height = content_height;
		_content._defer_layout = false;
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
			else if(!scrollbar_vertical._visible)
			{
				scrollbar_vertical.visible = true;
			}
			
			if(!prev_scroll_vertical)
			{
				content_width -= scrollbar_vertical._width;
				
				if(@_content._layout != null)
					_content._layout.padding_right = content_padding_right;
			}
		}
		else if(prev_scroll_vertical)
		{
			scrollbar_vertical.visible = false;
			
			if(@_content._layout != null)
					_content._layout.padding_right = 0;
		}
		
		if(needs_scroll_horizontal)
		{
			if(@scrollbar_horizontal == null)
			{
				@scrollbar_horizontal = Scrollbar(ui, Orientation::Horizontal);
				@scrollbar_horizontal.container = content;
				Container::add_child(scrollbar_horizontal);
			}
			else if(!scrollbar_horizontal._visible)
			{
				scrollbar_horizontal.visible = true;
			}
			
			if(!prev_scroll_horizontal)
			{
				content_height -= scrollbar_horizontal._height;
				
				if(@_content._layout != null)
					_content._layout.padding_bottom = content_padding_bottom;
			}
		}
		else if(prev_scroll_horizontal)
		{
			scrollbar_horizontal.visible = false;
			
			if(@_content._layout != null)
				_content._layout.padding_bottom = 0;
		}
		
		if(needs_scroll_horizontal != prev_scroll_horizontal || needs_scroll_vertical != prev_scroll_vertical)
		{
			content.width = content_width;
			content.height = content_height;
			
			_content.validate_layout = true;
			_content._do_layout_internal(ctx);
		}
		
		update_vertical_scrollbar(needs_scroll_vertical);
		update_horizontal_scrollbar(needs_scroll_horizontal);
		
		if(previous_scroll_x != _content._scroll_x || previous_scroll_y != _content._scroll_y)
		{
			ui._queue_event(@this.scroll, EventType::SCROLL, @this);
			
			previous_scroll_x = _content._scroll_x;
			previous_scroll_y = _content._scroll_y;
		}
		
//		calculate_scroll_rect(false);
		
//		ui.debug.rect(22, 22,
//			_content.x1 + _content.scroll_min_x, _content.y1 + _content.scroll_min_y,
//			_content.x1 + _content.scroll_max_x, _content.y1 + _content.scroll_max_y, 0, 1, 0xaa0000ff, true, 1);
	}
	
	protected void do_fit_contents(const bool fit_min) override
	{
		if(@scrollbar_vertical != null && scrollbar_vertical._visible)
		{
			scrollbar_vertical.visible = false;
		}
		
		if(@scrollbar_horizontal != null && scrollbar_horizontal._visible)
		{
			scrollbar_horizontal.visible = false;
		}
		
		// Not exactly sure where the content position is changed.
		// But it affects the scroll rect calculation
		_content._x=0;
		_content._y=0;
		_content._layout.padding_left	= EPSILON;
		_content._layout.padding_top	= EPSILON;
		_content._layout.padding_right	= EPSILON;
		_content._layout.padding_bottom	= EPSILON;
		
		_content.fit_to_contents(fit_min);
		Container::do_fit_contents(fit_min);
	}
	
	protected void update_vertical_scrollbar(const bool scroll)
	{
		if(!scroll)
			_content._scroll_y = 0;
		
		if(@scrollbar_vertical == null)
			return;
		
		if(scroll && scrollbar_vertical._visible)
		{
			scrollbar_vertical.x = _content._x + _content._width;
			scrollbar_vertical.y = ui.style.spacing;
			scrollbar_vertical.height = _content._height;
		}
		else
		{
			scrollbar_vertical.position = 0;
		}
	}
	
	protected void update_horizontal_scrollbar(const bool scroll)
	{
		if(!scroll)
			_content._scroll_x = 0;
		
		if(@scrollbar_horizontal == null)
			return;
		
		if(scroll && scrollbar_horizontal._visible)
		{
			scrollbar_horizontal.x = ui.style.spacing;
			scrollbar_horizontal.y = _content._y + _content._height;
			scrollbar_horizontal.width = _content._width;
		}
		else
		{
			scrollbar_horizontal.position = 0;
		}
	}
	
	protected float layout_padding_left		{ get const override { return ui.style.padding(content_padding_left); } }
	
	protected float layout_padding_right	{ get const override { return ui.style.padding(content_padding_right); } }
	
	protected float layout_padding_top		{ get const override { return ui.style.padding(content_padding_top); } }
	
	protected float layout_padding_bottom	{ get const override { return ui.style.padding(content_padding_bottom); } }
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_scroll(EventInfo@ event)
	{
		if(@scrollbar_vertical != null && (_content.hovered || scrollbar_vertical.hovered))
		{
			scrollbar_vertical.position += event.mouse.scroll * scroll_amount;
			validate_layout = true;
		}
		else if(@scrollbar_horizontal != null && scrollbar_horizontal.hovered)
		{
			scrollbar_horizontal.position += event.mouse.scroll * scroll_amount;
			validate_layout = true;
		}
	}
	
}
