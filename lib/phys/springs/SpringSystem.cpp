#include '../../drawing/common.cpp'
#include '../../math/math.cpp';

#include 'Force.cpp';
#include 'Particle.cpp';
#include 'constraints/PositionConstraint.cpp';
#include 'constraints/DistanceConstraint.cpp';
#include 'constraints/AngularConstraint.cpp';

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
	private float drag_a;
	private float drag_b;
	
	SpringSystem(const float drag=0.01)
	{
		set_drag(drag);
	}
	
	void set_drag(const float drag)
	{
		this._drag = drag;
		drag_a = 2 - drag;
		drag_b = 1 - drag;
	}
	
	Force@ add_force(const float x, const float y)
	{
		Force@ f = Force(x, y);
		forces.insertLast(f);
		force_count++;
		
		return f;
	}
	
	Particle@ add_partice(const float x, const float y)
	{
		Particle@ particle = Particle(x, y);
		particles.insertLast(particle);
		particle_count++;
		
		return particle;
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
	
	void step()
	{
		// Accumulate forces
		for(int i = 0; i < particle_count; i++)
		{
			Particle@ particle = particles[i];
			
			if(particle.is_static)
				continue;
			
			particle.force_x = gravity.x;
			particle.force_y = gravity.y;
			
			for(int j = 0; j < force_count; j++)
			{
				Force@ force = forces[j];
				particle.force_x += force.x;
				particle.force_y += force.y;
			}
			
			// Verlet
			particle.set_position(
				drag_a * particle.x - particle.prev_x * drag_b + particle.force_x * DT * DT,
				drag_a * particle.y - particle.prev_y * drag_b + particle.force_y * DT * DT
			);
		}
		
		// Satisfy constraints
		for(int i = 0; i < constraint_iterations; i++)
		{
			for(int j = 0; j < constraint_count; j++)
			{
				constraints[j].resolve();
			}
		}
	}
	
	void debug_draw(scene@ g, const float origin_x=0, const float origin_y=0)
	{
		for(int i = 0; i < particle_count; i++)
		{
			particles[i].debug_draw(g, origin_x, origin_y);
		}
		
		for(int i = 0; i < constraint_count; i++)
		{
			constraints[i].debug_draw(g, origin_x, origin_y);
		}
	}
	
}