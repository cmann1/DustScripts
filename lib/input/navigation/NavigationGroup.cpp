#include 'INavigable.cpp';
#include 'NavigationGroupItem.cpp';
#include 'navigation.cpp';

class NavigationGroup : INavigable
{
	
	/// When the first or last element is reached will wrap around.
	bool wrap = true;
	
	private UI@ ui;
	private string _id;
	private NavigationGroup@ _parent;
	
	private NavigateOn _navigate_on = NavigateOn(Inherit | Tab | Escape);
	
	private dictionary elements_map;
	private NavigationGroupItem@ first_item;
	private NavigationGroupItem@ last_item;
	
	NavigationGroup(UI@ ui)
	{
		@this.ui = ui;
		_id = (++ui.NEXT_ID) + '';
	}
	
	string id
	{
		get const { return _id; }
	}
	
	NavigationGroup@ navigation_parent
	{
		get { return @_parent; }
		set { @_parent = @value; }
	}
	
	NavigateOn navigate_on
	{
		get const { return navigation::get(_navigate_on, _parent); }
		set { _navigate_on = value; }
	}
	
	bool can_navigate_to {
		get const { return true; }
	}
	
	INavigable@ previous_navigable(INavigable@ from)
	{
		if(@from == null || @from.navigation_parent != @this)
			return null;
		
		if(@from == @first_item.element)
			return @_parent != null ? @_parent.previous_navigable(@this) : (wrap ? @last_item.element : null);
		
		NavigationGroupItem@ item = cast<NavigationGroupItem@>(elements_map[from.id]);
		
		if(@item == null)
			return null;
		
		if(@item.previous == null && @_parent != null)
			return @_parent.previous_navigable(@this);
		
		return check_next(item, -1);
	}
	
	INavigable@ next_navigable(INavigable@ from)
	{
		if(@from == null || @from.navigation_parent != @this)
			return null;
		
		if(@from == @last_item.element)
			return @_parent != null ? @_parent.next_navigable(@this) : (wrap ? @first_item.element : null);
		
		NavigationGroupItem@ item = cast<NavigationGroupItem@>(elements_map[from.id]);
		
		if(@item == null)
			return null;
		
		if(@item.next == null && @_parent != null)
			return @_parent.next_navigable(@this);
		
		return check_next(item, 1);
	}
	
	private INavigable@ check_next(NavigationGroupItem@ item, const int dir)
	{
		NavigationGroupItem@ from_item = item;
		
		do
		{
			@item = (dir >= 0 ? item.next : item.previous);
			if(@item.element != null && item.element.can_navigate_to)
				return item.element;
			
			if(@item.next == null)
				return null;
		}
		while(@item != null && @item != @from_item);
		
		return null;
	}
	
	void add_first(INavigable@ element)
	{
		if(@element == null)
			return;
		
		if(@element.navigation_parent != null)
		{
			element.navigation_parent.remove(@element);
		}
		
		NavigationGroupItem@ item = NavigationGroupItem(element);
		@elements_map[element.id] = @item;
		
		if(@first_item == null)
		{
			@first_item = @item;
			@last_item = @item;
			return;
		}
		
		@element.navigation_parent = @this;
		@first_item.previous = @item;
		@item.next = @first_item;
		@first_item = @item;
	}
	
	void add_last(INavigable@ element)
	{
		if(@element == null)
			return;
		
		if(@element.navigation_parent != null)
		{
			element.navigation_parent.remove(@element);
		}
		
		NavigationGroupItem@ item = NavigationGroupItem(element);
		@elements_map[element.id] = @item;
		@element.navigation_parent = @this;
		
		if(@last_item == null)
		{
			@first_item = @item;
			@last_item = @item;
			return;
		}
		
		@last_item.next = @item;
		@item.previous = @last_item;
		@last_item = @item;
	}
	
	void add_before(INavigable@ element, INavigable@ other)
	{
		if(@element == null)
			return;
		
		if(@other == null)
		{
			add_first(@element);
			return;
		}
		
		if(@other.navigation_parent != @this)
			return;
		
		if(@element.navigation_parent != null)
		{
			element.navigation_parent.remove(@element);
		}
		
		NavigationGroupItem@ item = NavigationGroupItem(element);
		NavigationGroupItem@ other_item = cast<NavigationGroupItem@>(elements_map[other.id]);
		@elements_map[element.id] = @item;
		
		@element.navigation_parent = @this;
		if(@other_item.previous != null)
		{
			@other_item.previous.next = @item;
		}
		@other_item.previous = @item;
		@item.next = @other_item;
		
		if(@other_item == @first_item)
		{
			@first_item = @item;
		}
	}
	
	void add_after(INavigable@ element, INavigable@ other)
	{
		if(@element == null)
			return;
		
		if(@other == null)
		{
			add_first(@element);
			return;
		}
		
		if(@other.navigation_parent != @this)
			return;
		
		if(@element.navigation_parent != null)
		{
			element.navigation_parent.remove(@element);
		}
		
		NavigationGroupItem@ item = NavigationGroupItem(element);
		NavigationGroupItem@ other_item = cast<NavigationGroupItem@>(elements_map[other.id]);
		@elements_map[element.id] = @item;
		
		@element.navigation_parent = @this;
		if(@other_item.next != null)
		{
			@other_item.next.previous = @item;
		}
		@other_item.next = @item;
		@item.previous = @other_item;
		
		if(@other_item == @last_item)
		{
			@last_item = @item;
		}
	}
	
	void remove(INavigable@ element)
	{
		if(@element == null || @element.navigation_parent != @this)
			return;
		
		NavigationGroupItem@ item = cast<NavigationGroupItem@>(elements_map[element.id]);
		elements_map.delete(id);
		@element.navigation_parent = null;
		
		if(@item.previous != null)
		{
			@item.previous.next = @item.next;
		}
		
		if(@item.next != null)
		{
			@item.next.previous = @item.previous;
		}
		
		if(@item == @first_item)
		{
			@first_item = @item.next;
		}
		
		if(@item == @last_item)
		{
			@last_item = @item.previous;
		}
	}
	
	void clear()
	{
		elements_map.deleteAll();
		
		NavigationGroupItem@ item = @first_item;
		
		while(@item != null)
		{
			@item.element.navigation_parent = null;
			@item = @item.next;
		}
		
		@first_item = null;
		@last_item = null;
	}
	
	void print_items()
	{
		puts('NavigationGroupItems:');
		
		NavigationGroupItem@ item = first_item;
		int i = 0;
		
		while(@item != null)
		{
			Element@ el = cast<Element@>(item.element);
			
			if(@el != null)
			{
				puts('  ' + i + ' ' + el.element_type + '[' + el.name + ']');
			}
			
			@item = item.next;
			i++;
		}
	}
	
}
