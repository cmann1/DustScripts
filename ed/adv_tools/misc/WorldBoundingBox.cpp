#include 'IWorldBoundingBox.cpp';

class WorldBoundingBox : IWorldBoundingBox
{
	
	float x1, y1;
	float x2, y2;
	
	private int count;
	
	float get_world_x1() override
	{
		return x1;
	}
	
	float get_world_y1() override
	{
		return y1;
	}
	
	float get_world_x2() override
	{
		return x2;
	}
	
	float get_world_y2() override
	{
		return y2;
	}
	
	void reset()
	{
		count = 0;
	}
	
	void add(const float x1, const float y1, const float x2, const float y2)
	{
		if(count++ == 0)
		{
			this.x1 = x1;
			this.y1 = y1;
			this.x2 = x2;
			this.y2 = y2;
		}
		else
		{
			if(x1 < this.x1) this.x1 = x1;
			if(y1 < this.y1) this.y1 = y1;
			if(x2 > this.x2) this.x2 = x2;
			if(y2 > this.y2) this.y2 = y2;
		}
	}
	
}