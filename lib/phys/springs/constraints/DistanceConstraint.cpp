#include 'Constraint.cpp';

class DistanceConstraint : Constraint
{
	
	Particle@ particle1;
	Particle@ particle2;
	float rest_length;
	float stiffness;
	float damping;
	
	DistanceConstraint(Particle@ p1, Particle@ p2, const float stiffness=1, const float damping=0, const float rest_length=-1)
	{
		@particle1 = p1;
		@particle2 = p2;
		this.stiffness = stiffness;
		this.damping = damping;
		this.rest_length = rest_length < 0 ? distance(p1.x, p1.y, p2.x, p2.y) : rest_length;
	}
	
	void resolve(const float time_scale, const int iteration) override
	{
		float dx = particle2.x - particle1.x;
		float dy = particle2.y - particle1.y;
		const float d = sqrt(dx * dx + dy * dy);
		float resting_ratio = d == 0 ? rest_length : (rest_length - d) / d;
		resting_ratio = resting_ratio * stiffness - resting_ratio * damping;
		
		dx *= resting_ratio;
		dy *= resting_ratio;
		
		if(!particle1.is_static)
		{
			particle1.x -= dx;
			particle1.y -= dy;
		}
		
		if(!particle2.is_static)
		{
			particle2.x += dx;
			particle2.y += dy;
		}
	}
	
	void debug_draw(scene@ g, const float origin_x, const float origin_y) override
	{
		draw_line(g, 21, 21,
			origin_x + particle1.x, origin_y + particle1.y,
			origin_x + particle2.x, origin_y + particle2.y, 1, 0xff44dddd);
	}
	
}
