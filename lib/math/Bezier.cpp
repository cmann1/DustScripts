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
	
	int num_arcs = 10;
	array<float> arc_lengths(num_arcs + 1);
	float length;
	
	bool requires_update = true;
	
	Bezier() {}
	
	Bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int num_arcs=10)
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
	
	float x(float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * x1
			+ 3 * (t1 * t1) * t * x2
			+ 3 * t1 * (t * t) * x3
			+ (t * t * t) * x4;
    }

    float y(float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * y1
			+ 3 * (t1 * t1) * t * y2
			+ 3 * t1 * (t * t) * y3
			+ (t * t * t) * y4;
    }
	
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
	
	float mx(float distance)
	{
        return x(map(distance));
    }

    float my(float distance)
	{
        return y(map(distance));
    }
	
}