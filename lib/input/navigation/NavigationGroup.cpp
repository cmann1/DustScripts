#include 'INavigable.cpp';
#include 'NavigationGroupItem.cpp';
#include 'navigation.cpp';
#include 'NavigationDirection.cpp';

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
	
	INavigable@ previous_navigable(INavigable@ from, INavigable@ _initial=null) override
	{
		return next_navigable(from, _initial, Backward);
	}
	
	INavigable@ next_navigable(INavigable@ from, INavigable@ _initial=null) override
	{
		return next_navigable(from, _initial, Forward);
	}
	
	protected INavigable@ next_navigable(INavigable@ from, INavigable@ _initial, const NavigationDirection dir)
	{
		if(@from == null)
			return null;
		
		// Came back to the navigable that initiated navigation.
		// Unlikely to happen but check to prevent an infinite loop.
		if(@_initial == @from)
			return _initial;
		
		if(@_initial == null)
			@_initial = from;
		
		NavigationGroupItem@ next = null;
		NavigationGroupItem@ start_item = dir == Forward ? first_item : last_item;
		
		// Coming into this group from outside - move to the first
		if(@from.navigation_parent != @this)
		{
			if(@first_item == null)
				return @_parent != null
					? _parent.next_navigable(this, _initial, dir)
					: null;
			
			@next = start_item;
		}
		// Navigating from the last element in this group.
		else if(is_end(from, dir))
		{
			// Don't wrap so pass back up to parent.
			if(!wrap)
				return @_parent != null
					? @_parent.next_navigable(this, _initial, dir)
					: null;
			
			// Wrap to first element.
			@next = start_item;
		}
		
		if(@next == null)
		{
			@next = cast<NavigationGroupItem@>(elements_map[from.id]).sibling(dir);
		}
		
		NavigationGroupItem@ start = null;
		
		// Elements may not be visible or be disabled. Keep looking until we reach the start again or a navigable element is found.
		while(true)
		{
			// No navigable elements found, pass back up to parent.
			if(@next == null || @next == @start)
				return @_parent != null ? @_parent.next_navigable(this, _initial, dir) : null;
			
			if(next.element.can_navigate_to)
				return next.element;
			
			// Assign start after first check above to prevent first element from returning on first iteration.
			if(@start == @null)
			{
				@start = next;
			}
			
			// Go to next and wrap if necessary.
			@next = next.sibling(dir);
			if(@next == null && wrap)
			{
				@next = start_item;
			}
		}
		
		return null;
	}
	
	private bool is_end(INavigable@ item, NavigationDirection dir)
	{
		NavigationGroupItem@ check = dir == Forward ? last_item : first_item;
		dir = dir == Forward ? Backward : Forward;
		
		while(@check != null)
		{
			if(@item == @check.element)
				return true;
			if(check.element.can_navigate_to)
				return false;
			
			@check = check.sibling(dir);
		}
		
		return false;
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
