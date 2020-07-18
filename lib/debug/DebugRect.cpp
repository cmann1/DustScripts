#include '../drawing/common.cpp';
#include '../drawing/common.cpp';

class DebugRect : DebugItem
{
	
	float x1;
	float y1;
	float x2;
	float y2;
	float rotation;
	float thickness;
	uint colour;
	
	void set(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation=0, float thickness=-1, uint colour=0xFFFFFFFF, bool world=true, int frames=1)
	{
		DebugItem::set(layer, sub_layer, frames, world);
		
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.rotation = rotation;
		this.thickness = thickness;
		this.colour = colour;
	}
	
	void draw(scene@ g)
	{
		if(thickness <= 0)
		{
			if(world)
				g.draw_rectangle_world(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
			else
				g.draw_rectangle_hud(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
		}
		else
		{
			if(rotation == 0)
			{
				if(world)
					outline_rect(g, layer, sub_layer, x1, y1, x2, y2, thickness, colour);
				else
					outline_rect_hud(g, layer, sub_layer, x1, y1, x2, y2, thickness, colour);
			}
			else
			{
				if(world)
					outline_rotated_rect(g, layer, sub_layer,
						(x1 + x2) * 0.5, (y1 + y2) * 0.5,
						(x2 - x1) * 0.5, (y2 - y1) * 0.5, rotation, thickness, colour);
				else
					outline_rotated_rect_hud(g, layer, sub_layer,
						(x1 + x2) * 0.5, (y1 + y2) * 0.5,
						(x2 - x1) * 0.5, (y2 - y1) * 0.5, rotation, thickness, colour);
			}
		}
	}
	
}

class DebugRectPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<DebugRect@> pool(pool_size);
	
	DebugRect@ get()
	{
		if(pool_index > 0)
			return pool[--pool_index];
		
		return DebugRect();
	}
	
	void release(DebugRect@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = obj;
	}
	
}