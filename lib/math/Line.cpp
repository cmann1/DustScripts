class Line
{
	
	float x1;
	float y1;
	float x2;
	float y2;
	
	Line() {}
	
	Line(float x1, float y1, float x2, float y2)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}
	
	bool intersection(Line other, float &out x, float &out y, float & out t)
	{
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float other_dx = other.x2 - other.x1;
		const float other_dy = other.y2 - other.y1;
		
		float det = (-other_dx * dy + dx * other_dy);
		
		if(det < EPSILON && det > -EPSILON)
		{
			x = 0;
			y = 0;
			t = 0;
			return false;
		}
		
		t = (other_dx * (y1 - other.y1) - other_dy * (x1 - other.x1)) / det;

		if(t < 0 || t > 1)
		{
			x = 0;
			y = 0;
			return false;
		}
		
		float s = (-dy * (x1 - other.x1) + dx * (y1 - other.y1)) / det;
		
		if(s < 0 || s > 1)
		{
			x = 0;
			y = 0;
			return false;
		}
		
		x = x1 + dx * t;
		y = y1 + dy * t;
		return true;
	}
	
	float closest_point(float x, float y, float &out out_x, float &out out_y)
	{
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		
		// It's a point not a line segment
		if(dx == 0 && dy == 0)
		{
			out_x = x1;
			out_y = y1;
			return 0;
		}
		
		// Calculate the t that minimizes the distance.
		const float t = ((x - x1) * dx + (y - y1) * dy) / (dx * dx + dy * dy);

		if(t <= 0)
		{
			out_x = x1;
			out_y = y1;
			return 0;
		}
	
		if(t >= 1)
		{
			out_x = x2;
			out_y = y2;
			return 1;
		}

		out_x = x1 + dx * t;
		out_y = y1 + dy * t;
		return t;
	}
	
	float distance_squared(const float x, const float y)
	{
		const float l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
		
		if(l2 == 0)
			return (x1 - x) * (x1 - x) + (y1 - y) * (y1 - y);
		
		float t = ((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / l2;
		t = max(0.0, min(1.0, t));
		
		const float dx = x1 + t * (x2 - x1) - x;
		const float dy = y1 + t * (y2 - y1) - y;
		return dx * dx + dy * dy;
	}
	
	string to_string()
	{
		return 'Line(' + x1 + ',' + y1+ ' > ' + x2 + ',' + y2 + ')';
	}
	
}