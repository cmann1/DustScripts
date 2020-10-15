class Particle
{
	
	bool is_static;
	float x, y;
	float prev_x, prev_y;
	float force_x, force_y;
	
	Particle(const float x, const float y)
	{
		this.x = x;
		this.y = y;
		prev_x = x;
		prev_y = y;
	}
	
	void set_position(const float x, const float y)
	{
		prev_x = this.x;
		prev_y = this.y;
		this.x = x;
		this.y = y;
	}
	
	void reset(const float x, const float y)
	{
		prev_x = this.x = x;
		prev_y = this.y = y;
		force_x = force_y = 0;
	}
	
	void debug_draw(scene@ g, const float origin_x, const float origin_y)
	{
		g.draw_rectangle_world(21, 21,
			origin_x + x - 2, origin_y + y - 2,
			origin_x + x + 2, origin_y + y + 2, 45, 0xffdd4444);
	}
	
}