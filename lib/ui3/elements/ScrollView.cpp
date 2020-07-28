#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include '../utils/ILayoutParentHandler.cpp';
#include 'SingleContainer.cpp';
#include 'Scrollbar.cpp';

class ScrollView : LockedContainer, ILayoutParentHandler
{
	
	bool scroll_horizontal = true;
	bool scroll_vertical = true;
	float scroll_amount = 15;
	Container@ content { get { return @_content; } }
	
	protected Container@ _content;
	protected Scrollbar@ scrollbar_horizontal;
	protected Scrollbar@ scrollbar_vertical;
	
	ScrollView(UI@ ui)
	{
		super(ui, 'scrv');
		
		@_content = Container(ui);
		Container::add_child(_content);
		_content.clip_contents = ClippingMode::Inside;
		@_content._layout_handler = this;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(@scrollbar_vertical != null && scrollbar_vertical.visible)
			stack.push(scrollbar_vertical);
			
		if(@scrollbar_horizontal != null && scrollbar_horizontal.visible)
			stack.push(scrollbar_horizontal);
		
		stack.push(_content);
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(@scrollbar_vertical != null && scrollbar_vertical.visible)
		{
			scrollbar_vertical._x = _width - scrollbar_vertical._width - ui.style.spacing;
			scrollbar_vertical._y = ui.style.spacing;
			scrollbar_vertical._height = _height - ui.style.spacing * 2;
		}
	}
	
	void do_child_layout(LayoutContext@ ctx, Container@ container)
	{
		const bool prev_scroll_vertical = @scrollbar_vertical != null && scrollbar_vertical.visible;
		const bool prev_scroll_horizontal = @scrollbar_horizontal != null && scrollbar_horizontal.visible;
		
		_content.x = ui.style.spacing;
		_content.y = ui.style.spacing;
		_content.width  = _width  - ui.style.spacing * 2;
		_content.height = _height - ui.style.spacing * 2;
		
		if(prev_scroll_vertical)
		{
			if(!scroll_vertical)
				scrollbar_vertical.visible = false;
			else
				_content._width -= scrollbar_vertical._width;
		}
		
		if(prev_scroll_horizontal)
		{
			if(!scroll_horizontal)
				scrollbar_horizontal.visible = false;
			else
				_content._height -= scrollbar_horizontal._height;
		}
		
		_content._do_layout_internal(ctx);
		
		const bool needs_scroll_vertical   = scroll_vertical   && _content.scroll_max_y - _content.scroll_min_y > _content._height;
		const bool needs_scroll_horizontal = scroll_horizontal && _content.scroll_max_x - _content.scroll_min_x > _content._width;
		
		if(needs_scroll_vertical)
		{
			if(@scrollbar_vertical == null)
			{
				@scrollbar_vertical = Scrollbar(ui, Orientation::Vertical);
				Container::add_child(scrollbar_vertical);
			}
			else if(!scrollbar_vertical.visible)
			{
				scrollbar_vertical.visible = true;
			}
			
			if(!prev_scroll_vertical)
				_content._width -= scrollbar_vertical._width;
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
				Container::add_child(scrollbar_horizontal);
			}
			else if(!scrollbar_horizontal.visible)
			{
				scrollbar_horizontal.visible = true;
			}
			
			if(!prev_scroll_horizontal)
				_content._height-= scrollbar_horizontal._height;
		}
		else if(prev_scroll_horizontal)
		{
			scrollbar_horizontal.visible = false;
		}
		
		if(needs_scroll_horizontal != prev_scroll_horizontal || needs_scroll_vertical != prev_scroll_vertical)
		{
			_content._validate_layout = true;
			_content._do_layout_internal(ctx);
		}
		
		const bool scrollbar_vertical_update   = prev_scroll_vertical && scrollbar_vertical.busy_dragging;
		const bool scrollbar_horizontal_update = prev_scroll_horizontal && scrollbar_horizontal.busy_dragging;
		
		int scroll_dir;
		const bool scrolled = !scrollbar_vertical_update && !scrollbar_horizontal_update && ui.mouse.scrolled(scroll_dir);
		const bool scrolled_x = scrolled && @scrollbar_horizontal != null && scrollbar_horizontal.hovered;
		const bool scrolled_y = scrolled && _content.hovered || @scrollbar_vertical != null && scrollbar_vertical.hovered;
		
		content.scroll_y = clamp(
			scrollbar_vertical_update ? -scrollbar_vertical.position :
				(scrolled_y
					? _content.scroll_y - scroll_dir * scroll_amount
					: _content._scroll_y),
			-max(0, (_content.scroll_max_y - _content.scroll_min_y) - _content._height),
			0);
		content.scroll_x = clamp(
			scrollbar_horizontal_update ? -scrollbar_horizontal.position :
			(scrolled_x
				? _content.scroll_x - scroll_dir * scroll_amount
				: _content._scroll_x),
			-max(0, (_content.scroll_max_x - _content.scroll_min_x) - _content._width),
			0);
		
		if(needs_scroll_vertical)
			update_vertical_scrollbar();
		if(needs_scroll_horizontal)
			update_horizontal_scrollbar();
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_rectangle(x1, y1, x2, y2, 0, style.normal_bg_clr);
	}
	
	protected void update_vertical_scrollbar()
	{
		if(@scrollbar_vertical == null || !scrollbar_vertical.visible)
			return;
		
		scrollbar_vertical._x = _content._x + _content._width;
		scrollbar_vertical._y = ui.style.spacing;
		scrollbar_vertical._height = _height - ui.style.spacing * 2;
		
		scrollbar_vertical.scroll_min = 0;
		scrollbar_vertical.scroll_max = _content.scroll_max_y - _content.scroll_min_y;
		scrollbar_vertical.scroll_visible = _content._height;
		scrollbar_vertical.position = -_content._scroll_y;
	}
	
	protected void update_horizontal_scrollbar()
	{
		if(@scrollbar_horizontal == null || !scrollbar_horizontal.visible)
			return;
		
		scrollbar_horizontal._x = ui.style.spacing;
		scrollbar_horizontal._y = _content._y + _content._height;
		scrollbar_horizontal._width = _width - ui.style.spacing * 2;
		
		scrollbar_horizontal.scroll_min = 0;
		scrollbar_horizontal.scroll_max = _content.scroll_max_x - _content.scroll_min_x;
		scrollbar_horizontal.scroll_visible = _content._width;
		scrollbar_horizontal.position = -_content._scroll_x;
	}
	
}