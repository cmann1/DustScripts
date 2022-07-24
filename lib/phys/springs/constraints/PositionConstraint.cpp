#include 'Constraint.cpp';

class PositionConstraint : Constraint
{
	
	Particle@ particle;
	float x;
	float y;
	float strength;
	
	PositionConstraint(Particle@ particle, const float x, const float y, const float strength = 1)
	{
		@this.particle = particle;
		this.x = x;
		this.y = y;
		this.strength = strength;
	}
	
	void resolve(const float time_scale, const int iteration) override
	{
		if(strength != 1)
		{
			particle.x += (x - particle.x) * strength;
			particle.y += (y - particle.y) * strength;
		}
		else
		{
			particle.x = x;
			particle.y = y;
		}
	}
	
}
