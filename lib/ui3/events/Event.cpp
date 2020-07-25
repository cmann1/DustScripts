#include 'EventInfo.cpp';
#include 'EventType.cpp';

funcdef void EventCallback(EventInfo@);

class Event
{
	
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
//		puts(event.type + ' ' + event.target._id);
		
		for(int i = 0; i < num_listeners; i++)
		{
			listeners[i](event);
		}
	}
	
}