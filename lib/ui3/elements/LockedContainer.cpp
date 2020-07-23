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
		puts('add_child not supporte');
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		puts('remove_child not supported');
		return false;
	}
	
	protected bool _remove_child_internal(Element@ child) override
	{
		puts('_remove_child_internal not supported.');
		return false;
	}
	
}