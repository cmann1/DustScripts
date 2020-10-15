#include 'Constraint.cpp';

class PositionConstraint : Constraint
{
	
	Particle@ particle;
	float x;
	float y;
	
	PositionConstraint(Particle@ particle, const float x, const float y)
	{
		@this.particle = particle;
		this.x = x;
		this.y = y;
	}
	
	void resolve() override
	{
		particle.x = x;
		particle.y = y;
	}
	
}