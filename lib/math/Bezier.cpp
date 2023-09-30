/// A simple bezier curve consisting of two vertices, and two control points.
class Bezier
{
	
	[position,mode:world,layer:19,y:y1]
	float x1; [hidden] float y1;
	[position,mode:world,layer:19,y:y2]
	float x2; [hidden] float y2;
	[position,mode:world,layer:19,y:y3]
	float x3; [hidden] float y3;
	[position,mode:world,layer:19,y:y4]
	float x4; [hidden] float y4;
	
	/// Can be used to track whether this curve has been modified since the last call to `update`.
	bool requires_update = true;
	
	// The approximate length of the curve. Call `update` before using.
	float length;
	
	/// Controls the precision use to approximate the curve length when calling `update`.
	int num_arcs = 10;
	array<float> arc_lengths(num_arcs + 1);
	
	Bezier() {}
	
	Bezier(
		const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
		const int num_arcs=10)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.x3 = x3;
		this.y3 = y3;
		this.x4 = x4;
		this.y4 = y4;
		this.num_arcs = num_arcs;
	}
	
	/// Approximates the length of the curve. Make sure to call after making any changes.
	/// Useful for getting points along the curve with uniform spacing using `mx` and `my`.
	/// Use `num_arcs` to controll the accuracy.
	void update()
	{
		arc_lengths[0] = 0;
		
		float ox = x(0);
		float oy = y(0);
		float clen = 0;
		const float tx = 1.0 / num_arcs;
		
		for(int i = 1; i <= num_arcs; i++) {
			float px = x(i * tx);
			float py = y(i * tx);
			float dx = ox - px;
			float dy = oy - py;        
			clen += sqrt(dx * dx + dy * dy);
			arc_lengths[i] = clen;
			ox = px;
			oy = py;
		}
		
		length = clen;
		
		requires_update = false;
	}
	
	/// Returns the x value at the given t value, where 0 >= t <= 1.
	/// Use `mx` instead for a unforim distribution across the length of the curve.
	float x(const float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * x1
			+ 3 * (t1 * t1) * t * x2
			+ 3 * t1 * (t * t) * x3
			+ (t * t * t) * x4;
    }

	/// Returns the y value at the given t value, where 0 >= t <= 1.
	/// Use `my` instead for a unforim distribution across the length of the curve.
    float y(const float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * y1
			+ 3 * (t1 * t1) * t * y2
			+ 3 * t1 * (t * t) * y3
			+ (t * t * t) * y4;
    }
	
	/// Same as `x` and `y` but returns both at the same time.
	void get_pos(const float t, float &out x, float &out y)
	{
		const float t1 = 1 - t;
		x = (t1 * t1 * t1) * x1
			+ 3 * (t1 * t1) * t * x2
			+ 3 * t1 * (t * t) * x3
			+ (t * t * t) * x4;
		y = (t1 * t1 * t1) * y1
			+ 3 * (t1 * t1) * t * y2
			+ 3 * t1 * (t * t) * y3
			+ (t * t * t) * y4;
	}
	
	/// Maps a distance along he curve to a t value.
	/// Make sure to call `update` before using.
	float map(float distance)
	{
        int low = 0, high = num_arcs;
		int index = 0;
		
        while(low < high)
		{
            index = low + ((high - low) / 2);
			
            if(arc_lengths[index] < distance)
			{
                low = index + 1;
            }
			else
			{
                high = index;
            }
        }
		
        if(arc_lengths[index] > distance)
		{
            index--;
        }
		
		if(index < 0)
			index = 0;
		else if(index >= num_arcs)
			index = num_arcs - 1;

        float lengthBefore = arc_lengths[index];
		
		return lengthBefore == distance
			? float(index) / num_arcs
			: (index + (distance - lengthBefore) / (arc_lengths[index + 1] - lengthBefore)) / num_arcs;
    }
	
	/// Returns the x value at the given distance along the curve.
	/// Make sure to call `update` before using.
	float mx(float distance)
	{
        return x(map(distance));
    }
	
	/// Returns the y value at the given distance along the curve.
	/// Make sure to call `update` before using.
    float my(float distance)
	{
        return y(map(distance));
    }
	
}
