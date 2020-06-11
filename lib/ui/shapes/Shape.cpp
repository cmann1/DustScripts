#include '../Element.cpp';

class Shape : Element
{
	
	uint colour;
	
	Shape(UI@ ui, uint colour = 0xCCFFFFFF)
	{
		super(ui);
		
		this.colour = colour;
	}
	
}