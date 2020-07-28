#include '../../events/EventInfo.cpp';

class EventInfoPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<EventInfo@> pool(pool_size);
	
	EventInfo@ get()
	{
		return pool_index > 0 ? @pool[--pool_index] : EventInfo();
	}
	
	void release(EventInfo@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = @obj;
	}
	
}