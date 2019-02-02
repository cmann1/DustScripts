class Vec2
{
	
	float x, y;
	
	Vec2(float x=0, float y=0)
	{
		this.x = x;
		this.y = y;
	}
	
	bool opEquals(const Vec2 &in other)
	{
		return x == other.x && y == other.y;
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
	
}