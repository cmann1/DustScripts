#include '../layouts/Layout.cpp';
#include '../utils/ILayoutParentHandler.cpp';
#include 'Element.cpp';

class Container : Element
{
	/**
	 * @class Container
	 * @brief Any element that can contain other elements
	 */
	
	bool _validate_layout = true;
	float scroll_min_x;
	float scroll_min_y;
	float scroll_max_x;
	float scroll_max_y;
	
	ILayoutParentHandler@ _layout_handler;
	
	float _scroll_x;
	float _scroll_y;
	
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
	
	float scroll_x
	{
		get const { return _scroll_x; }
		set
		{
			if(_scroll_x == value) return;
			_scroll_x = value;
			_validate_layout = true;
		}
	}
	
	float scroll_y
	{
		get const { return _scroll_y; }
		set
		{
			if(_scroll_y == value) return;
			_scroll_y = value;
			_validate_layout = true;
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
		if(_validate_layout)
		{
//			puts(_id+'.zvalidate_layout');
			if(@_layout != null)
			{
				_layout.do_layout(@children,
					_scroll_x, _scroll_y, _scroll_x + _width, _scroll_y + _height,
					scroll_min_x, scroll_min_y, scroll_max_x, scroll_max_y);
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
			
			_validate_layout = false;
		}
	}
	
}