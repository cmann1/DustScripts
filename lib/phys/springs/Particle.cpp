class Particle
{
	
	int body_id;
	bool is_static;
	float x, y;
	float prev_x, prev_y;
	float force_x, force_y;
	/// A force that's added and cleared to this particle each frame
	float impulse_x, impulse_y;
	
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
			origin_x + x - 3, origin_y + y - 3,
			origin_x + x + 3, origin_y + y + 3, 45, 0xffdd4444);
	}
	
}
