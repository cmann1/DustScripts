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
		array<Particle@>@ particles = @this.particles;
		const int count = int(particles.length);
		
		for(int i = 0; i < count - 1; i++)
		{
			Particle@ p = particles[i];
			
			Particle@ p1 = particles[0];
			for(int j = 1; j < count; j++)
			{
				if(j == i || j - 1 == i)
				{
					@p1 = particles[j];
					continue;
				}
				
				Particle@ p2 = particles[j];
				
				const float radius = (p1.radius > p2.radius ? p1.radius : p2.radius) + p.radius;
				
				if(
					p.x < (p1.x < p2.x ? p1.x : p2.x) - radius ||
					p.y < (p1.y < p2.y ? p1.y : p2.y) - radius ||
					p.x > (p1.x > p2.x ? p1.x : p2.x) + radius ||
					p.x > (p1.y > p2.y ? p1.y : p2.y) + radius)
				{
					@p1 = p2;
					continue;
				}
				
				line.x1 = p1.x;
				line.y1 = p1.y;
				line.x2 = p2.x;
				line.y2 = p2.y;
				float x, y;
				const float t = line.closest_point(p.x, p.y, x, y);
				
				const float line_radius = p1.radius + (p2.radius - p1.radius) * t;
				const float dx = p.x - x;
				const float dy = p.y - y;
				float dist = dx * dx + dy * dy;
				
				if(dist < (line_radius + p.radius) * (line_radius + p.radius))
				{
					dist = sqrt(dist);
					const float resting_ratio = (dist != 0
						? ((line_radius + p.radius) - dist) / dist
						: (line_radius + p.radius)) * strength;
					p.x += dx * resting_ratio;
					p.y += dy * resting_ratio;
					//p1.x -= dx;
					//p1.y -= dy;
					//p2.x -= dx;
					//p2.y -= dy;
				}
				
				@p1 = p2;
			}
		}
	}
	
}
