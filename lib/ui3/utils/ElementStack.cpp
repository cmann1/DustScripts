class ElementStack
{
	
	private array<Element@> stack;
	private int _size;
	private int index;
	
	void push(Element@ element)
	{
		if(index == _size)
		{
			_size += 16;
			stack.resize(_size);
		}
		
		@stack[index++] = element;
	}
	
	void push(const array<Element@>@ &in elements)
	{
		const int count = int(elements.size());
		
		if(index + count > _size)
		{
			_size = index + count + 16;
			stack.resize(_size);
		}
		
		for(int i = 0; i < count; i++)
		{
			@stack[index++] = elements[i];
		}
	}
	
	void push_reversed(const array<Element@>@ &in elements)
	{
		const int count = int(elements.size());
		
		if(index + count > _size)
		{
			_size = index + count + 16;
			stack.resize(_size);
		}
		
		for(int i = count - 1; i >= 0; i--)
		{
			@stack[index++] = elements[i];
		}
	}
	
	Element@ pop()
	{
		if(index == 0)
			return null;
		
		return stack[--index];
	}
	
	Element@ peek()
	{
		if(index == 0)
			return null;
		
		return stack[index];
	}
	
	void clear()
	{
		index = 0;
	}
	
	int size { get { return index; } }
	
}