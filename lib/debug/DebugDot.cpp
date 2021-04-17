#include '../drawing/common.cpp';
#include 'DebugItem.cpp';

class DebugDot : DebugItem
{
	float x;
	float y;
	float size;
	float rotation;
	uint colour;
	
	void set(uint layer, uint sub_layer,
		float x, float y,
		float size, uint colour, float rotation, bool world=true, int frames=1)
	{
		DebugItem::set(layer, sub_layer, frames, world);
		
		this.x = x;
		this.y = y;
		this.size = size;
		this.colour = colour;
		this.rotation = rotation;
	}
	
	void draw(scene@ g)
	{
		if(world)
			g.draw_rectangle_world(layer, sub_layer,
				x - size, y - size,
				x + size, y + size,
				rotation, colour);
		else
			g.draw_rectangle_hud(layer, sub_layer,
				x - size, y - size,
				x + size, y + size,
				rotation, colour);
	}
	
}

class DebugDotPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<DebugDot@> pool(pool_size);
	
	DebugDot@ get()
	{
		if(pool_index > 0)
			return pool[--pool_index];
		
		return DebugDot();
	}
	
	void release(DebugDot@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = obj;
	}
	
}
