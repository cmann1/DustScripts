#include '../../UI.cpp';
#include '../../Style.cpp';
#include '../Graphic.cpp';

abstract class Shape : Graphic
{
	
	protected bool _use_custom_colour;
	protected uint _colour;
	
	Shape(UI@ ui)
	{
		super(ui);
		
		_use_custom_colour = false;
		
		init();
	}
	
	Shape(UI@ ui, const uint colour)
	{
		super(ui);
		
		_use_custom_colour = true;
		_colour = colour;
		
		init();
	}
	
	string element_type { get const override { return 'Shape'; } }
	
	protected void init()
	{
		
	}
	
}