#include '../layouts/Layout.cpp';
#include 'Element.cpp';

class Container : Element
{
	/**
	 * @class Container
	 * @brief Any element that can contain other elements
	 */
	
	protected array<Element@> children;
	protected int num_children;
	
	protected Layout@ _layout;
	
	Container(UI@ ui, const string &in type_identifier = 'cnt')
	{
		super(ui, type_identifier);
	}
	
	Layout@ layout
	{
		get { return _layout; }
		set { @_layout = value; }
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
	}
	
	void clear()
	{
		children.resize(0);
	}
	
	void do_layout(const float parent_x, const float parent_y)
	{
		Element::do_layout(parent_x, parent_y);
		
		// TODO: Layouts
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		stack.push_reversed(@children);
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(num_children == 0)
			return;
		
		if(disabled || alpha != 1)
			style.disable_alpha();
		
		for(int i = 0; i < num_children; i++)
		{
			children[i].draw(style, sub_frame);
		}
		
		if(disabled || alpha != 1)
			style.restore_alpha();
	}
	
}