#include '../../UI.cpp';
#include '../../Style.cpp';
#include '../Graphic.cpp';

abstract class Shape : Graphic
{
	
	protected bool _use_custom_colour;
	protected uint _colour;
	
	Shape(UI@ ui, const string type_identifier)
	{
		super(ui, type_identifier + 'shp');
		
		_use_custom_colour = false;
		
		init();
	}
	
	Shape(UI@ ui, const string type_identifier, const uint colour)
	{
		super(ui, type_identifier + 'shp');
		
		_use_custom_colour = true;
		_colour = colour;
		
		init();
	}
	
	protected void init()
	{
		
	}
	
}