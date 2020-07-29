#include '../layouts/Layout.cpp';
#include '../utils/ILayoutParentHandler.cpp';
#include 'Element.cpp';

class Container : Element
{
	/**
	 * @class Container
	 * @brief Any element that can contain other elements
	 */
	
	Element@ scroll_into_view;
	
	bool _validate_layout = true;
	float scroll_min_x;
	float scroll_min_y;
	float scroll_max_x;
	float scroll_max_y;
	
	/// After scrolling an element into view this will be set for one frame.
	/// Prevents attached scrollbars from resetting the position.
	bool _scrolled_into_view;
	ILayoutParentHandler@ _layout_handler;
	
	protected array<Element@> children;
	protected int num_children;
	
	protected Layout@ _layout;
	
	Container(UI@ ui, const string &in type_identifier = 'ctr')
	{
		super(ui, type_identifier);
	}
	
	Layout@ layout
	{
		get { return @_layout; }
		set
		{
			if(@_layout == @value)
				return;
			
			@_layout = value;
			_validate_layout = true;
		}
	}
	
	float width
	{
		set override
		{
			if(_width == value) return;
			_width = value;
			_validate_layout = true;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	float height
	{
		set override
		{
			if(_height == value) return;
			_height = value;
			_validate_layout = true;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	
	array<Element@>@ get_children()
	{
		return @children;
	}
	
	bool add_child(Element@ child)
	{
		if(child is null || @child == @this || @child.parent == @this)
			return false;
		
		if(@child.parent != null)
		{
			child.parent.remove_child(child);
		}
		
		children.insertLast(child);
		num_children++;
		@child.parent = @this;
		_validate_layout = true;
		return true;
	}
	
	bool remove_child(Element@ child)
	{
		if(_remove_child_internal(child))
		{
			child._removed();
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
		_validate_layout = true;
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
		
		_validate_layout = true;
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
		_validate_layout = true;
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
		_validate_layout = true;
	}
	
	void clear()
	{
		children.resize(0);
		_validate_layout = true;
	}
	
	Element@ first_child
	{
		get { return num_children > 0 ? @children[0] : null; }
	}
	
	Element@ last_child
	{
		get { return num_children > 0 ? @children[num_children - 1] : null; }
	}
	
	Element@ get_child(int index)
	{
		if(index < 0 || index >= num_children)
			return null;
		
		return @children[index];
	}
	
	int child_count { get const { return num_children; } }
	
	/**
	 * @brief Required call after modifying layout properties
	 */
	void update_layout()
	{
		_validate_layout = true;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		stack.push_reversed(@children);
	}
	
	void _do_layout(LayoutContext@ ctx)
	{
		if(@_layout_handler != null)
		{
			_layout_handler.do_child_layout(ctx, this);
		}
		else
		{
			_do_layout_internal(ctx);
		}
	}
	
	void _do_layout_internal(LayoutContext@ ctx)
	{
		_scrolled_into_view = false;
		
		if(@scroll_into_view != null)
		{
			bool scroll_changed = false;
			
			if(@scroll_into_view.parent == @this)
			{
				if(scroll_into_view.x1 < x1 + ui.style.spacing)
				{
					_scroll_x += x1 + ui.style.spacing - scroll_into_view.x1;
					scroll_changed = true;
				}
				else if(scroll_into_view.x2 > x2 - ui.style.spacing)
				{
					_scroll_x += x2 - ui.style.spacing - scroll_into_view.x2;
					scroll_changed = true;
				}
				
				if(scroll_into_view.y1 < y1 + ui.style.spacing)
				{
					_scroll_y += y1 + ui.style.spacing - scroll_into_view.y1;
					scroll_changed = true;
				}
				else if(scroll_into_view.y2 > y2 - ui.style.spacing)
				{
					_scroll_y += y2 - ui.style.spacing - scroll_into_view.y2;
					scroll_changed = true;
				}
			}
			
			_scrolled_into_view = scroll_changed;
			@scroll_into_view = null;
		}
		
		if(_validate_layout)
		{
			if(@_layout != null)
			{
				_layout.do_layout(@children,
					0, 0, _width, _height,
					scroll_min_x, scroll_min_y, scroll_max_x, scroll_max_y,x1,y1);
			}
			else if(num_children > 0)
			{
				Element@ element = children[0];
				
				scroll_min_x = element._x;
				scroll_min_y = element._y;
				scroll_max_x = element._x + element._width;
				scroll_max_y = element._y + element._height;
				
				for(int i = 1; i < num_children; i++)
				{
					@element = children[i];
					
					if(element._x < scroll_min_x)
						scroll_min_x = element._x;
					if(element._y < scroll_min_y)
						scroll_min_y = element._y;
					if(element._x + element._width > scroll_max_x)
						scroll_max_x = element._x + element._width;
					if(element._y + element._height> scroll_max_y)
						scroll_max_y = element._y + element._height;
				}
			}
			else
			{
				scroll_min_x = 0;
				scroll_min_y = 0;
				scroll_max_x = 0;
				scroll_max_y = 0;
			}
		}
	}
	
}