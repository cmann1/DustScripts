#include "IntVec2.cpp"
#include "math.cpp"

Vec2 round(const Vec2 &in v)
{
	return Vec2(round(v.x), round(v.y));
}

float dot(const Vec2 &in v1, const Vec2 &in v2)
{
	return dot(v1.x, v1.y, v2.x, v2.y);
}

class Vec2
{
	
	float x, y;
	
	Vec2() {}
	
	Vec2(float x=0, float y=0)
	{
		this.x = x;
		this.y = y;
	}
	
	Vec2(const IntVec2 &in v)
	{
		this.x = v.x;
		this.y = v.y;
	}
	
	Vec2(entity@ &in e)
	{
		x = e.x();
		y = e.y();
	}
	
	float magnitude() const
	{
		return sqrt(x * x + y * y);
	}
	
	float sqr_magnitude() const
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
	
	Vec2 opNeg() const
	{
		return Vec2(-x, -y);
	}
	
	Vec2 opAdd(const Vec2 &in other) const
	{
		return Vec2(
			x + other.x,
			y + other.y
		);
	}
	
	Vec2 opSub(const Vec2 &in other) const
	{
		return Vec2(
			x - other.x,
			y - other.y
		);
	}
	
	Vec2 opMul(const Vec2 &in other) const
	{
		return Vec2(
			x * other.x,
			y * other.y
		);
	}
	
	Vec2 opMul_r(float value) const
	{
		return Vec2(
			x * value,
			y * value
		);
	}
	
	Vec2 opMul(float value) const
	{
		return Vec2(
			x * value,
			y * value
		);
	}
	
	Vec2 opDiv(float value) const
	{
		return Vec2(
			x / value,
			y / value
		);
	}
	
	bool opEquals(const Vec2 &in other) const
	{
		return x == other.x and y == other.y;
	}
	
	string opConv() const
	{
		return "Vec2(" + formatFloat(x) + "," + formatFloat(y) + ")";
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
