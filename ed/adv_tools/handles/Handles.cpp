#include 'Handle.cpp';

class Handles
{
	
	private AdvToolScript@ script;
	
	private int handle_pool_size;
	private int handle_pool_count;
	private array<Handle@> handle_pool;
	
	private int handles_size = 32;
	private int handles_count;
	private array<Handle@> handles(handles_size);
	
	private bool has_hit_handle;
	
	bool mouse_over;
	bool mouse_over_last_handle;
	
	void init(AdvToolScript@ script)
	{
		@this.script = script;
	}
	
	bool handle(const HandleShape shape, const float x, const float y, const float size, const float rotation, const uint colour, const uint highlight_colour, const bool force_highlight)
	{
		Handle@ handle = handle_pool_count > 0
			? handle_pool[--handle_pool_count]
			: Handle();
		
		if(handles_count == handles_size)
		{
			handles.resize(handles_size += 32);
		}
		
		@handles[handles_count++] = handle;
		handle.init(shape, x, y, size, rotation, colour, highlight_colour);
		mouse_over_last_handle = false;
		
		if(script.mouse_in_scene && handle.hit_test(script, script.mouse.x, script.mouse.y))
		{
			mouse_over = true;
			mouse_over_last_handle = true;
			
			if(has_hit_handle)
			{
				handle.hit = force_highlight;
				return false;
			}
			
			has_hit_handle = true;
			return script.mouse.left_press && !script.space;
		}
		
		handle.hit = force_highlight;
		return false;
	}
	
	bool circle(const float x, const float y, const float size, const uint colour, const uint highlight_colour, const bool force_highlight=false)
	{
		return handle(HandleShape::Circle, x, y, size, 0, colour, highlight_colour, force_highlight);
	}
	
	bool square(const float x, const float y, const float size, const float rotation, const uint colour, const uint highlight_colour, const bool force_highlight=false)
	{
		return handle(HandleShape::Square, x, y, size, rotation, colour, highlight_colour, force_highlight);
	}
	
	Handle@ get_last_handle()
	{
		return handles_count > 0
			? handles[handles_count - 1]
			: null;
	}
	
	void step()
	{
		if(handle_pool_count + handles_count >= handle_pool_size)
		{
			handle_pool.resize(handle_pool_size = handle_pool_count + handles_count + 32);
		}
		
		for(int i = 0; i < handles_count; i++)
		{
			@handle_pool[handle_pool_count++] = @handles[i];
		}
		
		handles_count = 0;
		mouse_over = false;
		has_hit_handle = false;
	}
	
	void draw()
	{
		for(int i = 0; i < handles_count; i++)
		{
			handles[i].draw(script);
		}
	}
	
}