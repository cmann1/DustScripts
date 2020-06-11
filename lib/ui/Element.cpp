#include '../math/Rect.cpp';

class Element
{
	
	UI@ ui;
	Container@ parent;
	float x;
	float y;
	float width;
	float height;
	
	Element(UI@ ui)
	{
		@this.ui = @ui;
	}
	
	void draw(scene@ g, Rect rect)
	{
		
	}
	
}