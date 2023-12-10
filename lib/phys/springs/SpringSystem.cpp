#include '../../debug/Debug.cpp'
#include '../../drawing/common.cpp'
#include '../../math/math.cpp';

#include 'Force.cpp';
#include 'Particle.cpp';
#include 'constraints/PositionConstraint.cpp';
#include 'constraints/DistanceConstraint.cpp';
#include 'constraints/AngularConstraint.cpp';
#include 'constraints/TileConstraint.cpp';

/// Basically the system used for the santa hat plugin, put together really quickly with code copied
/// from various sources on the internet.
class SpringSystem
{
	
	/// Gravity
	Force gravity(0, 48 * 9.81);
	
	/// The number of constraint iterations per frame
	int constraint_iterations = 5;
	
	private array<Force@> forces;
	private int force_count;
	private array<Particle@> particles;
	private int particle_count;
	private array<Constraint@> constraints;
	private int constraint_count;
	
	private float _drag;
	private float delta_prev = DT;
	Debug@ debug;
	
	SpringSystem(const float drag=0.99)
	{
		set_drag(drag);
	}
	
	void clear()
	{
		forces.resize(0);
		force_count = 0;
		particles.resize(0);
		particle_count = 0;
		constraints.resize(0);
		constraint_count = 0;
	}
	
	void set_drag(const float drag)
	{
		this._drag = drag;
	}
	
	float get_drag()
	{
		return this._drag;
	}
	
	Force@ add_force(const float x, const float y)
	{
		Force@ f = Force(x, y);
		forces.insertLast(f);
		force_count++;
		
		return f;
	}
	
	Particle@ add_particle(const float x, const float y)
	{
		Particle@ particle = Particle(x, y);
		particles.insertLast(particle);
		particle_count++;
		
		return particle;
	}
	
	Particle@ add_particle(Particle@ p)
	{
		particles.insertLast(p);
		particle_count++;
		
		return p;
	}
	
	Constraint@ add_constraint(Constraint@ constraint)
	{
		constraints.insertLast(constraint);
		constraint_count++;
		
		return constraint;
	}
	
	PositionConstraint@ add_constraint(PositionConstraint@ constraint)
	{
		constraints.insertLast(constraint);
		constraint_count++;
		
		return constraint;
	}
	
	DistanceConstraint@ add_constraint(DistanceConstraint@ constraint)
	{
		constraints.insertLast(constraint);
		constraint_count++;
		
		return constraint;
	}
	
	AngularConstraint@ add_constraint(AngularConstraint@ constraint)
	{
		constraints.insertLast(constraint);
		constraint_count++;
		
		return constraint;
	}
	
	TileConstraint@ add_constraint(TileConstraint@ constraint)
	{
		constraints.insertLast(constraint);
		constraint_count++;
		
		return constraint;
	}
	
	void remove_particle(Particle@ particle)
	{
		const int index = particles.findByRef(particle);
		
		if(index == -1)
			return;
		
		@particles[index] = @particles[particle_count - 1];
		particles.resize(--particle_count);
	}
	
	void remove_constraint(Constraint@ constraint)
	{
		const int index = constraints.findByRef(constraint);
		
		if(index == -1)
			return;
		
		@constraints[index] = @constraints[constraint_count - 1];
		constraints.resize(--constraint_count);
	}
	
	void step(const float time_scale)
	{
		const float delta = DT * time_scale;
		
		// Accumulate forces
		for(int i = 0; i < particle_count; i++)
		{
			Particle@ particle = particles[i];
			particle.has_contact = false;
			particle.contact_vx = 0;
			particle.contact_vy = 0;
			
			if(particle.is_static)
			{
				particle.prev_x = particle.x;
				particle.prev_y = particle.y;
				continue;
			}
			
			particle.force_x = particle.impulse_x + gravity.x;
			particle.force_y = particle.impulse_y + gravity.y;
			particle.impulse_x = 0;
			particle.impulse_y = 0;
			
			for(int j = 0; j < force_count; j++)
			{
				Force@ force = forces[j];
				particle.force_x += force.x;
				particle.force_y += force.y;
			}
			
			const float x_ratio = _drag * (1 / (1 + (time_scale * DT * particle.air_friction))) * (delta / delta_prev);
			
			// Verlet
			particle.set_position(
				particle.x + (particle.x - particle.prev_x) * x_ratio + particle.force_x * delta * delta,
				particle.y + (particle.y - particle.prev_y) * x_ratio + particle.force_y * delta * delta
			);
		}
		
		// Satisfy constraints
		for(int i = 0; i < constraint_iterations; i++)
		{
			for(int j = 0; j < constraint_count; j++)
			{
				constraints[j].resolve(time_scale, i);
			}
		}
		
		delta_prev = delta;
	}
	
	void resolve_constraints(const int constraint_iterations=4, const float time_scale=1)
	{
		for(int i = 0; i < constraint_iterations; i++)
		{
			for(int j = 0; j < constraint_count; j++)
			{
				constraints[j].resolve(time_scale, j);
			}
		}
	}
	
	void clear_particle_velocities()
	{
		for(int i = 0; i < particle_count; i++)
		{
			Particle@ particle = particles[i];
			
			particle.prev_x = particle.x;
			particle.prev_y = particle.y;
		}
	}
	
	void debug_draw(scene@ g, const float origin_x=0, const float origin_y=0)
	{
		for(int i = 0; i < constraint_count; i++)
		{
			constraints[i].debug_draw(g, origin_x, origin_y);
		}
		
		for(int i = 0; i < particle_count; i++)
		{
			particles[i].debug_draw(g, origin_x, origin_y);
		}
	}
	
}
