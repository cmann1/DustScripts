#include 'Vec2.cpp';

class Rect
{
	
	float x1, y1, x2, y2;
	
	Rect() {}
	
	Rect(float x1, float y1, float x2, float y2)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}
	
	float get_width() const { return x2 - x1; }
    void set_width(float new_width) { x2 = x1 + new_width; }
	
	float get_height() const { return y2 - y1; }
    void set_height(float new_height) { y2 = y1 + new_height; }
	
	float get_centre_x() const { return (x1 + x2) * 0.5; }
    void set_centre_x(float new_x)
	{
		const float width = (x2 - x1) * 0.5;
		x1 = new_x - width;
		x2 = new_x + width;
	}
	
	float get_centre_y() const { return (y1 + y2) * 0.5; }
    void set_centre_y(float new_y)
	{
		const float height = (y2 - y1) * 0.5;
		y1 = new_y - height;
		y2 = new_y + height;
	}
	
	void set(float x1, float y1, float x2, float y2)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}
	
	void union(const Rect &in other)
	{
		if(other.x1 < this.x1)
		{
			this.x1 = other.x1;
		}
		
		if(other.y1 < this.y1)
		{
			this.y1 = other.y1;
		}
		
		if(other.x2 > this.x2)
		{
			this.x2 = other.x2;
		}
		
		if(other.y2 > this.y2)
		{
			this.y2 = other.y2;
		}
	}
	
	void intersect(const Rect &in other)
	{
		x1 = max(x1, other.x1);
		y1 = max(y1, other.y1);
		x2 = min(x2, other.x2);
		y2 = min(y2, other.y2);
	}
	
	void expand(float amount)
	{
		x1 -= amount;
		y1 -= amount;
		x2 += amount;
		y2 += amount;
	}
	
	void offset(float x, float y)
	{
		x1 += x;
		y1 += y;
		x2 += x;
		y2 += y;
	}
	
	bool overlaps_point(float x, float y)
	{
		return x <= x2 && x >= x1 && y <= y2 && y >= y1;
	}
	
	bool overlaps_point(const Vec2 &in pos)
	{
		return pos.x <= x2 && pos.x >= x1 && pos.y <= y2 && pos.y >= y1;
	}
	
	string to_string()
	{
		return '[' + x1 + ',' + y1 + ' > ' + x2 + ',' + y2 + ']';
	}
	
}