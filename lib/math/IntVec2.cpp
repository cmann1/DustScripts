#include "Vec2.cpp"

int orientation(const IntVec2 &in v1, const IntVec2 &in v2, const IntVec2 &in v3)
{
	return (v3.y - v2.y) * (v2.x - v1.x) - (v2.y - v1.y) * (v3.x - v2.x);
}

int cross_product_z(const IntVec2 &in v1, const IntVec2 &in v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

class IntVec2
{
	
	int x, y;
	
	IntVec2() {}
	
	IntVec2(int x=0, int y=0)
	{
		this.x = x;
		this.y = y;
	}
	
	IntVec2(const Vec2 &in v)
	{
		this.x = int(v.x);
		this.y = int(v.y);
	}
	
	bool opEquals(const IntVec2 &in other) const
	{
		return x == other.x and y == other.y;
	}
	
	IntVec2& opAssign(const IntVec2 &in other)
	{
		this.x = other.x;
		this.y = other.y;
		return this;
	}
	
	IntVec2 opAdd(const IntVec2 &in other) const
	{
		return IntVec2(
			x + other.x,
			y + other.y
		);
	}
	
	IntVec2 opSub(const IntVec2 &in other) const
	{
		return IntVec2(
			x - other.x,
			y - other.y
		);
	}
	
	IntVec2 opMul(int value) const
	{
		return IntVec2(
			x * value,
			y * value
		);
	}
	
	IntVec2 opMul_r(int value) const
	{
		return IntVec2(
			x * value,
			y * value
		);
	}
	
}
