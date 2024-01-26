#include '../layouts/Layout.cpp';
#include 'Element.cpp';

namespace Container { const string TYPE_NAME = 'Container'; }

class Container : Element
{
	/**
	 * @class Container
	 * @brief Any element that can contain other elements
	 */
	
	protected Element@ _scroll_into_view;
	
	float scroll_min_x;
	float scroll_min_y;
	float scroll_max_x;
	float scroll_max_y;
	/// If true, when a child element is focus it will be scrolled into v iew
	bool autoscroll_on_focus = true;
	
	Event scroll_change;
	
	/// After scrolling an element into view this will be set for one frame.
	/// Prevents attached scrollbars from resetting the position.
	bool _scrolled_into_view;
	
	bool _defer_layout;
	Layout@ _layout;
	
	protected array<Element@> children;
	protected int num_children;
	protected bool children_visible = true;
	
	protected bool auto_update_scroll_rect = true;
	
	Container(UI@ ui)
	{
		super(ui);
	}
	
	string element_type { get const override { return Container::TYPE_NAME; } }
	
	Layout@ layout
	{
		get { return @_layout; }
		set
		{
			if(@_layout == @value)
				return;
			
			@_layout = value;
			validate_layout = true;
		}
	}
	
	Element@ scroll_into_view
	{
		set
		{
			if(@_scroll_into_view == @value)
				return;
			
			@_scroll_into_view = value;
			validate_layout = true;
		}
	}
	
	array<Element@>@ get_children()
	{
		return @children;
	}
	
	bool add_child(Element@ child, int index=-1)
	{
		if(child is null || @child == @this || @child.parent == @this)
			return false;
		
		if(@child.parent != null)
		{
			child.parent.remove_child(child);
		}
		
		if(index < 0 || index >= num_children)
		{
			children.insertLast(child);
		}
		else
		{
			children.insertAt(index, child);
		}
		
		num_children++;
		@child.parent = @this;
		validate_layout = true;
		return true;
	}
	
	bool remove_child(Element@ child)
	{
		if(_remove_child_internal(child))
		{
			child._removed();
			return true;
		}
		
		return false;
	}
	
	protected bool _remove_child_internal(Element@ child)
	{
		if(child is null or @child.parent != @this)
			return false;
		
		int index = children.findByRef(child);
		
		if(index < 0)
			return false;
		
		children.removeAt(index);
		num_children--;
		child._added();
		@child.parent = null;
		validate_layout = true;
		return true;
	}
	
	void set_child_index(Element@ child, int index)
	{
		if(@child == null || @child.parent != @this)
			return;
		
		int old_index = children.findByRef(@child);
		
		if(old_index == -1 || old_index == index)
			return;
		
		if(old_index < index)
			index--;
		
		children.removeAt(old_index);
		
		if(index >= num_children)
		{
			children.insertLast(child);
		}
		else
		{
			children.insertAt(index < 0 ? 0 : index, child);
		}
		
		validate_layout = true;
	}
	
	void move_to_front(Element@ child)
	{
		if(num_children == 0 || @children[num_children - 1] == @child)
			return;
		
		set_child_index(child, num_children + 1);
	}
	
	void move_to_back(Element@ child)
	{
		if(num_children == 0 || @children[0] == @child)
			return;
		
		set_child_index(child, 0);
	}
	
	void move_up(Element@ child)
	{
		if(@child == null || @child.parent != @this || @children[num_children - 1] == @child)
			return;
		
		int index = children.findByRef(@child);
		
		if(index == -1 || index == num_children - 1)
			return;
		
		@children[index] = @children[index + 1];
		@children[index + 1] = child;
		validate_layout = true;
	}
	
	void move_down(Element@ child)
	{
		if(@child == null || @child.parent != @this || @children[0] == @child)
			return;
		
		int index = children.findByRef(@child);
		
		if(index == -1 || index == 0)
			return;
		
		@children[index] = @children[index - 1];
		@children[index - 1] = child;
		validate_layout = true;
	}
	
