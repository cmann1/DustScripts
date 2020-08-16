#include 'INavigable.cpp';

class NavigationGroupItem
{
	
	INavigable@ element;
	NavigationGroupItem@ previous;
	NavigationGroupItem@ next;
	
	NavigationGroupItem(INavigable@ element)
	{
		@this.element = @element;
	}
	
}