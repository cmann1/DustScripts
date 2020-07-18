#include 'DebugItem.cpp';

class DebugItemList
{
	
	DebugItem@ first;
	DebugItem@ last;
	int count;
	
	void insert(DebugItem@ item)
	{
		if(@last == null)
		{
			@first = @last = item;
			count = 1;
		}
		else
		{
			@last.next = item;
			@item.prev = last;
			@last = item;
			
			count++;
		}
	}
	
	void remove(DebugItem@ item)
	{
		if(@item.prev != null)
			@item.prev.next = item.next;
		else
			@first = item.next;
		
		if(@item.next != null)
			@item.next.prev = item.prev;
		else
			@last = item.prev;
		
		@item.prev = null;
		@item.next = null;
	}
	
}