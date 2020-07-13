class TileCachChunkQueue
{
	
	private uint max__size;
	private uint _size;
	private array<string> queue;
	private int front;
	private int back;
	
	TileCachChunkQueue(uint max__size)
	{
		this.max__size = max__size;
		queue.resize(max__size);
	}
	
	int size
	{
		get const
		{
			return _size;
		}
	}
	
	bool is_full()
	{
		return _size == max__size;
	}
	
	bool enqueue(string item)
	{
		if(_size == max__size)
			return false;
		
		if(_size == 0)
		{
			front = back = 0;
			queue[0] = item;
			_size++;
			
			return true;
		}
		
		back = (back + 1) % max__size;
		queue[back] = item;
		_size++;
		
		return true;
	}
	
	bool dequeue(string &out item)
	{
		if(_size == 0)
			return false;
		
		if(_size == 1)
		{
			item = queue[front];
			front = back = -1;
			_size = 0;
			
			return true;
		}
		
		item = queue[front];
		front = (front + 1) % max__size;
		_size--;
		
		return true;
	}
	
	bool peek(string &out item)
	{
		if(_size == 0)
			return false;
		
		item = queue[front];
		return true;
	}
	
	void clear()
	{
		_size = 0;
		front = back = -1;
	}
	
}