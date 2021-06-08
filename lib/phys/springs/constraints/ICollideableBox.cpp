/// Represents a rotated rectangle particles can collide with.
/// Used in conjunction with BoxesCollisionConstraint
interface ICollideableBox
{
	
	/// Compared againts Particle::body_id to determine which particles should not collide
	/// with this box
	int get_box_id() const;
	/// Return the centre, half width/height and rotation in degrees of this box
	void get_box_properties(float &out x, float &out y, float &out hw, float &out hh, float &out rotation);
	
}
