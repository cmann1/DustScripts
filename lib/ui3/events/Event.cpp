#include 'EventInfo.cpp';
#include 'EventType.cpp';

funcdef void EventCallback(EventInfo@);

/// A UI event.
/// At the moment adding function delegates causes some kind of memory leak. As a workaround until that is sorted
/// listeners are stored in predefined fields, limiting the amount of listeners on a single event to at most four.
class Event
{
	
	bool enabled = true;
	
	private array<EventCallback@> listeners;
	private int num_listeners;
	
	void on(EventCallback@ callback)
	{
		listeners.insertLast(callback);
		num_listeners++;
	}
	
	void off(EventCallback@ callback)
	{
		int index = listeners.findByRef(callback);
		
		if(index >= 0)
		{
			@listeners[index] = @listeners[--num_listeners];
			listeners.resize(num_listeners);
		}
	}
	
	void dispatch(EventInfo@ event)
	{
		if(!enabled)
			return;
		
//		puts(event.type + (@event.target != null ? ' ' + event.target._id : ''));
		
		for(int i = 0; i < num_listeners; i++)
		{
			listeners[i](event);
		}
	}
	
}