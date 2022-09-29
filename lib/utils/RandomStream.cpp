/**
 * https://blog.gskinner.com/archives/2008/01/source_code_see.html
 */
class RandomStream
{
	
	protected uint _seed = 0;
	protected uint _current_seed = 0;
	
	RandomStream(uint seed=1)
	{
		_seed = _current_seed = seed;
	}
	
	uint get_seed() const
	{
		return _seed;
	}
	
	void set_seed(uint value)
	{
		_seed = _current_seed = value;
	}
	
	uint get_current_seed() const
	{
		return _current_seed;
	}
	
	uint set_current_seed(uint value) const
	{
		return _current_seed;
	}
	
	// reset(); // resets the number series, retaining the same seed
	void reset()
	{
		_current_seed = _seed;
	}
	
	uint nexti()
	{
		return _current_seed = (_current_seed * 16807) % 2147483647;
	}
	
	// Returns a number between 0..1 exclusive.
	float nextf()
	{
		return float(_current_seed = (_current_seed * 16807) % 2147483647) / float(0x7fffffff) + 0.000000000233;
	}
	
	// Returns a number between min..max exclusive
	int rangei(const int min, const int max)
	{
		return min + nexti() % (max - min);
	}
	
	// Returns a number between 0..max exclusive
	int rangei(const int max)
	{
		return nexti() % max;
	}
	
	// Returns a number between min..max or -min..-max
	float outer_rangei(const int min, const int max)
	{
		return (min + nexti() % (max - min)) * (nextf() < 0.5 ? -1 : 1);
	}
	
	// Returns a number between min..max exclusive
	float rangef(const float min, const float max)
	{
		return min + (max - min) * nextf();
	}
	
	// Returns a number between 0..max exclusive
	float rangef(const float max)
	{
		return max * nextf();
	}
	
	// Returns a number between min..max or -min..-max
	float outer_rangef(const float min, const float max)
	{
		return (min + (max - min) * nextf()) * (nextf() < 0.5 ? -1.0 : 1.0);
	}
	
	// Returns true or false base on chance.
	bool boolean(float chance=0.5)
	{
		return nextf() < chance;
	}
	
	// Returns 1 or -1 based on chance.
	int sign(float chance=0.5)
	{
		return nextf() < chance ? 1 : -1;
	}
	
	// Returns 1 or 0 base on chance.
	int bit(float chance=0.5)
	{
		return nextf() < chance ? 1 : 0;
	}
	
}
