class LockedContainer : Container
{
	
	/**
	 * @class LockedContainer
	 * @brief A container that manages its own children, e.g. a Button or Tooltip
	 */
	 
	LockedContainer(UI@ ui, const string &in type_identifier)
	{
		super(ui, type_identifier);
	}
	
	bool add_child(Element@ child) override
	{
		puts('add_child not supported');
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		puts('remove_child not supported');
		return false;
	}
	
	void set_child_index(Element@ child, int index)
	{
		puts('set_child_index not supported');
	}
	
	void move_to_front(Element@ child)
	{
		puts('move_to_front not supported');
	}
	
	void move_to_back(Element@ child)
	{
		puts('move_to_back not supported');
	}
	
	void move_up(Element@ child)
	{
		puts('move_up not supported');
	}
	
	void move_down(Element@ child)
	{
		puts('move_down not supported');
	}
	
	
}