	void clear()
	{
		children.resize(0);
		num_children = 0;
		validate_layout = true;
	}
	
	bool contains(Element@ element)
	{
		while(@element != null)
		{
			if(@element == @this)
				return true;
			
			@element = @element.parent;
		}
		
		return false;
	}
	
	Element@ first_child
	{
		get { return num_children > 0 ? @children[0] : null; }
	}
	
	Element@ last_child
	{
		get { return num_children > 0 ? @children[num_children - 1] : null; }
	}
	
	Element@ previous_sibling(Element@ child)
	{
		if(@child == null)
			return null;
		
		int index = children.findByRef(@child);
		
		if(index <= 0)
			return null;
		
		return @children[index - 1];
	}
	
	Element@ next_sibling(Element@ child)
	{
		if(@child == null)
			return null;
		
		int index = children.findByRef(@child);
		
		if(index == -1 || index == num_children - 1)
			return null;
		
		return @children[index + 1];
	}
	
	Element@ get_child(int index)
	{
		if(index < 0 || index >= num_children)
			return null;
		
		return @children[index];
	}
	
	int get_child_index(Element@ child)
	{
		return children.findByRef(@child);
	}
	
	int child_count { get const { return num_children; } }
	
	void fit_to_contents(const bool fit_min=false)
	{
		if(validate_layout || fit_min)
		{
			do_fit_contents(fit_min);
		}
		
		float inset_x, inset_y;
		_get_subtree_insets(inset_x, inset_y);
		
		const float padding_left	= layout_padding_left + inset_x;
		const float padding_top		= layout_padding_top + inset_y;
		const float padding_right	= layout_padding_right;
		const float padding_bottom	= layout_padding_bottom;
		const float border_size		= layout_border_size;
		
		float width  = padding_left + (scroll_max_x - scroll_min_x) + padding_right;
		float height = padding_top  + (scroll_max_y - scroll_min_y) + padding_bottom;
		
		if(border_size > 0)
		{
			width  += border_size * 2;
			height += border_size * 2;
		}
		
		this.width  = width;
		this.height = height;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(children_visible)
		{
			stack.push_reversed(@children);
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(_defer_layout)
			return;
		
		_do_layout_internal(@ctx);
		
		_scrolled_into_view = false;
		
		if(@_scroll_into_view != null)
		{
			bool scroll_changed = false;
			
			if(@_scroll_into_view.parent == @this)
			{
				if(_scroll_x + _scroll_into_view._x < 0)
				{
					_scroll_x = -_scroll_into_view._x;
					scroll_changed = true;
				}
				else if(_scroll_x + _scroll_into_view._x + _scroll_into_view._width > _width)
				{
					_scroll_x = _width - (_scroll_into_view._x + _scroll_into_view._width);
					scroll_changed = true;
				}
				
				if(_scroll_y + _scroll_into_view._y < 0)
				{
					_scroll_y = -_scroll_into_view._y;
					scroll_changed = true;
				}
				else if(_scroll_y + _scroll_into_view._y + _scroll_into_view._height > _height)
				{
					_scroll_y = _height - (_scroll_into_view._y + _scroll_into_view._height);
					scroll_changed = true;
				}
				
				_scroll_x = clamp(_scroll_x, -max(scroll_max_x - _width, 0.0), 0);
				_scroll_y = clamp(_scroll_y, -max(scroll_max_y - _height, 0.0), 0);
			}
			
			_scrolled_into_view = scroll_changed;
			@_scroll_into_view = null;
		}
	}
	
	void _do_layout_internal(LayoutContext@ ctx)
	{
		if(!validate_layout)
			return;
		
		if(@_layout != null)
		{
			const float min_x = scroll_min_x;
			const float min_y = scroll_min_y;
			const float max_x = scroll_max_x;
			const float max_y = scroll_max_y;
			
			_layout.do_layout(@children,
				0, 0, _width, _height,
				scroll_min_x, scroll_min_y, scroll_max_x, scroll_max_y);
			
			if(min_x != scroll_min_x || min_y != scroll_min_y || max_x != scroll_max_x || max_y != scroll_max_y)
			{
				ui._dispatch_event(@scroll_change, EventType::SCROLL_CHANGE, @this);
			}
		}
		else if(auto_update_scroll_rect)
		{
			calculate_scroll_rect(false);
		}
		
		validate_layout = false;
	}
	
	float get_preferred_width(const float max_height=-1)
	{
		do_fit_contents(true);
		return scroll_max_x - scroll_min_x + layout_padding_left + layout_padding_right + layout_border_size * 2;
	}
	
	float get_preferred_height(const float max_width=-1)
	{
		do_fit_contents(true);
		return scroll_max_y - scroll_min_y + layout_padding_top + layout_padding_bottom + layout_border_size * 2;
	}
	
	protected void calculate_scroll_rect(const bool fit_min)
	{
		const float min_x = scroll_min_x;
		const float min_y = scroll_min_y;
		const float max_x = scroll_max_x;
		const float max_y = scroll_max_y;
		
		if(num_children == 0 || !children_visible)
		{
			scroll_min_x = 0;
			scroll_min_y = 0;
			scroll_max_x = 0;
			scroll_max_y = 0;
		}
		else
		{
			Element@ element = children[0];
			
			scroll_min_x = element._x;
			scroll_min_y = element._y;
			scroll_max_x = element._x + (fit_min ? element._set_width  : element._width);
			scroll_max_y = element._y + (fit_min ? element._set_height : element._height);
			
			for(int i = 1; i < num_children; i++)
			{
				@element = children[i];
				
				if(!element._visible)
					continue;
				
				const float width  = fit_min ? element._set_width  : element._width;
				const float height = fit_min ? element._set_height : element._height;
				
				if(element._x < scroll_min_x)
					scroll_min_x = element._x;
				if(element._y < scroll_min_y)
					scroll_min_y = element._y;
				if(element._x + width > scroll_max_x)
					scroll_max_x = element._x + width;
				if(element._y + height > scroll_max_y)
					scroll_max_y = element._y + height;
			}
		}
		
		if(min_x != scroll_min_x || min_y != scroll_min_y || max_x != scroll_max_x || max_y != scroll_max_y)
		{
			ui._dispatch_event(@scroll_change, EventType::SCROLL_CHANGE, @this);
		}
	}
	
	protected void do_fit_contents(const bool fit_min)
	{
		if(@_layout != null && children_visible)
		{
			const float min_x = scroll_min_x;
			const float min_y = scroll_min_y;
			const float max_x = scroll_max_x;
			const float max_y = scroll_max_y;
			
			_layout.do_layout(@children,
				0, 0,
				fit_min ? 0 : _width,
				fit_min ? 0 : _height,
				scroll_min_x, scroll_min_y, scroll_max_x, scroll_max_y);
			
			if(min_x != scroll_min_x || min_y != scroll_min_y || max_x != scroll_max_x || max_y != scroll_max_y)
			{
				ui._dispatch_event(@scroll_change, EventType::SCROLL_CHANGE, @this);
			}
		}
		else if(auto_update_scroll_rect || !children_visible)
		{
			calculate_scroll_rect(fit_min);
		}
	}
	
	protected float layout_padding_left		{ get const { return @_layout != null && !is_nan(_layout.padding_left) ? _layout.padding_left: 0; } }
	
	protected float layout_padding_right	{ get const { return @_layout != null && !is_nan(_layout.padding_right) ? _layout.padding_right : 0; } }
	
	protected float layout_padding_top		{ get const { return @_layout != null && !is_nan(_layout.padding_top) ? _layout.padding_top: 0; } }
	
	protected float layout_padding_bottom	{ get const { return @_layout != null && !is_nan(_layout.padding_bottom) ? _layout.padding_bottom: 0; } }
	
	protected float layout_border_size		{ get const { return 0; } }
	
}
