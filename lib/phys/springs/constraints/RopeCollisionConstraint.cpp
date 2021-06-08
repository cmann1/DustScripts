/// Treats a series of particles as connected line segments
/// preventing them from passing through each other.
/// Uses the particle radius to determine the rope thickness
/// (Note this doesn't actually connect the particles together,
/// use distance constraints for that)
class RopeCollisionConstraint : Constraint
{
	
	array<Particle@> particles;
	float strength = 0.05;
	
	private Line line;
	
	void resolve(const float time_scale, const int iteration) override
	{
		const int count = int(particles.length);
		
		for(int i = 0; i < count - 1; i++)
		{
			Particle@ p = particles[i];
			
			Particle@ p1 = particles[0];
			for(int j = 1; j < count; j++)
			{
				Particle@ p2 = particles[j];
				
				if(j != i && j - 1 != i)
				{
					line.x1 = p1.x;
					line.y1 = p1.y;
					line.x2 = p2.x;
					line.y2 = p2.y;
					float x, y;
					const float t = line.closest_point(p.x, p.y, x, y);
					const float line_radius = p1.radius + (p2.radius - p1.radius) * t;
					float dx = p.x - x;
					float dy = p.y - y;
					float dist = dx * dx + dy * dy;
					
					if(dist < (line_radius + p.radius) * (line_radius + p.radius))
					{
						dist = sqrt(dist);
						float resting_ratio = dist != 0
							? ((line_radius + p.radius) - dist) / dist
							: (line_radius + p.radius);
						resting_ratio = resting_ratio * strength;
						dx *= resting_ratio;
						dy *= resting_ratio;
						p.x += dx;
						p.y += dy;
						//p1.x -= dx;
						//p1.y -= dy;
						//p2.x -= dx;
						//p2.y -= dy;
					}
				}
				
				@p1 = p2;
			}
		}
	}
	
}
