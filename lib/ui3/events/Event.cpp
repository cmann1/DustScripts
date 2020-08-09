#include 'EventInfo.cpp';
#include 'EventType.cpp';

funcdef void EventCallback(EventInfo@);

/// A UI event.
/// At the moment adding function delegates causes some kind of memory leak. As a workaround until that is sorted
/// listeners are stored in predefined fields, limiting the amount of listeners on a single event to at most four.
class Event
{
	
//	private array<EventCallback@> listeners;
	private EventCallback@ listener1;
	private EventCallback@ listener2;
	private EventCallback@ listener3;
	private EventCallback@ listener4;
	private int num_listeners;
	
	void on(EventCallback@ callback)
	{
		if(@listener1 == @callback || @listener2 == @callback || @listener3 == @callback || @listener4 == @callback)
			return;
		
		switch(num_listeners)
		{
			case 0: @listener1 = callback; break;
			case 1: @listener2 = callback; break;
			case 2: @listener3 = callback; break;
			case 3: @listener4 = callback; break;
			case 4: puts('Max number of listeners reached!'); return;
		}
		
//		listeners.insertLast(callback);
		num_listeners++;
	}
	
	void off(EventCallback@ callback)
	{
		if(@listener4 == @callback)
		{
			@listener4 = null;
			num_listeners--;
		}
		else if(@listener3 == @callback)
		{
			@listener3 = @listener4;
			@listener4 = null;
			num_listeners--;
		}
		else if(@listener2 == @callback)
		{
			@listener2 = @listener3;
			@listener3 = @listener4;
			@listener4 = null;
			num_listeners--;
		}
		else if(@listener1 == @callback)
		{
			@listener1 = @listener2;
			@listener2 = @listener3;
			@listener3 = @listener4;
			@listener4 = null;
			num_listeners--;
		}
		
//		int index = listeners.findByRef(callback);
//		
//		if(index >= 0)
//		{
//			@listeners[index] = @listeners[--num_listeners];
//			listeners.resize(num_listeners);
//		}
	}
	
	void dispatch(EventInfo@ event)
	{
//		puts(event.type + ' ' + event.target._id);
		
		if(num_listeners >= 1)
			listener1(event);
		if(num_listeners >= 2)
			listener2(event);
		if(num_listeners >= 3)
			listener3(event);
		if(num_listeners >= 4)
			listener4(event);
		
//		for(int i = 0; i < num_listeners; i++)
//		{
//			listeners[i](event);
//		}
	}
	
}