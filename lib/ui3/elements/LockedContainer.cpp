#include '../UI.cpp';
#include '../utils/ElementStack.cpp';
#include 'Element.cpp';
#include 'LockedContainer.cpp';

abstract class LockedContainer : Container
{
	
	/**
	 * @class LockedContainer
	 * @brief A container that manages its own children, e.g. a Button or Tooltip
	 */
	 
	LockedContainer(UI@ ui)
	{
		super(ui);
	}
	
	bool add_child(Element@ child, int index=-1) override
	{
		puts('add_child not supported');
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		puts('remove_child not supported');
		return false;
	}
	
	void set_child_index(Element@ child, int index) override
	{
		puts('set_child_index not supported');
	}
	
	void move_to_front(Element@ child) override
	{
		puts('move_to_front not supported');
	}
	
	void move_to_back(Element@ child) override
	{
		puts('move_to_back not supported');
	}
	
	void move_up(Element@ child) override
	{
		puts('move_up not supported');
	}
	
	void move_down(Element@ child) override
	{
		puts('move_down not supported');
	}
	
	void clear() override
	{
		puts('clear not supported');
	}
	
}
