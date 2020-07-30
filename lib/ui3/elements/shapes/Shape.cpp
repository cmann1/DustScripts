#include '../../UI.cpp';
#include '../Graphic';

abstract class Shape : Graphic
{
	
	protected bool _use_custom_colour;
	protected uint _colour;
	
	Shape(UI@ ui, const string type_identifier)
	{
		super(ui, type_identifier + 'shp');
		
		_use_custom_colour = false;
	}
	
	Shape(UI@ ui, const string type_identifier, const uint colour)
	{
		super(ui, type_identifier + 'shp');
		
		_use_custom_colour = true;
		_colour = colour;
	}
	
}