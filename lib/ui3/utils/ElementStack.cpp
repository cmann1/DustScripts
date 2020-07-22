class ElementStack
{
	
	private array<Element@> stack;
	private int size;
	private int index;
	
	void push(Element@ element)
	{
		if(index == size)
		{
			size += 16;
			stack.resize(size);
		}
		
		@stack[index++] = element;
	}
	
	void push(array<Element@> elements)
	{
		const int count = int(elements.size());
		
		if(index + count > size)
		{
			size = index + count + 16;
			stack.resize(size);
		}
		
		for(int i = 0; i < count; i++)
		{
			@stack[index++] = elements[i];
		}
	}
	
	void push_reversed(array<Element@> elements)
	{
		const int count = int(elements.size());
		
		if(index + count > size)
		{
			size = index + count + 16;
			stack.resize(size);
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
	
	void clear()
	{
		index = 0;
	}
	
}