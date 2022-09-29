/// https://docs.godotengine.org/en/3.1/classes/class_@gdscript.html#class-gdscript-method-ease
float ease(float x, const double c)
{
	if(x < 0)
	{
		x = 0;
	}
	else if(x > 1.0)
	{
		x = 1.0;
	}
	
	if(c > 0)
	{
		return c < 1.0
			? 1.0 - pow(1.0 - x, 1.0 / c)
			: pow(x, c);
	}
	
	// Inout ease
	if(c < 0)
	{
		return x < 0.5
			? pow(x * 2.0, -c) * 0.5
			: (1.0 - pow(1.0 - (x - 0.5) * 2.0, -c)) * 0.5 + 0.5;
	}
	
	// No ease (raw)
	return 0; 
}
