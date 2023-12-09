/**
 * Various functions for calculating basic a pseudorandom hash/noise value based on one or more inputs.
 */

/** Returns a pseudorandom value between 0 and 1 based on a single input. */
float prng(const float x)
{
	return fraction(abs(sin(x * 12.9898) * 43758.5453));
}
/** Returns a pseudorandom value between `min` and `max` based on a single input. */
float prng(const float x, const float min, const float max)
{
	return min + (max - min) * fraction(abs(sin(x * 12.9898) * 43758.5453));
}
/** Returns a pseudorandom value between negative `range` and `range` based on a single input. */
float prng(const float x, const float range)
{
	return range * (fraction(abs(sin(x * 12.9898) * 43758.5453)) * 2.0 - 1.0);
}
/** Returns a pseudorandom value between 0 and 1 based on two inputs. */
float prng2(const float x, const float y)
{
  return fraction(abs(sin(x * 12.9898 + y * 78.233)) * 43758.5453);
}
/** Returns a pseudorandom value between `min` and `max` based on two inputs. */
float prng2(const float x, const float y, const float min, const float max)
{
	return min + (max - min) * fraction(abs(sin(x * 12.9898 + y * 78.233)) * 43758.5453);
}
/** Returns a pseudorandom value between negative `range` and `range` based on two inputs. */
float prng2(const float x, const float y, const float range)
{
	return range * (fraction(abs(sin(x * 12.9898 + y * 78.233)) * 43758.5453) * 2.0 - 1.0);
}
