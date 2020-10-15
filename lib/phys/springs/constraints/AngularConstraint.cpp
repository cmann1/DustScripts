#include 'Constraint.cpp';

class AngularConstraint : Constraint
{
	
	Particle@ particle1;
	Particle@ particle2;
	Particle@ base_particle;
	float stiffness;
	float damping = 0.005;
	float base_angle;
	
	private float min, max;
	
	AngularConstraint(Particle@ particle1, Particle@ particle2, Particle@ base_particle, const float min, const float max, const float stiffness=0.1, const float damping=0.005)
	{
		@this.particle1 = particle1;
		@this.particle2 = particle2;
		@this.base_particle = base_particle;
		this.stiffness = stiffness;
		this.damping = damping;
		
		set_range(min, max);
	}
	
	void set_range(float min, float max)
	{
		min = normalize_angle(min * DEG2RAD);
		max = normalize_angle(max * DEG2RAD);
		this.min = (min < max ? min : max);
		this.max = (max > min ? max : min);
	}
	
	void resolve() override
	{
		float dx = particle2.x - particle1.x;
		float dy = particle2.y - particle1.y;
		
		if(@base_particle != null)
		{
			base_angle = atan2(particle1.y - base_particle.y, particle1.x - base_particle.x);
		}
		
		const float current_angle = shortest_angle(base_angle, atan2(dy, dx));
		const float min_dx = shortest_angle(current_angle, min);
		const float max_dx = shortest_angle(current_angle, max);
		
		float offset_angle;
		
		if(min_dx > 0 and max_dx < 0)
		{
			offset_angle = abs(max_dx) < min_dx ? max_dx : min_dx;
		}
		else if(min_dx > 0)
		{
			offset_angle = min_dx;
		}
		else if(max_dx < 0)
		{
			offset_angle = max_dx;
		}
		else
		{
			offset_angle = 0;
		}
		
		if(offset_angle != 0)
		{
			const float da = offset_angle * stiffness - offset_angle * damping;
			
			dx *= 0.5;
			dy *= 0.5;
			const float mx = particle1.x + dx;
			const float my = particle1.y + dy;
			rotate(dx, dy, da, dx, dy);
			particle2.x = mx + dx;
			particle2.y = my + dy;
			particle1.x = mx - dx;
			particle1.y = my - dy;
		}
	}
	
	void debug_draw(scene@ g, const float origin_x, const float origin_y) override
	{
		const float x = origin_x + particle1.x;
		const float y = origin_y + particle1.y;
		
		const float l = 24;
//		draw_line(g, 21, 21, x, y, x + cos(base_angle) * l, y + sin(base_angle) * l, 1, 0x99dddddd);
//		draw_line(g, 21, 21, x, y, x + cos(base_angle + min) * l, y + sin(base_angle + min) * l, 1, 0x99dd4444);
//		draw_line(g, 21, 21, x, y, x + cos(base_angle + max) * l, y + sin(base_angle + max) * l, 1, 0x994444dd);
	}
	
}