#include '../UI.cpp';
#include '../../math/math.cpp';
#include '../utils/ElementStack.cpp';
#include 'Element.cpp';
#include 'LockedContainer.cpp';

namespace SingleContainer { const string TYPE_NAME = 'SingleContainer'; }

class SingleContainer : LockedContainer
{
	
	/**
	 * @class LockedContainer
	 * @brief A Container that can only hold a single child
	 */
	
	protected Element@ _content;
	
	SingleContainer(UI@ ui, Element@ content)
	{
		super(ui);
		
		@this.content = content;
	}
	
	string element_type { get const override { return SingleContainer::TYPE_NAME; } }
	
	Element@ content
	{
		get { return @_content; }
		set
		{
			if(@_content == @value)
				return;
			
			if(@_content != null)
			{
				Container::remove_child(_content);
			}
			
			@_content = @value;
			
			if(@_content != null)
			{
				Container::add_child(_content);
			}
			
			validate_layout = true;
		}
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(@_content != null)
		{
			stack.push(_content);
		}
	}
	
	bool add_child(Element@ child, int index=-1) override
	{
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		if(Container::remove_child(child))
		{
			@_content = null;
			return true;
		}
		
		return false;
	}
	
}
