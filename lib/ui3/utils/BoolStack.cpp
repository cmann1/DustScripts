class BoolStack
{
	
	private array<bool> stack;
	private int _size;
	private int index;
	
	void push(bool element)
	{
		if(index == _size)
		{
			_size += 16;
			stack.resize(_size);
		}
		
		stack[index++] = element;
	}
	
	void push(const array<bool>@ &in elements)
	{
		const int count = int(elements.size());
		
		if(index + count > _size)
		{
			_size = index + count + 16;
			stack.resize(_size);
		}
		
		for(int i = 0; i < count; i++)
		{
			stack[index++] = elements[i];
		}
	}
	
	void push_reversed(const array<bool>@ &in elements)
	{
		const int count = int(elements.size());
		
		if(index + count > _size)
		{
			_size = index + count + 16;
			stack.resize(_size);
		}
		
		for(int i = count - 1; i >= 0; i--)
		{
			stack[index++] = elements[i];
		}
	}
	
	void push(const BoolStack@ &in other_stack)
	{
		if(index + other_stack.index > _size)
		{
			_size = index + other_stack.index + 16;
			stack.resize(_size);
		}
		
		for(int i = 0; i < other_stack.index; i++)
		{
			stack[index++] = other_stack.stack[i];
		}
	}
	
	bool pop()
	{
		if(index == 0)
			return false;
		
		return stack[--index];
	}
	
	bool peek()
	{
		if(index == 0)
			return false;
		
		return stack[index - 1];
	}
	
	void clear()
	{
		index = 0;
	}
	
	int size { get { return index; } }
	
}