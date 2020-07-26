#include 'GenericEventInfo.cpp';
#include 'EventType.cpp';

funcdef void GenericEventCallback(GenericEventInfo@);

class GenericEvent
{
	
	private array<GenericEventCallback@> listeners;
	private int num_listeners;
	
	void on(GenericEventCallback@ callback)
	{
		listeners.insertLast(callback);
		num_listeners++;
	}
	
	void off(GenericEventCallback@ callback)
	{
		int index = listeners.findByRef(callback);
		
		if(index >= 0)
		{
			@listeners[index] = @listeners[--num_listeners];
			listeners.resize(num_listeners);
		}
	}
	
	void dispatch(GenericEventInfo@ event)
	{
//		puts(event.type + ' ' + event.target._id);
		
		for(int i = 0; i < num_listeners; i++)
		{
			listeners[i](event);
		}
	}
	
}