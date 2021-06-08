#include '../../../math/geom.cpp';

#include 'Constraint.cpp';
#include 'ICollideableBox.cpp';

/// Collides particles with a set of boxes
class BoxesCollisionConstraint : Constraint
{
	
	float stiffness = 1;
	float friction;
	
	private int size_particles = 128;
	private int num_particles;
	private array<Particle@> particles(size_particles);
	private int size_boxes = 16;
	private int num_boxes;
	private array<ICollideableBox@> boxes(size_boxes);
	
	void resolve(const float time_scale, const int iteration) override
	{
		for(int i = num_particles - 1; i >= 0; i--)
		{
			Particle@ p = particles[i];
			
			for(int j = num_boxes - 1; j >= 0; j--)
			{
				ICollideableBox@ box = boxes[j];
				
				if(box.box_id == p.body_id)
					continue;
				
				float bx, by, hw, hh, rotation;
				box.get_box_properties(bx, by, hw, hh, rotation);
				rotation *= DEG2RAD;
				float c = cos(-rotation);
				float s = sin(-rotation);
				
				// Particle x/y relative to box local space
				float px = c * (p.x - bx) - s * (p.y - by);
				float py = s * (p.x - bx) + c * (p.y - by);
				
				// Not inside the box
				if(px < -hw || px > hw || py < -hh || py > hh)
					continue;
				
				float cx, cy;
				const int rect_side = closest_point_to_rect(
					px, py, -hw, -hh, hw, hh,
					cx, cy);
				
				float local_nx, local_ny;
				switch(rect_side)
				{
					case 0: local_nx = -1; local_ny =  0; break;
					case 1: local_nx = +1; local_ny =  0; break;
					case 2: local_nx =  0; local_ny = -1; break;
					case 3: local_nx =  0; local_ny = +1; break;
				}
				
				const float nx = c * local_nx - s * local_ny;
				const float ny = s * local_nx + c * local_ny;
				p.set_contact(ny, ny);
				
				// Get delta, and rotate back to world space
				cx -= px;
				cy -= py;
				c = cos(rotation);
				s = sin(rotation);
				px = c * cx - s * cy;
				py = s * cx + c * cy;
				p.x += px * stiffness;
				p.y += py * stiffness;
				
				const float x_ratio = 1 / (1 + (time_scale * DT * friction));
				p.prev_x = p.x - (p.x - p.prev_x) * x_ratio;
				p.prev_y = p.y - (p.y - p.prev_y) * x_ratio;
			}
		}
	}
	
	void add(Particle@ p)
	{
		if(num_particles + 1 >= size_particles)
			particles.resize(size_particles *= 2);
		
		@particles[num_particles++] = p;
	}
	
	void add(array<Particle@> particles)
	{
		const int count = int(particles.length);
		
		while(num_particles + count >= size_particles)
			this.particles.resize(size_particles *= 2);
		
		for(int i = 0; i < count; i++)
		{
			@this.particles[num_particles++] = particles[i];
		}
	}
	
	void add(ICollideableBox@ b)
	{
		if(num_boxes + 1 >= size_boxes)
			boxes.resize(size_boxes *= 2);
		
		@boxes[num_boxes++] = b;
	}
	
	void remove(Particle@ p)
	{
		const int index = particles.findByRef(p);
		
		if(index != -1)
		{
			@particles[index] = particles[--num_particles];
		}
	}
	
	void remove(array<Particle@> particles)
	{
		for(int i = int(particles.length) - 1; i >= 0; i--)
		{
			const int index = this.particles.findByRef(particles[i]);
			
			if(index != -1)
			{
				@this.particles[index] = this.particles[--num_particles];
			}
		}
	}
	
	void remove(ICollideableBox@ b)
	{
		const int index = boxes.findByRef(b);
		
		if(index != -1)
		{
			@boxes[index] = boxes[--num_boxes];
		}
	}
	
	void clear()
	{
		num_particles = 0;
		num_boxes = 0;
	}
	
}
