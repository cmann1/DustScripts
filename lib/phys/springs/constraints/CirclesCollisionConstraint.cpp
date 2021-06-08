#include 'Constraint.cpp';

/// Performs circle/circle collisions using particle radius
class CirclesCollisionConstraint : Constraint
{
	
	float stiffness = 0.15;
	array<Particle@> particles;
	
	void resolve(const float time_scale, const int iteration) override
	{
		const int count = int(particles.length);
		
		for(int i = 0; i < count - 1; i++)
		{
			Particle@ p1 = particles[i];
			
			for(int j = i + 1; j < count; j++)
			{
				Particle@ p2 = particles[j];
				float dx = p2.x - p1.x;
				float dy = p2.y - p1.y;
				float dist = dx * dx + dy * dy;
				
				if(dist < (p1.radius + p2.radius) * (p1.radius + p2.radius))
				{
					dist = sqrt(dist);
					float resting_ratio = dist != 0
						? ((p1.radius + p2.radius) - dist) / dist
						: (p1.radius + p2.radius);
					resting_ratio = resting_ratio * stiffness;
					dx *= resting_ratio;
					dy *= resting_ratio;
					p1.x -= dx;
					p1.y -= dy;
					p2.x += dx;
					p2.y += dy;
				}
			}
		}
	}
	
}
