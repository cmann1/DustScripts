class Vec2
{
	
	float x, y;
	
	Vec2(float x=0, float y=0)
	{
		this.x = x;
		this.y = y;
	}
	
	Vec2(entity@ &in e)
	{
		x = e.x();
		y = e.y();
	}
	
	float magnitude()
	{
		return sqrt(x * x + y * y);
	}
	
	float sqr_magnitude()
	{
		return x * x + y * y;
	}
	
	void normalise()
	{
		float length = sqrt(x * x + y * y);
		
		if(length != 0)
		{
			x /= length;
			y /= length;
		}
		else
		{
			x = y = 0;
		}
	}
	
	bool opEquals(const Vec2 &in other)
	{
		return x == other.x && y == other.y;
	}
	
	Vec2@ opDivAssign(const float &in v)
	{
		if(v != 0)
		{
			x /= v;
			y /= v;
		}
		else
		{
			x = y = 0;
		}
		
		return this;
	}
	
	Vec2@ opMulAssign(const float &in v)
	{
		x *= v;
		y *= v;
		return this;
	}
	
	bool equals(const float &in x, const float &in y)
	{
		return this.x == x && this.y == y;
	}
	
	void set(const float &in x, const float &in y)
	{
		this.x = x;
		this.y = y;
	}
	
	void set(const Vec2 &in v)
	{
		x = v.x;
		y = v.y;
	}
	
	void set(entity@ &in e)
	{
		x = e.x();
		y = e.y();
	}
	
}