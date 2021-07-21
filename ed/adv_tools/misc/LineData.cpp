class LineData
{
	
	float x1, y1;
	float x2, y2;
	float mx, my;
	float length;
	float angle;
	float inv_delta_x, inv_delta_y;
	
	bool aabb_intersection(
		const float x1, const float y1, const float x2, const float y2,
		float &out t_min, float &out t_max)
	{
		float t1 = (x1 - this.x1) * inv_delta_x;
		float t2 = (x2 - this.x1) * inv_delta_x;

		const float tt_min = min(t1, t2);
		const float tt_max = max(t1, t2);

		t1 = (y1 - this.y1) * inv_delta_y;
		t2 = (y2 - this.y1) * inv_delta_y;

		t_min = max(tt_min, min(min(t1, t2), tt_max));
		t_max = min(tt_max, max(max(t1, t2), tt_min));
		
		return t_min <= 1 && t_max >= 0 && t_max > t_min;
	}
	
}
