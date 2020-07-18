#include '../drawing/common.cpp';
#include 'DebugItem.cpp';

class DebugLine : DebugItem
{
	float x1;
	float y1;
	float x2;
	float y2;
	float thickness;
	uint colour;
	
	void set(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, int frames=1, bool world=true)
	{
		DebugItem::set(layer, sub_layer, frames, world);
		
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.thickness = thickness;
		this.colour = colour;
	}
	
	void draw(scene@ g)
	{
		draw_line(g, layer, sub_layer, x1, y1, x2, y2, thickness, colour, world);
	}
	
}

class DebugLinePool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<DebugLine@> pool(pool_size);
	
	DebugLine@ get()
	{
		if(pool_index > 0)
			return pool[--pool_index];
		
		return DebugLine();
	}
	
	void release(DebugLine@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = obj;
	}
	
}