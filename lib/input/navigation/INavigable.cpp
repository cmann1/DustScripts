#include 'NavigationGroup.cpp';
#include 'NavigateOn.cpp';

interface INavigable
{
	
	string id { get const; }
	
	bool can_navigate_to { get const; }
	
	/// Internal - don't set explicitly.
	NavigationGroup@ navigation_parent { get; set; }
	
	NavigateOn navigate_on { get const; }
	
	INavigable@ previous_navigable(INavigable@ from);
	
	INavigable@ next_navigable(INavigable@ from);
	
}
