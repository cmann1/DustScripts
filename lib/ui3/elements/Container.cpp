#include 'Element.cpp';

/**
 * @class Container
 * @brief Any element that can contain other elements
 */
class Container : Element
{
	
	protected array<Element@> children;
	protected int num_children;
	
	Container(UI@ ui, const string &in type_identifier = 'cnt')
	{
		super(ui, type_identifier);
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
	
	// TODO: Methods to change child order
	
	void clear()
	{
		children.resize(0);
	}
	
	void do_layout(const float &in parent_x, const float &in parent_y)
	{
		Element::do_layout(parent_x, parent_y);
		
		// TODO: Layouts
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		stack.push_reversed(children);
	}
	
	void draw(const Graphics@ &in graphics, const float &in sub_frame) override
	{
		Element::draw(graphics, sub_frame);
		
		for(uint i = 0, count = children.size(); i < count; i++)
		{
			Element@ element = children[i];
			
			if(element.visible)
			{
				element.draw(graphics, sub_frame);
			}
		}
	}
	
}