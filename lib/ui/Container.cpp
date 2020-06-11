#include 'Element.cpp';

class Container : Element
{
	
	array<Element@> children;
	
	Container(UI@ ui)
	{
		super(ui);
	}
	
	bool addChild(Element@ child)
	{
		if(child is null or @child.parent == @this)
			return false;
		
		children.insertLast(child);
		@child.parent = @this;
		return true;
	}
	
	bool removeChild(Element@ child)
	{
		if(child is null or @child.parent != @this)
			return false;
		
		int index = children.findByRef(child);
		
		if(index < 0)
			return false;
		
		children.removeAt(index);
		@child.parent = null;
		return true;
	}
	
	void clear()
	{
		children.resize(0);
	}
	
}